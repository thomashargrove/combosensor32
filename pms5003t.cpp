#include "pms5003t.h"

// https://www.superhouse.tv/38-diy-air-quality-sensor-part-1-basic-model/

pms::pms(Stream& s) : pmsSerial(s) {
}

int pms::read(unsigned long timeout, bool debugToSerial) {
  if (! pmsSerial.available()) {
    // TODO: Implement timeout
    //Serial.println("Not Avail");
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  // TODO: Implement timeout
  if(pmsSerial.peek() != 0x42) 
  {
    Serial.print("Skipping: ");
    while(pmsSerial.peek() != 0x42)
    {
      int i = pmsSerial.read();
      Serial.print(" 0x");
      Serial.print(i, HEX);
    }
    Serial.println("");
    return false;
  }
 
  // Now read all 32 bytes
  if (pmsSerial.available() < 32) {
    //Serial.println("Less than 32");
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  pmsSerial.readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
  //for (uint8_t i=2; i<32; i++) {
  //  Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  // }
  //Serial.println();
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&latest_data, (void *)buffer_u16, 30);
 
  if (sum != latest_data.checksum) {
    Serial.println("Checksum failure");
    Serial.print("Expected ");
    Serial.print(sum);
    Serial.print(" but got ");
    Serial.print(latest_data.checksum);
    Serial.println();

    return false;
  }
  // success!
  return true;
}

void pms::sleep() {
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
  pmsSerial.write(command, sizeof(command));
}

void pms::wakeUp() {
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
  pmsSerial.write(command, sizeof(command));
}

// Active mode. Default mode after power up. In this mode sensor would send serial data to the host automatically.
void pms::activeMode()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71 };
  pmsSerial.write(command, sizeof(command));
  mode = MODE_ACTIVE;
}

// Passive mode. In this mode sensor would send serial data to the host only for request.
void pms::passiveMode()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
  pmsSerial.write(command, sizeof(command));
  mode = MODE_PASSIVE;
}

// Request read in Passive Mode.
void pms::requestRead()
{
  if (mode == MODE_PASSIVE)
  {
    uint8_t command[] = { 0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71 };
    pmsSerial.write(command, sizeof(command));
  }
}

void pms::getStandardPM(pmWeights& w)
{
  w.pm1d0 = latest_data.pm10_standard;
  w.pm2d5 = latest_data.pm25_standard;
  w.pm10d0 = latest_data.pm100_standard;
}

int pms::getPM2d5()
{
  pmWeights w;
  getStandardPM(w);
  return w.pm2d5;
}

void pms::getEnvironmentalPM(pmWeights& w)
{
  w.pm1d0 = latest_data.pm10_env;
  w.pm2d5 = latest_data.pm25_env;
  w.pm10d0 = latest_data.pm100_env;
}

void pms::getParticleCounts(particleCounts& pc)
{
  pc.particles_0d3um = latest_data.particles_03um;
  pc.particles_0d5um = latest_data.particles_05um;
  pc.particles_1d0um = latest_data.particles_10um;
  pc.particles_2d5um = latest_data.particles_25um;
}

float pms::getTemperatureF()
{
  float tempC = latest_data.temperature10x / 10.0;
  return tempC * 1.8 + 32;
}

float pms::getTemperatureC()
{
  return latest_data.temperature10x / 10.0;
}

float pms::getRelativeHumidity()
{
  return latest_data.humidity10x / 10.0;
}


