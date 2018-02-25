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
	int t1, t2;

	if (wiringPiSetup() == -1) exit(1) ;

	pinMode(trig1, OUTPUT) ;
	pinMode(echo1, INPUT) ;
	pinMode(trig2, OUTPUT) ;
	pinMode(echo2, INPUT) ;
	pinMode(trig3, OUTPUT) ;
	pinMode(echo3, INPUT) ;


	while(1) {
		t1 = micros();
		digitalWrite(trig1, LOW) ;
		delay(10) ;
		digitalWrite(trig1, HIGH) ;
		delay(10) ;
		digitalWrite(trig1, LOW) ;

		while (digitalRead(echo1) == LOW) ;
		start_time1 =micros() ;
		while (digitalRead(echo1) == HIGH) ;
		end_time1 = micros() ;
		distance1 = (end_time1 - start_time1) / 29. / 2. ;

		digitalWrite(trig2, LOW);
		delay(10);
		digitalWrite(trig2, HIGH);
		delay(10);
		digitalWrite(trig2, LOW);

		while (digitalRead(echo2) == LOW);
		start_time2 = micros();
		while (digitalRead(echo2) == HIGH);
		end_time2 = micros();
		distance2 = (end_time2 - start_time2) / 29. / 2.;

		digitalWrite(trig3, LOW);
		delay(10);
		digitalWrite(trig3, HIGH);
		delay(10);
		digitalWrite(trig3, LOW);

		while (digitalRead(echo3) == LOW);
		start_time3 = micros();
		while (digitalRead(echo3) == HIGH);
		end_time3 = micros();
		distance3 = (end_time3 - start_time3) / 29. / 2.;


		printf("no.1 = %.2f cm     no.2 = %.2f cm     no.3 = %.2f cm \n", distance1,distance2,distance3) ;
		t2 = micros();
		printf("It took %d\n", (t2-t1)/1000);
//		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;
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
*/
	}
	return 0 ;
}

