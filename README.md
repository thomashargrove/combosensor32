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

# Wiring / Assembly

Screen - 20m/2

1. Test screen first.  Use ssd1306_128x64_i2c.ico and change address to 0x3C
2. Cut black, red, green, yellow to 3in
3. SDA - Yellow, SCL - Green, Red - 3.3v, Black - Gnd

PMS - 5m/2

1. Cut 4.5in from tip of white connector
2. Start with black - Keep 2 inside, skip 1, keep 2, skip 3
3. Power and Gnd are reverse colors
4. Dont strip power/gnd yet

Button - 25m/2

1. 6x6x7 and 6x6x8
1. test fit first
2. snip leads one side
3. 2.25in black jumper x2
4. 2.5in green to big button
5. 3in yellow to small button

S8 - 15m/2

1. 3.5in of red, black, green, blue, yellow
2. From back.  Upper left red, down 1 black.  Upper right skip, down 1 blue, down 1 green, skip, yellow bottom

ESP32 - 50m/2

1. Flash first
1. Test Fit
1. 2in red, black
2. 

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
Backlog template {"NAME":"Combo32","GPIO":[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1696,1664,0,640,608,32,0,1632,1600,1,0,0,0,0,1,1,1,1,1,0,0,1],"FLAG":0,"BASE":1}; Module 0
# Screen Setup
Backlog DisplayModel 2; DisplayMode 0
DisplayDimmer 100
# Configure button as input and not power
Backlog SetOption73 1; ButtonTopic 0
Rule1 on Button1#state do Br comboDriver.next_mode() endon
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


