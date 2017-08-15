#include <stdio.h>
#include <wiringPi.h>

#define SERVO 1 // GPIO 1�� ���
#define RANGE 200 // ��ȣ �ֱ� 20ms

int main (void)
{
  int stri;
  if (wiringPiSetup() == -1) return 1 ; //wiringPi �����ϰ��

  softPwmCreate(SERVO, 0, RANGE); 

  while(1)
  { //input 9~20 ���� �Է�
	  fputs("input int(9~20)  : ", stdout);
	  scanf("%d", &stri);

	  if (stri != 0) {
  	softPwmWrite(SERVO, stri); //�Է��� ������ �����̰�
  	delay (1000);
	  softPwmWrite(SERVO, 0); //����
	  }
    else if (stri == 0) return 0;
  }

  return 0 ;
}

