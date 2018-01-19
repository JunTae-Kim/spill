#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <softPwm.h>

#define SERVO 26
#define RANGE 200		//pwm period 20ms
#define standard 16

int main (void)
{
	int stri, k;
	if (wiringPiSetup() == -1) 
	{
		return 1 ;
 	}

	pinMode(SERVO,PWM_OUTPUT);

 	softPwmCreate(SERVO,0,RANGE);

	while(1)
	{
		fputs("input int  : ", stdout);
		scanf("%d", &stri);

		if (stri != 0) {
		softPwmWrite(SERVO, stri);
		delay (200);
		softPwmWrite(SERVO, 0);
		}

		else if (stri == 0) return 0;

		/*
		for (;;)
		{
		softPwmWrite(SERVO, 15);
		delay (1000) ;
	
		softPwmWrite(SERVO, 10);
		delay (1000) ;
    
		softPwmWrite(SERVO, 20);
		delay (1000) ;	

		softPwmWrite(SERVO, 10);
		delay (1000) ;
		}
		*/

		if (k == 27)
			break;

	}  
	return 0 ;
}

