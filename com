g++ -o softtones softtones.cpp -lwiringPi -lpthread && sudo ./softtones 
