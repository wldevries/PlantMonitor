//#include <SPI.h>

#include <WiFi101.h>
#include <WiFiUdp.h>

#include <NTPClient.h>
#include <RTCZero.h>

#include "Constants.h"

bool sendRequest = true; // used to understand if the http request must be sent

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

/* Create an rtc object */
RTCZero rtc;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(humidityVccPin, OUTPUT);
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
  Serial.println("Setting RTC");
  rtc.begin();
  rtc.setEpoch(timeClient.getEpochTime());

  rtc.setAlarmTime(0, 0, 0);    //in this way the request is sent every minute at 0 seconds
  rtc.enableAlarm(rtc.MATCH_SS);

  rtc.attachInterrupt(alarmMatch);
  Serial.println();

  // Can't get reconnect to work...
  // disconnectFromAP();

  lockOnTemperature();
}

void alarmMatch() {
  Serial.print(rtc.getEpoch());
  Serial.println(": alarm went off");
  sendRequest = true;
}

int humidity = 0;  // variable to store the value coming from the sensor
float temperature = 0;

void loop() {
  if (sendRequest) {
    digitalWrite(ledPin, HIGH);

    humidity = readHumidity();
    sendValue(humidityName, rtc.getEpoch(), humidity);

    if (readTemperature(&temperature)) {
      sendValue(temperatureName, rtc.getEpoch(), temperature);
    } else
    {
      Serial.println("failed to read temperature");
    }
    
    sendRequest = false;
    delay(100);
    digitalWrite(ledPin, LOW);
  }
}

int readHumidity() {
  int humidity = 0;
  digitalWrite(humidityVccPin, HIGH);
  delay(20);
  humidity = analogRead(sensorPin);
  digitalWrite(humidityVccPin, LOW);
  return humidity;
}
