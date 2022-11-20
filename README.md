# Wiring

| Device | Color | Esp32 | Tasmota |
| --- | --- | --- | --- |
| PMS pin 5 TX | Black | GPIO 18 | PMS5003 Rx |
| PMS pin 4 RX | Red | GPIO 19 | PMS5003 Tx |
| Screen I2C SDA | yellow | GPIO 21 | I2C SDA |
| Screen I2C SCL | green | GPIO 22 | I2C SCL |
| Button | Green | GPIO 23 | Button 1 |
| S8 TX | Green | GPIO 25 | SAir Rx |
| S8 RX | Blue | GPIO 26 | SAir Tx |
| S8+PMS Power | Red | VCC | |
| Screen Power | Red | 3.3v | |
| S8/PMS/Screen/Button ground | Black | GND | |

# Tasmota Build Flags
```c++
#undef  TEMP_CONVERSION
#define TEMP_CONVERSION   true

#undef PMS_MODEL_PMS5003T
#define PMS_MODEL_PMS5003T

#undef  USE_PMS5003
#define USE_PMS5003                              // Add support for PMS5003 and PMS7003 particle concentration sensor (+1k3 code)

#undef  USE_DISPLAY
#define USE_DISPLAY

#undef  USE_DISPLAY_SSD1306
#define USE_DISPLAY_SSD1306

#undef USE_SENSEAIR
#define USE_SENSEAIR

```

# Setup

```sh
# Configure pins
ADD
# Screen Setup
Backlog DisplayModel 2; DisplayMode 0; DisplayDimmer 100
# Configure button as input and not power
Backlog SetOption73 1; ButtonTopic 0
Rule1 on Button1#state do Br d1.next_mode() endon
Rule1 1
# PMS Set it up to sleep for 5 min between readings extend life
Sensor18 300
# Timezone
Backlog0 Timezone 99; TimeStd 0,1,11,1,2,-480; TimeDst 0,2,3,1,2,-420
# Names
Backlog Hostname OfficeAQI; DeviceName OfficeAQI; FriendlyName OfficeAQI; 
# MQTT Setup
Backlog Topic HargroveBellevue_Office_AQI; TelePeriod 60
```


