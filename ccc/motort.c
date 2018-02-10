#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>


#define PWM 1 // pwm 1pin
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
//	pinMode(ENABLE,OUTPUT);
//	pinMode(LEDT,OUTPUT);
//	pinMode(SERVO,PWM_OUTPUT);

	digitalWrite(PWM,LOW);
//	digitalWrite(DIR,HIGH);

	softPwmCreate(PWM,0,200);
//	softPwmCreate(SERVO,0,200);

//	digitalWrite(DIR,LOW);

//	softPwmWrite(PWM,200);
//	delay(2000);

//	softPwmWrite(SERVO,16);
//	softPwmWrite(PWM,100);
//	digitalWrite(PWM,HIGH);
	digitalWrite(DIR,HIGH);
//	digitalWrite(ENABLE,LOW);
//	delay(2000);

	softPwmWrite(PWM, 50);
	delay(500);

	softPwmWrite(PWM,0);

//	digitalWrite(PWM,LOW);
//	digitalWrite(DIR,LOW);
//	delay(2000);

//	softPwmWrite(PWM, 100);
//	digitalWrite(DIR,HIGH);
//	digitalWrite(ENABLE,LOW);
//	delay(2000);

//	digitalWrite(PWM,LOW);
//	softPwmWrite(SERVO, 0);
//	softPwmWrite(PWM, 0);
//	digitalWrite(DIR,LOW);


  return 0 ;
}
