#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>

//cc -o softpwm softpwm.c -lwiringPi -lpthread
#define PIN 27
#define RANGE 512

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


void playTone(int tone, int duration) {
  if (tone==0) {
    printf(".\n");
    delay(duration);
    return;
  }

  double hz=noteHz[tone];
  long us=(1000000/hz/2)-60;

  printf("%s hz=%6.2lf us=%ld\n",noteNames[tone],hz,us);

  long elapsed=0;
  long now=millis(); 
  long audibleDuration=duration*3/4;
  
  while (elapsed<audibleDuration) {
    digitalWrite(PIN,0);
    usleep(us);
    digitalWrite(PIN,1);
    usleep(us);
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

   if (piHiPri(1)) {
     printf("high priority failed\n");
     return 2;
   }


/*
   for (int i=2; i<RANGE/2; i+=4) {
     printf("i=%d\n", i);
     if (softPwmCreate (PIN, 0, i)) {
       printf("soft pwm create failed\n");
       return 2;
     }
     softPwmWrite(PIN, 50);
     delay(1000);
   }
*/

   pinMode(PIN, OUTPUT);
   printf("speaker on pin %d\n",PIN);

   
   for (int i=0; yankeeDoodle[i]>=0; ++i) {
     playTone(yankeeDoodle[i], 400);
   }
}
   



