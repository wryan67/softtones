#!/bin/ksh

if [ "$1" = "" ];then
  echo usage: com PROGRAM
  exit 2
fi

PROGRAM=`basename $1 .cpp`

echo compiling $PROGRAM...
g++ -O2 -o $PROGRAM $PROGRAM.cpp -lwiringPi -lpthread 

if [ $? = 0 ];then
  sudo ./$PROGRAM 
fi
