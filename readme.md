software tone generator
-----------------------
Generate pure tones on a standard GPIO pin while avoiding the internal PWM

* softtones.cpp - uses only software to genrate tones.  Since this is linux, this will always be a bit scratchy
* showclock.cpp  - a little standalone utility to check your external clock frequency
* exClockTones.cpp - generate very clear pure tones using [DS3231](https://smile.amazon.com/gp/product/B07V68443F) external clock


## Prerequisite

Uses WiringPi for Raspberry Pi


## Circuit Diagram

![circuit diagram](https://raw.githubusercontent.com/wryan67/softtones/master/readme/circuit%20diagram.png)
