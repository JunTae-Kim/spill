#include<stdio.h>
#include<wiringPi.h>

#define PWM 21
#define DIR 22
#define ENABLE 23
#define SERVO 1
#define trigPin 4
#define echoPin 5

int main(void)
{
if(wiringPiSetup() == -1)
return 1;

pinMode(PWM, 
