#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <poll.h>


//cc -o softpwm softpwm.c -lwiringPi -lpthread
int soundPin=29;  // ouput

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

volatile unsigned* gpio, * gpset, * gpclr, * gpin, * timer, * intrupt;
bool interruptInit=false;
#define GPIO_BASE  0x20200000
#define TIMER_BASE 0x20003000
#define INT_BASE 0x2000B000

/***************** SETUP ****************
Sets timer and interrupt pointers for future use
Does not disable interrupts
return 1 = OK
       0 = error with message print
************************************/

int interruptSetup() {
    if (interruptInit) {
      return true;
    }
    interruptInit=true;
    int memfd;
    unsigned int timend;
    void* gpio_map, * timer_map, * int_map;

    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memfd < 0) {
        printf("Mem open error\n");
        return(0);
    }

    gpio_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
        MAP_SHARED, memfd, GPIO_BASE);

    timer_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
        MAP_SHARED, memfd, TIMER_BASE);

    int_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
        MAP_SHARED, memfd, INT_BASE);

    close(memfd);

    if (gpio_map == MAP_FAILED ||
        timer_map == MAP_FAILED ||
        int_map == MAP_FAILED) {
        printf("Map failed\n");
        return(0);
    }
    // interrupt pointer
    intrupt = (volatile unsigned*)int_map;
    // timer pointer
    timer = (volatile unsigned*)timer_map;
    ++timer;    // timer lo 4 bytes
                // timer hi 4 bytes available via *(timer+1)

                // GPIO pointers
    gpio = (volatile unsigned*)gpio_map;
    gpset = gpio + 7;     // set bit register offset 28
    gpclr = gpio + 10;    // clr bit register
    gpin = gpio + 13;     // read all bits register

        // setup  GPIO 2/3 = inputs    have pull ups on board
        //        control reg = gpio + 0 = pin/10
        //        GPIO 2 shift 3 bits by 6 = (pin rem 10) * 3
        //        GPIO 3 shift 3 bits by 9 = (pin rem 10) * 3


    return(1);
}

/******************** INTERRUPTS *************

Is this safe?
Dunno, but it works

interrupts(0)   disable interrupts
interrupts(1)   re-enable interrupts

return 1 = OK
       0 = error with message print

Uses intrupt pointer set by setup()
Does not disable FIQ which seems to
cause a system crash
Avoid calling immediately after keyboard input
or key strokes will not be dealt with properly

*******************************************/

int interrupts(int flag) {
    interruptSetup();
    static unsigned int sav132 = 0;
    static unsigned int sav133 = 0;
    static unsigned int sav134 = 0;

    if (flag == 0)    // disable
    {
        if (sav132 != 0) {
            // Interrupts already disabled so avoid printf
            return(0);
        }

        if ((*(intrupt + 128) | *(intrupt + 129) | *(intrupt + 130)) != 0) {
            printf("Pending interrupts\n");  // may be OK but probably
            return(0);                       // better to wait for the
        }                                // pending interrupts to
                                         // clear

        sav134 = *(intrupt + 134);
        *(intrupt + 137) = sav134;
        sav132 = *(intrupt + 132);  // save current interrupts
        *(intrupt + 135) = sav132;  // disable active interrupts
        sav133 = *(intrupt + 133);
        *(intrupt + 136) = sav133;
    }
    else            // flag = 1 enable
    {
        if (sav132 == 0) {
            printf("Interrupts not disabled\n");
            return(0);
        }

        *(intrupt + 132) = sav132;    // restore saved interrupts
        *(intrupt + 133) = sav133;
        *(intrupt + 134) = sav134;
        sav132 = 0;                 // indicates interrupts enabled
    }
    return(1);
}



void playTone(int tone, int duration) {
  if (tone==0) {
    printf(".\n");
    delay(duration);
    return;
  }

  double hz=noteHz[tone];
  long us=(1000000/hz);

  printf("%s hz=%6.2lf us=%ld\n",noteNames[tone],hz,us);

  long elapsed=0;
  long now=millis(); 
  long audibleDuration=(duration)*3/4;
  long duty=us/2; 

  while (elapsed<audibleDuration) {
    digitalWrite(soundPin,0);
    usleep(duty);
    digitalWrite(soundPin,1);
    usleep(duty);
    elapsed=millis()-now;
  } 
  delay(duration-audibleDuration);
}

int main()
{
  if (wiringPiSetup()) {
    printf("wiringpi setup failed\n");
    return 2;
  }

  printf("waiting for interrupts to clear...\n");
  delay(2000);
  interrupts(0);

  if (piHiPri(1)) {
    printf("high priority failed\n");
    return 2;
  }
 
  setpriority(PRIO_PROCESS, getpid(), -20);

  pinMode(soundPin, OUTPUT);
  printf("speaker on pin %d\n",soundPin);
  
  for (int i=0; yankeeDoodle[i]>=0; ++i) {
    playTone(yankeeDoodle[i], 400);
  }

  playTone(2, 10000);
  interrupts(1);
}
   



