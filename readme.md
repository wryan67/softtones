software pure tone generator
----------------------------
Generate pure tones on a standard GPIO pin while avoiding the internal PWM.  Usefull when you don't want to have any lag in your c program, which happens when shelling out to the system to play music.  Also usefull if your PWM pin is tied up processing some other functionality and/or you don't want a bulky USB card in your project.

* softtones.cpp - uses only software to genrate tones.  Since this is linux, this will always be a bit scratchy
* showclock.cpp  - a little standalone utility to check your external clock frequency
* exClockTones.cpp - generate very clear pure tones using a [DS3231](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf) external clock
* Amazon link to example DS3231 board: [Amazon Link](https://smile.amazon.com/gp/product/B07V68443F) external clock


## Prerequisite

Uses WiringPi for Raspberry Pi

## compiling

Use the com script to complie the programs, example:

    $ ./com exClockTones
    
## executing

Please use sudo to achieve real-time priority for processing the clock signal.

    $ sudo ./exClockTones
    Speaker out:  26
    playingNote  0--F4
    playingNote  1--F4
    playingNote  2--G4
    playingNote  3--A4
    playingNote  4--F4
    playingNote  5--A4
    playingNote  6--G4
    playingNote  7--.
    playingNote  8--F4
    playingNote  9--F4
    playingNote 10--G4
    playingNote 11--A4
    playingNote 12--F4
    playingNote 13--.
    playingNote 14--E4
    playingNote 15--.
    playingNote 16--F4
    playingNote 17--F4
    playingNote 18--G4
    playingNote 19--A4
    playingNote 20--B4
    playingNote 21--A4
    playingNote 22--G4
    playingNote 23--F4
    playingNote 24--E4
    playingNote 25--C4
    playingNote 26--D4
    playingNote 27--E4
    playingNote 28--F4
    playingNote 29--.
    playingNote 30--F4
    playingNote 31--.


## Circuit Diagram

![circuit diagram](https://raw.githubusercontent.com/wryan67/softtones/master/readme/circuit%20diagram.png)
