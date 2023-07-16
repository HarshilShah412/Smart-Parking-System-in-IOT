#include <ESP8266WiFi.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const char *ssid = "OnePlus 8";  // Enter your WiFi Name
const char *pass = "Rahul1517Mansi";      // Enter your WiFi Password

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "Rahul1512"
#define MQTT_PASS "aio_iTTL448tLaw1NTpGJKH00G32i0aO"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

Servo Entryservo;  //servo as gate
Servo Exitservo;   //servo as gate

int carEnter = D5;   // entry sensor
int carExited = D1;  //exit sensor
int slot3 = D2;
int slot2 = D3;
int slot1 = D4;
int count = 0;

int CLOSE_ANGLE = 0;   // The closing angle of the servo motor arm
int OPEN_ANGLE = 180;  // The opening angle of the servo motor arm

int hh, mm, ss;
int pos;
int pos1;

String h, m, EntryTimeSlot1, ExitTimeSlot1, EntryTimeSlot2, ExitTimeSlot2, EntryTimeSlot3, ExitTimeSlot3;
boolean entrysensor, exitsensor, s1, s2, s3;

boolean s1_occupied = false;
boolean s2_occupied = false;
boolean s3_occupied = false;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe EntryGate = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/EntryGate");
Adafruit_MQTT_Subscribe ExitGate = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/ExitGate");

//Set up the feed you're publishing to
Adafruit_MQTT_Publish CarsParked = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/CarsParked");
Adafruit_MQTT_Publish EntrySlot1 = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/EntrySlot1");
Adafruit_MQTT_Publish ExitSlot1 = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/ExitSlot1");
Adafruit_MQTT_Publish EntrySlot2 = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/EntrySlot2");
Adafruit_MQTT_Publish ExitSlot2 = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/ExitSlot2");
Adafruit_MQTT_Publish EntrySlot3 = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/EntrySlot3");
Adafruit_MQTT_Publish ExitSlot3 = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/ExitSlot3");

void setup() {
  delay(1000);
  Serial.begin(9600);
  mqtt.subscribe(&EntryGate);
  mqtt.subscribe(&ExitGate);
  timeClient.begin();

  Entryservo.attach(D6);  // servo pin to D6
  Exitservo.attach(D0);   // servo pin to D5

  pinMode(carExited, INPUT);
  pinMode(carEnter, INPUT);

  pinMode(slot1, INPUT);
  pinMode(slot2, INPUT);
  pinMode(slot3, INPUT);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.print(ssid);
  Serial.println("IP Address is : ");
  Serial.println(WiFi.localIP());  //print local IP address
}

void loop() {
  MQTT_connect();

  timeClient.update();

  entrysensor = !digitalRead(carEnter);
  exitsensor = !digitalRead(carExited);
  s1 = digitalRead(slot1);
  s2 = digitalRead(slot2);
  s3 = digitalRead(slot3);

  if (entrysensor == 1) {
    if (s1_occupied == false || s2_occupied == false || s3_occupied == false) {
      count = count + 1;  //increment count
      Serial.println("Entry Door Open.");
      Entryservo.write(OPEN_ANGLE);
      delay(3000);
      Entryservo.write(CLOSE_ANGLE);
      Serial.println("Entry Door Closed.");
    }
  }

  if (exitsensor == 1) {
    count = count - 1;
    Serial.println("Exit Door Open.");
    Exitservo.write(OPEN_ANGLE);
    delay(3000);
    Exitservo.write(10);
    Serial.println("Exit Door Closed.");
  }
  CarsParked.publish(count);

  if (s1 == 0 && s1_occupied == false) {

    Serial.println("Occupied1 ");
    timeClient.update();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    h = String(hh);
    m = String(mm);
    h + " :" + m;

    EntryTimeSlot1 = h + " :" + m;

    //ExitTimeSlot1 = "0 : 0";
    Serial.println(EntryTimeSlot1);

    s1_occupied = true;
    EntrySlot1.publish((char *)EntryTimeSlot1.c_str());
  }

  if (s1 == 1 && s1_occupied == true) {
    Serial.println("Available1 ");
    timeClient.update();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    h = String(hh);
    m = String(mm);
    h + " :" + m;

    ExitTimeSlot1 = h + " :" + m;
    Serial.println(EntryTimeSlot1);

    s1_occupied = false;
    ExitSlot1.publish((char *)ExitTimeSlot1.c_str());
  }

  if (s2 == 0 && s2_occupied == false) {
    Serial.println("Occupied2 ");
    timeClient.update();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    h = String(hh);
    m = String(mm);
    h + " :" + m;

    EntryTimeSlot2 = h + " :" + m;
    //ExitTimeSlot2 = "0 : 0";
    Serial.println(EntryTimeSlot1);

    s2_occupied = true;
    EntrySlot2.publish((char *)EntryTimeSlot2.c_str());
  }

  if (s2 == 1 && s2_occupied == true) {
    Serial.println("Available2 ");
    timeClient.update();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    h = String(hh);
    m = String(mm);
    h + " :" + m;

    ExitTimeSlot2 = h + " :" + m;
    Serial.println(EntryTimeSlot1);

    s2_occupied = false;
    ExitSlot2.publish((char *)ExitTimeSlot2.c_str());
  }

  if (s3 == 0 && s3_occupied == false) {
    Serial.println("Occupied3 ");
    timeClient.update();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    h = String(hh);
    m = String(mm);
    h + " :" + m;

    EntryTimeSlot3 = h + " :" + m;
    //ExitTimeSlot3 = "0 : 0";
    Serial.println(EntryTimeSlot1);

    s3_occupied = true;
    EntrySlot3.publish((char *)EntryTimeSlot3.c_str());
  }

  if (s3 == 1 && s3_occupied == true) {
    Serial.println("Available3 ");
    timeClient.update();
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();
    h = String(hh);
    m = String(mm);
    h + " :" + m;

    ExitTimeSlot3 = h + " :" + m; 
    Serial.println(EntryTimeSlot1);

    s3_occupied = false;
    ExitSlot3.publish((char *)ExitTimeSlot3.c_str());
  }

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &EntryGate) {
      Serial.println((char *)EntryGate.lastread);

      if (!strcmp((char *)EntryGate.lastread, "OPEN")) {
        Entryservo.write(OPEN_ANGLE);
      }
    }

    if (subscription == &EntryGate) {
      Serial.println((char *)EntryGate.lastread);

      if (!strcmp((char *)EntryGate.lastread, "CLOSE")) {
        Entryservo.write(CLOSE_ANGLE);
      }
    }

    if (subscription == &ExitGate) {
      Serial.println((char *)ExitGate.lastread);

      if (!strcmp((char *)ExitGate.lastread, "OPEN")) {
        Exitservo.write(OPEN_ANGLE);
      }
    }

    if (subscription == &ExitGate) {
      Serial.println((char *)ExitGate.lastread);

      if (!strcmp((char *)ExitGate.lastread, "CLOSE")) {
        Exitservo.write(CLOSE_ANGLE);
      }
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      while (1)
        ;
    }
  }
}