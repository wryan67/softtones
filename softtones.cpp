#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>

//cc -o softpwm softpwm.c -lwiringPi -lpthread
#define PIN 26
#define RANGE 512

const char *notes[10] = {
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

float noteHz[10] = {
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

   
   for (int i=0; i<10; ++i) {
     double hz=noteHz[i];
     long us=(1000000/hz/2)-60;

     printf("%s hz=%6.2lf us=%ld\n",notes[i],hz,us);

     long elapsed=0;
     long now=millis(); 
     while (elapsed<500) {
       digitalWrite(PIN,0);
       usleep(us);
       digitalWrite(PIN,1);
       usleep(us);
       elapsed=millis()-now;
     } 
   }
}
   



