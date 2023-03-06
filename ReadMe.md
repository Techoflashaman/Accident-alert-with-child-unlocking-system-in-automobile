# Accident Alert with Child Unlocking System in Automobile

This project work on when the accident happen then the accelerometer sensor get activated and send emergency help to the emergency contact details via "call or sms" with live "location" as well as in real-time the "child lock of all doors will unlock". Also driver side door will open. 



## Components Used

### Hardware


1. ATmega 328P Microcontroller(Arduino Nano)
2. ESP8266 (Node MCU)
3. GPS Module(NEO-6M)
4. GSM Module(SIM800l)
5. Accelerometer Sensor(ADXL335)
6. L298N 2A Motor Driver Controller 
7. Servo Motor
8. BO Motor, Wheel & Chasis
9. Buzzer, Switch, LEDs
10. Resistor, Capacitor, Oscillator etc.
11. Power Supply & Wires 

### Software

1. VS Code (Arduino IDE Extension)
2. Mobile Application (To control the WiFi based car)

## Schematic 

<img src="https://firebasestorage.googleapis.com/v0/b/common-project-63634.appspot.com/o/accident%20alert%20with%20child%20unlocking%20ssystem%20in%20automobile%2Fschematic.PNG?alt=media&token=5d68757b-eae3-42cc-9818-8a0aab963231"></img>







## Result

<img src="https://firebasestorage.googleapis.com/v0/b/common-project-63634.appspot.com/o/accident%20alert%20with%20child%20unlocking%20ssystem%20in%20automobile%2Fimg1.jpg?alt=media&token=a5d079ac-68d2-43cf-90fe-a44689eb9589"></img>

<img src="https://firebasestorage.googleapis.com/v0/b/common-project-63634.appspot.com/o/accident%20alert%20with%20child%20unlocking%20ssystem%20in%20automobile%2Fimg2.jpg?alt=media&token=771ba283-156e-478f-9979-54b27f3b63a0"></img>

<img src="https://firebasestorage.googleapis.com/v0/b/common-project-63634.appspot.com/o/accident%20alert%20with%20child%20unlocking%20ssystem%20in%20automobile%2Fimg3.jpg?alt=media&token=b7954b82-40ed-4814-8d8f-c6f99b0f7161"></img>










## Code for collision detection

```javascript
/* embed */








```


## Code for WiFi controlled car

```javascript
/*
Author : Mohd Aman Ansari
embed
https://embed.org.in
*/

#define ENA   0          // Enable/speed motors Right        GPIO0(D3)
#define ENB   2          // Enable/speed motors Left         GPIO2(D4)
#define IN_1  15          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  14           // L298N in3 motors Left          GPIO14(D5)
#define IN_4  12           // L298N in4 motors Left          GPIO12(D6)  

#include <ESP8266WiFi.h>          //Include ESP8266WiFi Library
#include <WiFiClient.h>           //Include WiFiClient Library
#include <ESP8266WebServer.h>     //Include ESP8266WebServer Library

String command;             //String to store app command state.
int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;

const char* ssid = "Car";
ESP8266WebServer server(80);

void setup() {
 
 pinMode(ENA, OUTPUT);
 pinMode(ENB, OUTPUT);  
 pinMode(IN_1, OUTPUT);
 pinMode(IN_2, OUTPUT);
 pinMode(IN_3, OUTPUT);
 pinMode(IN_4, OUTPUT); 
  
  Serial.begin(115200);
  
// Connecting WiFi

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
 // Starting WEB-server 
     server.on ( "/", HTTP_handleRoot );
     server.onNotFound ( HTTP_handleRoot );
     server.begin();    
}

void goAhead(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }

void goBack(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void goRight(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }

void goLeft(){

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void goAheadRight(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar/speed_Coeff);
 
      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
   }

void goAheadLeft(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar/speed_Coeff);
  }

void goBackRight(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar/speed_Coeff);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void goBackLeft(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar/speed_Coeff);
  }

void stopRobot(){  

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
 }

void loop() {
    server.handleClient();
    
      command = server.arg("State");
      if (command == "F") goAhead();
      else if (command == "B") goBack();
      else if (command == "L") goLeft();
      else if (command == "R") goRight();
      else if (command == "I") goAheadRight();
      else if (command == "G") goAheadLeft();
      else if (command == "J") goBackRight();
      else if (command == "H") goBackLeft();
      else if (command == "0") speedCar = 400;
      else if (command == "1") speedCar = 470;
      else if (command == "2") speedCar = 540;
      else if (command == "3") speedCar = 610;
      else if (command == "4") speedCar = 680;
      else if (command == "5") speedCar = 750;
      else if (command == "6") speedCar = 820;
      else if (command == "7") speedCar = 890;
      else if (command == "8") speedCar = 960;
      else if (command == "9") speedCar = 1023;
      else if (command == "S") stopRobot();
}

void HTTP_handleRoot(void) {

if( server.hasArg("State") ){
       Serial.println(server.arg("State"));
  }
  server.send ( 200, "text/html", "" );
  delay(1);
}





