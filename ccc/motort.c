#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>


#define PWM 1 // pwm 1pin
#define DIR 22 

int main (void)
{

	int i;
	wiringPiSetup();


  if (wiringPiSetup() == -1) {
	printf("wiringPi error");
    return 1 ;
  }

	printf("bbbb \n");


	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);

	softPwmCreate(PWM,0,100);

//	digitalWrite(DIR,1);
//	softPwmWrite(PWM,5);
//	delay(500);

	digitalWrite(DIR,-1);
	softPwmWrite(PWM,10);
	delay(2000);

//	digitalWrite(DIR,0);
//	softPwmWrite(PWM,15);
//	delay(500);

//	softPwmWrite(PWM,0);

//	digitalWrite(PWM,LOW);
//	digitalWrite(DIR,LOW);
//	delay(2000);

	softPwmWrite(PWM, 30);
	digitalWrite(DIR,LOW);
	delay(2000);

//	digitalWrite(PWM,LOW);
	softPwmWrite(PWM, 0);
//	digitalWrite(DIR,LOW);


  return 0 ;
}
