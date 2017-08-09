#include <stdio.h>
#include <wiringPi.h>

#define SERVO 1 // BCM_GPIO 24

#define RANGE 100


int main (void)
{
  char str;
  if (wiringPiSetup() == -1) {
    return 1 ;
  }

  softPwmCreate(SERVO, 0, RANGE); 

  while(1)
  {
	fputs("input u,i,o,p ,q ", stdout);
	scanf("%c", &str);
	getchar();

	if (str =='u') {
	softPwmWrite(SERVO, 15);
	delay (1000);
	softPwmWrite(SERVO, 0);
	}
	else if (str =='i') {
	softPwmWrite(SERVO, 10);
	delay (1000);
	softPwmWrite(SERVO, 0);
	}
	else if (str =='o') {
	softPwmWrite(SERVO, 20);
	delay (1000);
	softPwmWrite(SERVO, 0);
	}
	else if (str =='p') {
	softPwmWrite(SERVO, 10);
	delay (1000);
	softPwmWrite(SERVO, 0);
	}
	else if (str =='q') return 0;
  }  
  

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
    
  return 0 ;
}
