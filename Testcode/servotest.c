#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <softPwm.h>
#define PWM
#define RANGE 100
int main (int argc, char *argv[])
{
int pos = 180;
int dir = 1;
if (wiringPiSetup() == -1) exit(1);
pinMode(0, OUTPUT);
digitalWrite(0, LOW);
softPwmCreate(0,0,200);
while(1) {
pos += dir;
if (pos <180 || pos > 194) dir *= -1;
softPwmWrite(0,pos);
delay(50);
}
return 0;
}

