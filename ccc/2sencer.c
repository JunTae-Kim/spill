#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
  int trig1 = 23;
  int echo1 = 24;
  int trig2 = 4;
  int echo2 = 5;
  int trig3 = 2;
  int echo3 = 3;
  int start_time1, end_time1 ;
  int start_time2, end_time2 ;
  int start_time3, end_time3 ;
  float distance1 ;
  float distance2 ;
  float distance3 ;
 
 if (wiringPiSetup() == -1) exit(1) ;

  pinMode(trig1, OUTPUT) ;
  pinMode(echo1, INPUT) ;
  pinMode(trig2, OUTPUT) ;
  pinMode(echo2, INPUT) ;
  pinMode(trig3, OUTPUT) ;
  pinMode(echo3, INPUT) ;


  while(1) {
    digitalWrite(trig1, LOW) ;
    digitalWrite(trig2, LOW) ;
    digitalWrite(trig3, LOW) ;
    delay(500) ;
    digitalWrite(trig1, HIGH) ;
    digitalWrite(trig2, HIGH) ;
    digitalWrite(trig3, HIGH) ;
    delayMicroseconds(10) ;

    digitalWrite(trig1, LOW) ;
    digitalWrite(trig2, LOW) ;
    digitalWrite(trig3, LOW) ;

    while (digitalRead(echo1) == 0) ;
    start_time1 =micros() ;
    while (digitalRead(echo2) == 0) ;
    start_time2 = micros() ;
    while (digitalRead(echo3) == 0) ;
    start_time3 = micros() ;

    while (digitalRead(echo1) == 1) ;
    end_time1 = micros() ;
    distance1 = (end_time1 - start_time1) / 29. / 2. ;
    printf("no.1 = %.2f cm\n", distance1) ;

    while (digitalRead(echo2) == 1) ;
    end_time2 = micros() ;
    distance2 = (end_time2 - start_time2) / 29. / 2. ;
    printf("no.2 = %.2f cm\n", distance2) ;

    while (digitalRead(echo3) == 1) ;
    end_time3 = micros() ;
    distance3 = (end_time3 - start_time3) / 29. / 2. ;
    printf("no.3 = %.2f cm\n", distance3) ;

/*    digitalWrite(trig2, LOW) ;
    delay(500) ; 
    digitalWrite(trig2, HIGH) ;
    delayMicroseconds(10) ;
    digitalWrite(trig2, LOW) ;
*/
 /*   while (digitalRead(echo2) == 0) ;
    start_time = micros() ;
    while (digitalRead(echo2) == 1) ;
    end_time = micros() ;
    distance = (end_time - start_time) / 29. / 2. ;
    printf("no.2 = distance %.2f cm\n", distance) ;
*/	 }
  return 0 ;
}

