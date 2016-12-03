//#include <SPI.h>

#include <WiFi101.h>
#include <WiFiUdp.h>

#include <NTPClient.h>
#include <RTCZero.h>

#include "Constants.h"

int sensorValue = 0;  // variable to store the value coming from the sensor

bool sendRequest = true; // used to understand if the http request must be sent

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

/* Create an rtc object */
RTCZero rtc;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  connectToAP();
  printWifiStatus();
  Serial.println();

  timeClient.begin();
  while (!timeClient.update()) {
    Serial.println("failed to retrieve time");
    delay(500);
  }

  Serial.println("ntp time received:");
  Serial.println(timeClient.getEpochTime());
  Serial.println(timeClient.getFormattedTime());
  Serial.println();

  Serial.println("Setting RTC");
  rtc.begin();
  rtc.setEpoch(timeClient.getEpochTime());

  rtc.setAlarmTime(0, 0, 0);    //in this way the request is sent every minute at 0 seconds
  rtc.enableAlarm(rtc.MATCH_SS);

  rtc.attachInterrupt(alarmMatch);
  Serial.println();

  // Can't get reconnect to work...
  // disconnectFromAP();
}

void alarmMatch() {
  Serial.println("alarm went off");
  sendRequest = true;
}

void loop() {
  if (sendRequest) {
    digitalWrite(ledPin, HIGH);
    sensorValue = analogRead(sensorPin);  
    sendValue(sensorName, rtc.getEpoch(), sensorValue);
    sendRequest = false;
    delay(100);
    digitalWrite(ledPin, LOW);
  }
}

