PlantMonitor
============
A sketch for the Arduino MKR1000.

Required hardware:
- Arduino MKR1000
- Humidity sensor
- Maxim onewire temperature sensor

It will read the sensor values of both sensors and send them to a Graphite 
Carbon time series database.

To get started you need to create a file Constants.h that includes constant 
variables that will change from implementation to implementation.

```c
const char ssid[] = "ssid";      //  your network SSID (name)
const char pass[] = "password";       // your network password

const int sensorPin = A1;    // select the input pin for the potentiometer
const int ledPin = 6;      // select the pin for the LED

const char sensorName[] = "path.to.the.humidity.recording.path";

const char graphiteServer[] = "mygraphiteserver";
const int graphitePort = 2003;
```