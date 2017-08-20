#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>


#define PWM 21 // pwm 1pin
#define DIR 22 // 
#define ENABLE 23 // 
#define LEDT 6 // led

#define SERVO 1

int main (void)
{

	int i;
	wiringPiSetup();


	if (wiringPiSetup() == -1) {
		printf("wiringPi error");
		return 1 ;
	}

	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(ENABLE,OUTPUT);
	pinMode(LEDT,OUTPUT);
	pinMode(SERVO,PWM_OUTPUT);

	softPwmCreate(PWM,0,100);
	softPwmCreate(SERVO,0,200);

	while (1) {
	scanf("%d\n", &i);
	if (i == 0)
		break;

	softPwmWrite(SERVO,i);
	softPwmWrite(PWM,50);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,LOW);
	delay(1000);
	}
	digitalWrite(ENABLE,HIGH);

  return 0 ;
}
