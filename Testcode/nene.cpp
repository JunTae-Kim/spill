#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#define A 28
#define RANGE 100

void pwm(int milliseconds, int pin);

int main()
{

	if (wiringPiSetup() == -1) return 1; //wiringPi error
	int milliseconds = 0;

	pinMode(A, OUTPUT);

	while(1)
	{
		printf("Dutyrate : ");
		scanf("%d",&milliseconds);
		pwm(milliseconds, A);
	}
	
	return 0;
}
	

void pwm(int milliseconds, int pin)
{
	float range = 20000;
	float range_signal = milliseconds*100;
	for (int i= 0; i<10;i++)
	{
		digitalWrite(pin, HIGH);
		delayMicroseconds(range_signal);
		digitalWrite(pin, LOW);
		delayMicroseconds(range-range_signal);
	}
}