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

	printf("bbbb \n");


	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(ENABLE,OUTPUT);
	pinMode(LEDT,OUTPUT);
	pinMode(SERVO,PWM_OUTPUT);

//	digitalWrite(PWM,HIGH);
//	digitalWrite(DIR,LOW);
//	digitalWrite(ENABLE,LOW);
//	digitalWrite(LEDT,HIGH);
	

	softPwmCreate(PWM,0,100);
	softPwmCreate(SERVO,0,200);


	softPwmWrite(SERVO,15);
	softPwmWrite(PWM,50);
	digitalWrite(DIR,LOW);
	digitalWrite(ENABLE,LOW);
//	digitalWrite(LEDT,HIGH);
	delay(5000);

	softPwmWrite(PWM, 50);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,LOW);
//	digitalWrite(LEDT,HIGH);
	delay(5000);
/*
	softPwmWrite(PWM, 200);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,LOW);
	digitalWrite(LEDT,HIGH);

	delay(2000);
*/

	//stop
	softPwmWrite(SERVO, 0);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,HIGH);
	digitalWrite(LEDT,LOW);


  return 0 ;
}
