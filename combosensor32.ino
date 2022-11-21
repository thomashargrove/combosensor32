/*
 Required Libraries
   Adafruit_SSD1306 - https://github.com/adafruit/Adafruit_SSD1306

*/

// Screen
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Guide to making custom fonts here:
//   Fonts - https://github.com/adafruit/Adafruit-GFX-Library/tree/master/Fonts
// Format in file:
//   /* {offset, width, height, advance cursor, x offset, y offset} */
// Some mono-space fonts:
//   https://draculatheme.com/blog/best-free-fonts-for-programming
#include <Fonts/FreeMono12pt7b.h>
//   https://www.fontsquirrel.com/fonts/Liberation-Mono
#include "LiberationMono-Regular9pt7b.h"
#include "LiberationMono-Bold9pt7b.h"
// https://www.dafont.com/seven-segment.font
#include "Seven_Segment34pt7b.h"

// PMS5003 Sensor
#include "pms5003t.h"
#include "aqi.h"
#define PMS5003_RX_PIN 18
#define PMS5003_TX_PIN 19
pms* pmsSensor;

// SenseAir S8 CO2 Sensor
#include <s8_uart.h>
#define S8_RX_PIN 25
#define S8_TX_PIN 26
S8_UART *sensor_S8;
S8_sensor sensor;

// Button
#define BUTTON_PIN 23


void setup_pms5003() {
  Serial2.begin(9600, SERIAL_8N1, PMS5003_RX_PIN, PMS5003_TX_PIN);
  pmsSensor = new pms(Serial2);
}

void setup_s8() {
  Serial1.begin(9600, SERIAL_8N1, S8_RX_PIN, S8_TX_PIN);
  sensor_S8 = new S8_UART(Serial1);
  // Check if S8 is available
  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
    // TODO: Update screen
    Serial.println("SenseAir S8 CO2 sensor not found!");
    while (1) { delay(1); };
  }
  Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
  printf("Firmware version: %s\n", sensor.firm_version);
  sensor.sensor_id = sensor_S8->get_sensor_ID();
  Serial.print("Sensor ID: 0x"); 
  printIntToHex(sensor.sensor_id, 4); 
  Serial.println("");
}

void setup_screen() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.cp437(true);
  display.println("Init...");
  display.display();
}

void setup_button() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), on_button_press, FALLING);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Init");

  setup_screen();
  setup_pms5003();
  setup_s8();
  setup_button();
}

// Globals for sensor values
int aqi = 0;
int co2 = 0;
int temperature = 0;
int humidity = 0;

void mode0() {
  display.clearDisplay();
  display.setTextSize(1);
  
  int base = 12;
  int numstart = 64;

  display.setFont(&LiberationMono_Regular9pt7b);
  display.setCursor(0, base + 0);
  display.print("CO2:");
  
  display.setCursor(0, base + 16);
  display.print("Aqi:");

  display.setCursor(0, base + 32);
  display.print("Temp:");
  display.setCursor(0, base + 48);
  display.print("%RH:");

  display.setFont(&LiberationMono_Bold9pt7b);
  display.setCursor(numstart, base + 0);
  display.printf("%4d", co2);
  
  display.setCursor(numstart, base + 16);
  display.printf("%4d", aqi);  

  display.setCursor(numstart, base + 32);
  display.printf("%4d", temperature);

  display.setCursor(numstart, base + 48);
  display.printf("%4d", humidity);

  display.display();
}

void mode1() {
  display.clearDisplay();
  display.setFont(&Seven_Segment34pt7b);
  display.setCursor(0, 16+47);
  display.print("0123");
  display.display();
}

volatile int mode = 0;
volatile int last_button = 0;
void on_button_press() {
  int now = millis();
  if ((now - last_button) > 300) {
    mode += 1;
    if (mode == 2) {
      mode = 0;
    }
    last_button = now;
  }
}

long last_co2_time = 0;
void loop() {
  long now = millis();
  int ret = pmsSensor->read(900, true);
  if(ret>0)
  {
    int pm2d5 = pmsSensor->getPM2d5();
    humidity = pmsSensor->getRelativeHumidity();
    int pm2d5adjusted = usaEpaStandardPm2d5Adjustment(pm2d5, humidity);
    aqi = convertPm25ToUsAqi(pm2d5adjusted);
    temperature = pmsSensor->getTemperatureF();
    
  }
  if ((last_co2_time == 0) || ((now - last_co2_time) > 5000))
  {
    co2 = sensor_S8->get_co2();
    last_co2_time = now;
  }

  if (mode == 0) {
    mode0();
  } else if (mode == 1) {
    mode1();
  }

  delay(100);
}
