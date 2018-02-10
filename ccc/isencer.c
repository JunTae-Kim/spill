#include <stdio.h>
#include <wiringPi.h>

#define PIR1 25

int main(void)
	{
		if(wiringPiSetup() == -1)
		return 1;

		pinMode(PIR1, INPUT);

		for(;;)
		{
			if(digitalRead(PIR1) == 0)
			{
			printf("Detection\n");
			delay(500);
			}
			else{
			printf("Not Detected\n");
			delay(500);
			}
		}
	return 0;
	}
