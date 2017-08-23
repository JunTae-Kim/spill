#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>


#define PWM 21 // pwm 1pin
#define DIR 22 //
#define ENABLE 23 //
#define SERVO 1

int main (void)
	int main (int argc, char*argv[])
	int trig = 4;
	int echo = 5;
	int start_time, end_time;
	float distance;
	int i;
	wiringPiSetup();{


  if (wiringPiSetup() == -1){
	printf("wiringPi error");
    return 1 ;
  }
	void setup(){
	pinMode(trig, OUTPUT)
	pinMode(echo, INPUT)
	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(ENABLE,OUTPUT);
	pinMode(SERVO,PWM_OUTPUT);
	}



//	digitalWrite(PWM,HIGH);
//	digitalWrite(DIR,LOW);
//	digitalWrite(ENABLE,LOW);


	softPwmCreate(PWM,0,100);
	softPwmCreate(SERVO,0,200);


	softPwmWrite(SERVO,15);
	softPwmWrite(PWM,50);
	digitalWrite(DIR,LOW);
	digitalWrite(ENABLE,LOW);
	delay(5000);

	softPwmWrite(PWM, 50);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,LOW);
	delay(5000);
/*
	softPwmWrite(PWM, 200);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,LOW);
	delay(2000);
*/

	//stop
	softPwmWrite(SERVO, 0);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,HIGH);

  return 0 ;
}
