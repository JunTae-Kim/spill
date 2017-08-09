#include <wiringPi.h>  //wiringPi.h ¼±¾ð
#include <stdio.h>
#include <stdlib.h>
#include <softPwm.h>

int main (int argc, char *argv[])
{
  int pos = 180 ;
  int dir = 1 ;
  if (wiringPiSetup() == -1) exit(1) ;  //init wiringPi

  pinMode(0, OUTPUT) ;  //set the 0 pin as OUTPUT
  digitalWrite(0, LOW) ;  //0 pin output LOW voltage
  softPwmCreate(0, 0, 200) ;  //pwm initialize HIGH time 0, LOW time 200ms

  while(1) {
    pos += dir ;
    if (pos < 180 || pos > 194) dir *= -1 ;
    softPwmWrite(0, pos) ;
    delay(50) ;
  }
  return 0 ;
}

