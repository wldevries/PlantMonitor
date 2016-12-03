#include <SPI.h>
#include <WiFi101.h>
#include <RTCZero.h>

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;

void connectToAP() {
  Serial.println("Connect Wifi");
  Serial.println("Current Wifi status: ");
  Serial.println(WiFi.status());
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 1 second for connection:
    delay(3000);
    status = WiFi.status();
  }

  Serial.println("Current Wifi status after connect: ");
  Serial.println(status);
}

void disconnectFromAP() {
  Serial.println("Disconnect Wifi");
  WiFi.disconnect();
  WiFi.end();
}

const int maxRetries = 3;
int sendRetries = 0;
void sendValue(char const * sensorName, uint32_t timestamp, float value) {
  // Can't get reconnect to work
  //connectToAP();
  
  Serial.print("Connect to graphite .. ");
  sendRetries = 0;
  while (!client.connect(graphiteServer, graphitePort) && sendRetries++ < maxRetries)  {
    //Serial.println("Current Wifi status: ");
    //Serial.println(WiFi.status());
    Serial.println();
    Serial.print("Cannot reach graphite server, retrying .. try ");
    Serial.println(sendRetries);
  }

  if (sendRetries > maxRetries) {
    Serial.println("Giving up .. :-(");
    return;
  }

  sendValues(sensorName, value, timestamp);
  client.flush();
  client.stop();
  
  Serial.println("sent sensor data:");
  printValues(sensorName, value, timestamp);
  
  // Can't get reconnect to work...
  //delay(5000);  
  //disconnectFromAP();
}

void printValues(const char name[], float value, uint32_t timestamp) {
  Serial.print(name);
  Serial.print(" ");  
  Serial.print(value);
  Serial.print(" ");
  Serial.print(timestamp);
  Serial.println();
}

void sendValues(const char name[], float value, uint32_t timestamp) {
  client.print(name);
  client.print(" ");  
  client.print(value);
  client.print(" ");
  client.print(timestamp);
  client.println();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

