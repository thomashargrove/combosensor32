#ifndef Pms5003t_h
#define Pms5003t_h

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

#include <stdint.h>

// https://5.imimg.com/data5/SELLER/Doc/2021/9/JX/MO/WE/17757918/pms-7003-dust-sensor.pdf
// Inspired by:
// 1) https://www.arduino.cc/reference/en/libraries/blinker_pmsx003st/
// 2) https://how2electronics.com/interfacing-pms5003-air-quality-sensor-arduino/
// 3) https://github.com/SuperHouse/AQS/tree/main/Firmware

struct pmWeights {
  uint16_t pm1d0;
  uint16_t pm2d5;
  uint16_t pm10d0;
};

struct particleCounts {
  uint16_t particles_0d3um;
  uint16_t particles_0d5um;
  uint16_t particles_1d0um;
  uint16_t particles_2d5um;  
};

class pms {
  public:
    pms(Stream& s);
    void sleep();
    void wakeUp();
    void requestRead();
    int read(unsigned long timeout, bool debugToSerial);
    void activeMode();
    void passiveMode();
    // CF=1, standard particle
    // I can't find good docs for this, but I believe this is adjusted for sea level 15deg C
    // Generally you should use this one
    void getStandardPM(pmWeights& w);
    int getPM2d5();
    // Under atmospheric environmennt
    void getEnvironmentalPM(pmWeights& w);
    void getParticleCounts(particleCounts& pc);
    float getTemperatureF();
    float getTemperatureC();
    float getRelativeHumidity();
  private:
    enum MODE { MODE_ACTIVE, MODE_PASSIVE };
    MODE mode = MODE_ACTIVE;
    void send(byte cmd, byte dataH, byte dataL);
    struct pms5003data {
      uint16_t framelen;
      uint16_t pm10_standard;
      uint16_t pm25_standard;
      uint16_t pm100_standard;
      uint16_t pm10_env;
      uint16_t pm25_env;
      uint16_t pm100_env;
      uint16_t particles_03um;
      uint16_t particles_05um;
      uint16_t particles_10um;
      uint16_t particles_25um;
      uint16_t temperature10x;
      uint16_t humidity10x;
      uint8_t version;
      uint8_t errorcode;
      uint16_t checksum;
    } latest_data;
    Stream& pmsSerial;
};

#endif