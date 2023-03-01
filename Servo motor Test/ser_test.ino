/*
Author : Mohd Aman Ansari
embed
https://embed.org.in
*/

#include <Servo.h>

Servo myservo; // create servo object to control a servo

int acc_x_pin = A0; // analog input pin for X-axis acceleration
int acc_y_pin = A1; // analog input pin for Y-axis acceleration
int acc_z_pin = A2; // analog input pin for Z-axis acceleration

int servo_pin = 9; // digital pin for servo signal

void setup() 
{
  myservo.attach(servo_pin); // attaches the servo on pin 9 to the servo object
  Serial.begin(9600); // start serial communication at 9600 bps
}

void loop() 
{
  // read accelerometer values
  int acc_x = analogRead(acc_x_pin);
  int acc_y = analogRead(acc_y_pin);
  int acc_z = analogRead(acc_z_pin);

  // calculate angle from acceleration values
  int angle = map(acc_x, 0, 1023, 0, 180);

  // set servo position based on angle
  myservo.write(angle);

  // print accelerometer values and servo position to serial monitor
  Serial.print("X-axis: ");
  Serial.print(acc_x);
  Serial.print("\tY-axis: ");
  Serial.print(acc_y);
  Serial.print("\tZ-axis: ");
  Serial.print(acc_z);
  Serial.print("\tAngle: ");
  Serial.println(angle);

  delay(100); // wait for 100ms before reading again
}
