#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM 1
#define DIR 22
#define RANGE 100

int main(int argc, char*argv[])
{
	int stri;
	int trig = 23;
	int echo = 24;
	int start_time, end_time;
	float distance;

	if (wiringPiSetup() == -1)
	{
		return 1; //wiringPi error
	}

	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);

	digitalWrite(DIR, HIGH);

	softPwmCreate(PWM, 0, RANGE);

	fputs("input int : ", stdout);
	scanf("%d", &stri);

	while(1)
	{
		softPwmWrite(PWM, stri);

		digitalWrite(trig, LOW);
		delay(10);
		digitalWrite(trig, HIGH);
		delay(10);
		digitalWrite(trig, LOW);

		while(digitalRead(echo) == 0);
		start_time = micros();
		while(digitalRead(echo) == 1);
		end_time = micros();
		distance = (end_time - start_time) / 29. / 2.;
		printf("distance %.2f cm\n", distance);

		if ( distance <= 100) {
			digitalWrite(PWM, 0);
			delay(500);
			digitalWrite(DIR, LOW);
			digitalWrite(PWM, 50);
			delay(1000);
		}

	}
	return 0;
}

