#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <softServo.h>

#define RCSERVO 12

int main(void)
{

	int i;
	int in;
	if (wiringPiSetupGpio() == -1)
		return 1;

	softServoSetup(RCSERVO, -1, -1, -1, -1, -1, -1, -1);

//	pinMode(PWM, PWM_OUTPUT);
//	pinMode(DIR, OUTPUT);

//	softServoSetup(RCSERVO, -1, -1, -1, -1, -1, -1, -1);

//	digitalWrite(PWM, LOW);
//	softPwmCreate(PWM, 0, 200);

	while(1)
	{

//	softPwmWrite(PWM, 50);
//	digitalWrite(DIR, HIGH);

	printf("input value : ");
	scanf("%d",&in);

	if (in == -1)
		break;

	softServoWrite(RCSERVO, in);
	delay(400);
	}

//	softPwmWrite(PWM, 0);
}

