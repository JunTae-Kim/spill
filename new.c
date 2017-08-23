#include<stdio.h>
#include<wiringPi.h>
#include<softPwm.h>


#define PWM 21
#define DIR 22
#define ENABLE 23
#define SERVO 1
#define trigPin 4
#define echoPin 5

int main(void)
{
/*
int distance=0;
int pulse=0;
*/
if(wiringPiSetup() == -1)
return 1;

pinMode(PWM, OUTPUT);
pinMode(DIR, OUTPUT);
pinMode(ENABLE, OUTPUT);
pinMode(SERVO, OUTPUT);

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

/*
for(;;)
*/

while(1)
{
digitalWrite(trigPin, LOW);
delay(500);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

while(digitalRead(echoPin) == LOW);

long startTime = micros();
while(digitalRead(echoPin) == HIGH);
long travelTime = micros() - startTime;

int distance = travelTime/58;

printf("distance;%dcm\n", distance);
delay(100);

softPwmCreate(PWM,0,100);
softPwmCreate(SERVO,0,200);

digitalWrite(PWM, HIGH);
digitalWrite(DIR, LOW);
digitalWrite(ENABLE, HIGH);
digitalWrite(SERVO, LOW);

if(distance<=30)
{
digitalWrite(PWM, LOW);
digitalWrite(DIR, LOW);
digitalWrite(ENABLE, LOW);
digitalWrite(SERVO, LOW);
delay(1000);
}
}
}


