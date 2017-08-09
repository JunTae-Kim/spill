#include <stdio.h>
#include <wiringPi.h>

#define SERVO 1 // BCM_GPIO 24

#define RANGE 100


int main (void)
{
  int stri;
  if (wiringPiSetup() == -1) {
    return 1 ;
  }

  softPwmCreate(SERVO, 0, RANGE); 

  while(1)
  {
	fputs("input int  : ", stdout);
	scanf("%d", &stri);

	if (stri != 0) {
	softPwmWrite(SERVO, stri);
	delay (1000);
	softPwmWrite(SERVO, 0);
	}

	else if (stri == 0) return 0;
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
