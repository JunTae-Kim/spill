#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM 1
#define DIR 22
#define RANGE 100

int main()
{
	int stri;
	if (wiringPiSetup() == -1) 
	{
		return 1; //wiringPi error
	}

	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);

	digitalWrite(DIR, HIGH);

	softPwmCreate(PWM,0,RANGE);

	while (1) 
	{
		fputs("input int  : ", stdout);
		scanf("%d", &stri);

		if (stri != 0) {
		softPwmWrite(PWM, stri);
		delay (2000);
		softPwmWrite(PWM, 0);

		}

		else if (stri == 0) return 0;

	} // While End 

	return 0;
}
