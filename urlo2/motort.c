#include <stdio.h>
#include <wiringPi.h>


#define PWM 21 // BCM_18
#define DIR 22 // BCM_23
#define ENABLE 23 // BCM_24
#define LEDT 6 // led

// #define SERVO 1

int main (void)
{

	int i;
	wiringPiSetup();


  if (wiringPiSetup() == -1) {
	printf("erroraaa");
    return 1 ;
  }
	printf("errorbb");


	pinMode(PWM,OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(ENABLE,OUTPUT);
	pinMode(LEDT,OUTPUT);

//	digitalWrite(PWM,HIGH);
//	digitalWrite(DIR,LOW);
//	digitalWrite(ENABLE,LOW);
//	digitalWrite(LEDT,HIGH);

	delay(1500);

	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,LOW);
	digitalWrite(LEDT,HIGH);
	analogWrite(PWM,100);
	delay(100);

	digitalWrite(DIR,LOW);
	digitalWrite(ENABLE,LOW);
	digitalWrite(LEDT,HIGH);
	analogWrite(PWM,100);
	delay(1500);


	analogWrite(PWM,200);
	digitalWrite(DIR,HIGH);
	digitalWrite(ENABLE,HIGH);
	digitalWrite(LEDT,LOW);

	delay(1500);



	digitalWrite(PWM,LOW);
	digitalWrite(DIR,LOW);
	digitalWrite(ENABLE,LOW);
	digitalWrite(LEDT,LOW);


  return 0 ;
}
