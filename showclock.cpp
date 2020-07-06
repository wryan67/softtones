#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define ClockInPin     25
#define ClockInPinBCM  26

int spiHandle;
static volatile int   sampleClockCounter = 0;

pthread_t threadCreate(void *(*method)(void *), const char *description) {
	pthread_t threadId;
	int status= pthread_create(&threadId, NULL, method, NULL);
	if (status != 0) {
		printf("%s::thread create failed %d--%s\n", description, status, strerror(errno));
		exit(9);
	}
	pthread_detach(threadId);
	return threadId;
}


unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return
        (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}

void* sampleTPS(void*) {

  delay(10);

  while (1) {
    sampleClockCounter=0;
    delay(1000); 
    printf("clock freq %dhz\n", sampleClockCounter);     
  }
}


void* takeSamplePolling(void*) {
    struct pollfd pfd;
    int    fd;
    char   buf[128];

    sprintf(buf, "gpio export %d in", ClockInPinBCM);
    system(buf);
    sprintf(buf, "/sys/class/gpio/gpio%d/value", ClockInPinBCM);

    if ((fd = open(buf, O_RDONLY)) < 0) {
        fprintf(stderr, "Failed, gpio %d not exported.\n", ClockInPinBCM);
        exit(1);
    }

    pfd.fd = fd;
    pfd.events = POLLPRI;

    char lastValue = 0;
    int  xread = 0;

    lseek(fd, 0, SEEK_SET);    /* consume any prior interrupt */
    read(fd, buf, sizeof buf);

    while (true) {
        //  poll(&pfd, 1, -1);         /* wait for interrupt */
        lseek(fd, 0, SEEK_SET);    /* consume interrupt */
        xread = read(fd, buf, sizeof(buf));

        if (xread > 0) {
            if (buf[0] != lastValue) {
                ++sampleClockCounter;
                lastValue = buf[0];
            }
        }
    }
}

bool setup() {
  if (int ret = wiringPiSetup()) {
    fprintf(stderr, "Wiring Pi setup failed, ret=%d\n", ret);
    exit(EXIT_FAILURE);
  }

  if ((spiHandle = wiringPiSPISetup(0, 1000000)) < 0) {
    fprintf(stderr, "opening SPI bus failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }



  return true;
}

int main()
{
  if (!setup()) {
    return 2;
  }
 
  printf("clock input is on pin %d\n", ClockInPin);
  threadCreate(takeSamplePolling, "takeSamplePoling");
  threadCreate(sampleTPS, "sampleTPS");

  
  while (true) {
    fflush(stdout);
    delay(1000);
  }
}
