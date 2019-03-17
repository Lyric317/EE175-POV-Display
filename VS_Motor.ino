// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       VS_Motor.ino
    Created:	3/5/2019 21:17:46
    Author:     NIKOSURBOOK\liuyi
*/

#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//initialize adafruit motorshield as serial port
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

// The setup() function runs once each time the micro-controller starts
void setup()
{
	
	AFMS.begin();
	Serial.begin(9600);

}

// Add the main program code into the continuous loop() function
void loop()
{
	int statInput = 0;
	statInput = Serial.read();//read data from serial port
	if(statInput == 52)
	{
		myMotor->setSpeed(52); //set running speed at 52 (0-255, 255 is full speed)
		myMotor->run(FORWARD);
	}
	else
	{
		myMotor->setSpeed(0);
		myMotor->run(FORWARD);
	}
	

}
