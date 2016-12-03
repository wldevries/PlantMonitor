#include <OneWire.h>


#define DS18S20_ID 0x10
#define DS18B20_ID 0x28

OneWire ds(onewirePin);

byte data[12];
byte addr[8];

// We are just looking for one temperature meter here
void lockOnTemperature() {
  Serial.println("Searching for a DS18* temperature device..");
  ds.reset_search();
  if ( !ds.search(addr)) {
      Serial.print("No onewire device found.\n");
      ds.reset_search();
      return;
  }

  Serial.print("R=");
  printHex(addr, 8);

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }

  if ( addr[0] == 0x10) {
      Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr[0] == 0x28) {
      Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr[0],HEX);
      return;
  }
}

void printHex(const byte hexes[], int count) {
  for(int i = 0; i < count; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
}

bool readTemperature(float *temperature) {
  return readTemperature(addr, temperature);
}

bool readTemperature(const byte addr[], float *temperature) {
  // Convert temperature
  ds.reset();
  ds.select(addr);
  ds.write(0x44);

  // Give the device calculation time
  delay(1000);
  
  int present = ds.reset();
  if (present < 1) {
    return false;
  }
  
  // Read Scratchpad    
  ds.select(addr);
  ds.write(0xBE);

  for (int i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  int crc = OneWire::crc8( data, 8);
  if (crc != data[8]) {
    Serial.println("CRC check failed");
  }

  *temperature = convertTemperature(addr[0], data);
  return true;
}

float convertTemperature(const byte onewireFamily, const byte scratchPad[])
{
    int otemp = ((scratchPad[1] << 8) + scratchPad[0]);
    float temp;
  
    // check for negative temperatures
    if (otemp > 2047)
    {
        // convert 2s complement negative number
        otemp = -(~(otemp - 1));
    }
    
    if (onewireFamily == DS18S20_ID)
    {
        temp = otemp / 2.0;
    }
    else if(onewireFamily == DS18B20_ID)
    {
        temp = otemp / 16.0;
    }
    return temp;
}
