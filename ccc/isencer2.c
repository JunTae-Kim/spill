#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PIR1 25
#define PWM 1
#define DIR 22

int main(void)
	{
		if(wiringPiSetup() == -1)
		return 1;

		pinMode(PWM, PWM_OUTPUT);
		pinMode(DIR, OUTPUT);
		pinMode(PIR1, INPUT);

		digitalWrite(PWM,LOW);

		softPwmCreate(PWM, 0, 100);
		digitalWrite(DIR, HIGH);
		softPwmWrite(PWM, 10);
		for(;;)
		{
			if(digitalRead(PIR1) == 0)
			{
			printf("Detection\n");
			softPwmWrite(PWM, 5);
			delay(500);
			}
			else{
			printf("Not Detected\n");
			delay(500);
			}
		}

	return 0;
	}
