/*
 * Author : Mohd Aman Ansari
 * Date : 26/01/2023
 * embed
 * https://embed.org.in
 */

#include <TinyGPS++.h> //Include TinyGPS++ Library
// #include <AltSoftSerial.h>     //Include AltSoftSerial Library
#include <SoftwareSerial.h>
#include <math.h>

#include <Servo.h>

// emergency phone number with country code
const String EMERGENCY_PHONE = "+916394915220";

// GSM Module RX pin to Arduino 3
// GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin, txPin);

// GPS Module RX pin to Arduino 9
// GPS Module TX pin to Arduino 8
// AltSoftSerial neogps;
TinyGPSPlus gps;

String sms_status, sender_number, received_date, msg;
String latitude, longitude;

#define BUZZER 12 // Buzzer at microcontroller 12 PIN(Arduino UNO)
#define BUTTON 11 // Button at microcontroller 11 PIN(Arduino UNO)
#define LEDR 5    // LED at microcontroller 5 PIN(Arduino UNO)
#define LEDL 6    // LED at microcontroller 6 PIN(Arduino UNO)
#define LED_red 7 // LED at microcontroller 7 PIN(Arduino UNO)

#define xPin A1 // ADXL accelerometer sensor(x,y,z) Pin at microcontroller A1,A2,A3 PIN(Arduino UNO)
#define yPin A2
#define zPin A3

Servo myservo; // create a Servo object
int pos = 0;   // variable to store the servo position

byte updateflag;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 110; // set sensitivity of ADXL345 accelerometer sensor
double angle;
boolean impact_detected = false;
// Used to run impact routine every 2mS.
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 10000; // 10 seconds

void setup()
{

  Serial.begin(9600); // Serial.println("Arduino serial initialize");
  sim800.begin(9600); // Serial.println("SIM800L serial initialize");
  //  neogps.begin(9600);    //Serial.println("NEO6M serial initialize");

  pinMode(BUZZER, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDL, OUTPUT);
  pinMode(LED_red, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  sms_status = "";
  sender_number = "";
  received_date = "";
  msg = "";

  sim800.println("AT"); // Check GSM Module
  delay(1000);
  // SendAT("AT", "OK", 2000); //Check GSM Module
  sim800.println("ATE1"); // Echo ON
  delay(1000);
  // SendAT("ATE1", "OK", 2000); //Echo ON
  sim800.println("AT+CPIN?"); // Check SIM ready
  delay(1000);
  // SendAT("AT+CPIN?", "READY", 2000); //Check SIM ready
  sim800.println("AT+CMGF=1"); // SMS text mode
  delay(1000);
  // SendAT("AT+CMGF=1", "OK", 2000); //SMS text mode
  sim800.println("AT+CNMI=1,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  // SendAT("AT+CNMI=1,1,0,0,0", "OK", 2000); //set sms received format
  // AT +CNMI = 2,1,0,0,0 - AT +CNMI = 2,2,0,0,0 (both are same)

  time1 = micros();
  // Serial.print("time1 = "); Serial.println(time1);

  // read calibrated values. otherwise false impact will trigger
  // when you reset your Arduino. (By pressing reset button)
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  myservo.attach(9); // attaches the servo on pin 9
}

void loop()
{

  // call impact routine every 2mS
  if (micros() - time1 > 1999)
    Impact();

  if (updateflag > 0)
  {
    updateflag = 0;
    Serial.println("Impact detected!!");
    Serial.print("Magnitude:");
    Serial.println(magnitude);

    //    getGps();
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED_red, HIGH);
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDL, HIGH);

    myservo.write(90); // Move the servo to 90 degrees

    impact_detected = true;
    impact_time = millis();
  }

  if (impact_detected == true)
  {
    if (millis() - impact_time >= alert_delay)
    {
      digitalWrite(BUZZER, LOW);
      digitalWrite(LED_red, LOW);
      // digitalWrite(LEDR, LOW);
      // digitalWrite(LEDL, LOW);

      // myservo.write(0);  // Move the servo back to 0 degrees

      makeCall();
      delay(1000);
      sendAlert();
      impact_detected = false;
      impact_time = 0;
    }
  }

  if (digitalRead(BUTTON) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_red, LOW);
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDL, LOW);

    myservo.write(0); // Move the servo back to 0 degrees

    impact_detected = false;
    impact_time = 0;
  }

  while (sim800.available())
  {
    parseData(sim800.readString());
  }

  while (Serial.available())
  {
    sim800.println(Serial.readString());
  }
}

