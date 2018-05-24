#include <stdio.h>
#include <wiringPi.h>

#define PIR 25

int main(void)
	{
		if(wiringPiSetup() == -1)
		return 1;

		pinMode(PIR, INPUT);

		for(;;){
			if(digitalRead(PIR) == 0)
			{
			printf("NOT Detected\n");
			delay(200);
			}
			else{
			printf("Detection\n");
			delay(200);
			}
		}
	return 0;
	}
