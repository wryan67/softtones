g++ -O2 -o softtones softtones.cpp -lwiringPi -lpthread && sudo ./softtones 
