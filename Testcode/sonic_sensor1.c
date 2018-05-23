#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM 1
#define DIR 22
#define RANGE 100

int main(int argc, char *argv[])
{
	int stri;
	int trig1 = 23;
	int trig2 = 4;
	int trig3 = 2;
	int echo1 = 24;
	int echo2 = 5;
	int echo3 = 3;
	int tag1, tag2, tag3 = 0;
	int start_time1, end_time1;
	int start_time2, end_time2;
	int start_time3, end_time3;
	float distance1;
	float distance2;
	float distance3;
	int t1, t2 = 0;

	if (wiringPiSetup() == -1)
	{
		return 1; //wiringPi error
	}

	pinMode(PWM, PWM_OUTPUT);
	pinMode(DIR, OUTPUT);

	pinMode(trig1, OUTPUT);
	pinMode(echo1, INPUT);
	pinMode(trig2, OUTPUT);
	pinMode(echo2, INPUT);
	pinMode(trig3, OUTPUT);
	pinMode(echo3, INPUT);

	digitalWrite(DIR, HIGH);

	softPwmCreate(PWM, 0, RANGE);

	fputs("input int : ", stdout);
	scanf("%d", &stri);

	while(1)
	{
		t1 = micros();
//		fputs("input int : ", stdout);
//		scanf("%d", &stri);

//		if (stri != 0) {
			softPwmWrite(PWM, stri);
//			delay(2000);
//			softPwmWrite(PWM, 0);
//			}
//		else if (stri == 0) return 0;

		digitalWrite(trig1, LOW);
		digitalWrite(trig2, LOW);
		digitalWrite(trig3, LOW);
		delay(10);
		digitalWrite(trig1, HIGH);
		digitalWrite(trig2, HIGH);
		digitalWrite(trig3, HIGH);
		delay(10);

		digitalWrite(trig1, LOW);
		digitalWrite(trig2, LOW);
		digitalWrite(trig3, LOW);

		while(1)
		{
			if (digitalRead(echo1) == 1) {
				if (tag1 == 0) {
					start_time1 = micros();
					tag1 = 1;
				}
			}
			else if (digitalRead(echo1) == 0 && tag1 == 1) {
				end_time1 = micros();
				tag1 = 2;
			}

			if (digitalRead(echo2) == 1) {
				if (tag2 == 0) {
					start_time2 = micros();
					tag2 = 1;
				}
			}
			else if (digitalRead(echo2) == 0 && tag2 == 1) {
				end_time2 = micros();
				tag2 = 2;
			}

			if (digitalRead(echo3) == 1) {
				if (tag3 == 0) {
					start_time3 = micros();
					tag3 = 1;
				}
			}
			else if (digitalRead(echo3) == 0 && tag3 == 1) {
				end_time3 = micros();
				tag3 = 2;
			}

			if (tag1 == 2 && tag2 == 2 && tag3 == 2) {
				break;
			}

//		printf("%d \t %d \t %d \n", tag1, tag2, tag3);
//		printf("%d \t %d \t %d \n", digitalRead(echo1), digitalRead(echo2), digitalRead(echo3));
		}

		tag1 = 0;
		tag2 = 0;
		tag3 = 0;

		distance1 = (end_time1 - start_time1) / 29. / 2.;
		distance2 = (end_time2 - start_time2) / 29. / 2.;
		distance3 = (end_time3 - start_time3) / 29. / 2.;

		if ( distance1 <= 100) {
			digitalWrite(PWM, 0);
			delay(300);
			digitalWrite(DIR, LOW);
			digitalWrite(PWM, 100);
			delay(1000);
			return 0;
		}

		printf("no.1 = %.2f cm\t", distance1);
		printf("no.2 = %.2f cm\t", distance2);
		printf("no.3 = %.2f cm\n", distance3);

		t2 = micros();
		printf("It tooks %d\n", (t2-t1)/1000);
	}
	return 0;
}