void Impact() // Impact() function

{

  time1 = micros(); // resets time value

  int oldx = xaxis; // store previous axis readings for comparison
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  // loop counter prevents false triggering. Vibration resets if there is an impact. Don't detect new changes until that "time" has passed.
  vibration--;
  // Serial.print("Vibration = "); Serial.println(vibration);
  if (vibration < 0)
    vibration = 0;
  // Serial.println("Vibration Reset!");

  if (vibration > 0)
    return;

  deltx = xaxis - oldx;
  delty = yaxis - oldy;
  deltz = zaxis - oldz;

  // Magnitude to calculate force of impact.
  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));

  if (magnitude >= sensitivity) // impact detected
  {
    updateflag = 1;
    // reset anti-vibration counter
    vibration = devibrate;
  }

  else
  {
    // if (magnitude > 15)
    // Serial.println(magnitude);
    // reset magnitude of impact to 0
    magnitude = 0;
  }
}

void parseData(String buff) // ParseData() Function
{
  Serial.println(buff);

  unsigned int len, index;

  // Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();

  if (buff != "OK")
  {

    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);
    // Serial.println(buff);

    if (cmd == "+CMTI")
    {
      // get newly arrived memory location and store it in temp
      // temp = 4
      index = buff.indexOf(",");
      String temp = buff.substring(index + 1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      // AT+CMGR=4 i.e. get message stored at memory location 4
      sim800.println(temp);
    }

    else if (cmd == "+CMGR")
    {
      // extractSms(buff);
      // Serial.println(buff.indexOf(EMERGENCY_PHONE));
      if (buff.indexOf(EMERGENCY_PHONE) > 1)
      {
        buff.toLowerCase();
        // Serial.println(buff.indexOf("get gps"));
        if (buff.indexOf("get gps") > 1)
        {
          //          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;

          sendSms(sms_data);
        }
      }
    }
  }
  else
  {
    // The result of AT Command is "OK"
  }
}

/*

void getGps()   //GetGPS() Function
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){

    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }

  if (newData) //If newData is true
  {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  }
  else {
    Serial.println("No GPS data is available");
    latitude = "";
    longitude = "";
  }

  Serial.print("Latitude= "); Serial.println(latitude);
  Serial.print("Lngitude= "); Serial.println(longitude);
}

*/

void sendAlert() // SendAlert() Function
{
  String sms_data;
  sms_data = "Accident Alert!!! All child lock with driver door is unlock!!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:"; // Link for GPS locaion
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}

void makeCall() // MakeCall() Function
{
  Serial.println("calling....");
  sim800.println("ATD" + EMERGENCY_PHONE + ";");
  delay(20000); // 20 sec delay
  sim800.println("ATH");
  delay(1000); // 1 sec delay
}

void sendSms(String text) // SendSMS() Function
{
  // return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + EMERGENCY_PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); // ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}

boolean SendAT(String at_command, String expected_answer, unsigned int timeout) // SendAT() Function
{

  uint8_t x = 0;
  boolean answer = 0;
  String response;
  unsigned long previous;

  // Clean the input buffer
  while (sim800.available() > 0)
    sim800.read();

  sim800.println(at_command);

  x = 0;
  previous = millis();

  // this loop waits for the answer with time out
  do
  {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (sim800.available() != 0)
    {
      response += sim800.read();
      x++;
      // check if the desired answer (OK) is in the response of the module
      if (response.indexOf(expected_answer) > 0)
      {
        answer = 1;
        break;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}