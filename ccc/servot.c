#include <stdio.h>
#include <wiringPi.h>

#define SERVO 26 // GPIO 1번 사용
#define RANGE 200 // 신호 주기 20ms

int main (void)
{
  int stri;
  if (wiringPiSetup() == -1) return 1 ; //wiringPi 오류일경우

  softPwmCreate(SERVO, 0, RANGE); 

  while(1)
  { //input 13~19 각도 입력
	  fputs("input int(13~19)  : ", stdout);
	  scanf("%d", &stri);

	  if (stri != 0) {
  	softPwmWrite(SERVO, stri); //입력한 각도로 움직이고
  	delay (1000);
	softPwmWrite(SERVO,0);
	  }
    else if (stri == 0) return 0;
  }

  return 0 ;
}

