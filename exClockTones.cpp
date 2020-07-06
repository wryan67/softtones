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
#define SpearkerOut  26

#define CLOCK_FREQ 65536

int spiHandle;
static volatile double currentDuration=999 *1000; // us
static volatile double clockTick=1000000.0/CLOCK_FREQ; // us
static volatile long  sampleClockCounter = 0;
static volatile long  swipe = 0;
static volatile int   flap=0;
static volatile int   playingNote=-1;
static volatile long  noteDuration= 400 * 1000; // us

 
int yankeeDoodle[99] = {
  5,5,6,7,5,7,6,0,5,5,6,7,5,0,4,0,5,5,6,7,8,7,6,5,4,2,3,4,5,0,5,0,-1
};

const char *noteNames[11] = {
  ".",
  "G3",
  "C4",
  "D4",
  "E4",
  "F4",
  "G4",
  "A4",
  "B4",
  "C4",
  "D5"
};

float noteHz[11] = {
  0,        // rest
  195.9977, // G3
  261.6256, // C4
  293.6648, // D4
  329.6276, // E4
  349.2282, // F4
  391.9954, // G4
  440.0000, // A4
  493.8833, // B4
  523.2511, // C5
  587.3295, // D5
};


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


void play() {
  
  currentDuration+=clockTick;
  long cd = currentDuration+0.5;
  
  if (cd>noteDuration) {
     currentDuration=0.0;
     if (yankeeDoodle[++playingNote]<0) {
       exit(0);
     }
     float currentNote=noteHz[yankeeDoodle[playingNote]];
     long  usNote=(1000000/currentNote);
     swipe= usNote/2/clockTick;
     printf("playingNote %2d--%s\n", playingNote, noteNames[yankeeDoodle[playingNote]]); fflush(stdout);
  }

  ++sampleClockCounter;
  if (sampleClockCounter%swipe==0) {
    flap=!flap;
    if (currentDuration<noteDuration*3/4) {
      digitalWrite(SpearkerOut, flap);
    }
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
                lastValue = buf[0];
//              if ((0x01&lastValue)==1) play();
                play();
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

  pinMode(SpearkerOut, OUTPUT);

  return true;
}

int main()
{
  if (!setup()) {
    return 2;
  }

  piHiPri(99);

  printf("Speaker out:  %d\n", SpearkerOut);


  threadCreate(takeSamplePolling, "takeSamplePoling");
//threadCreate(sampleTPS, "sampleTPS");

  
  while (true) {
    fflush(stdout);
    delay(1000);
  }
}
