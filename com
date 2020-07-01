g++ -o softpwm softpwm.cpp -lwiringPi -lpthread && sudo ./softpwm 
