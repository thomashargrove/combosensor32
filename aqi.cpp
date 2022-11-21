#include <Arduino.h>

float map_double(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int convertPm25ToUsAqi(int pm25_standard)
{
  // https://forum.airnowtech.org/t/the-aqi-equation/169
  if (pm25_standard <= 12) {
    return map_double(pm25_standard, 0, 12, 0, 50);
  } else if (pm25_standard <= 35.4) {
    return map_double(pm25_standard, 12.1, 35.4, 51, 100);
  } else if (pm25_standard <= 55.4) {
    return map_double(pm25_standard, 35.5, 55.4, 101, 150);
  } else if (pm25_standard <= 150.4) {
    return map_double(pm25_standard, 55.5, 150.4, 151, 200);
  } else if (pm25_standard <= 250.4) {
    return map_double(pm25_standard, 150.5, 250.4, 201, 300);
  } else if (pm25_standard <= 500.4) {
    return map_double(pm25_standard, 250.5, 500.4, 301, 500);
  } else {
    return 500;
  }
}

/*
float pm25table[7][4] = {
  {0.0,    12.0,   0,  50},
  {12.1,   35.4,  51, 100},
  {35.5,   55.4, 101, 150},
  {55.5,  150.4, 151, 200},
  {150.5, 250.4, 201, 300},
  {250.5, 500.4, 301, 500}
};

int convertPm25ToUsAqi(float pm25_standard) {
  // https://forum.airnowtech.org/t/the-aqi-equation/169
  // First round to 0.1
  float pm25round = ((int)(pm25_standard * 10.0)) / 10.0;
  for(int level=0; level<7; ++level) {
    if (pm25round >= pm25table[level][0] && pm25round <= pm25table[level][1]) {
      float pm25min = pm25table[level][0];
      float pm25max = pm25table[level][1];
      float aqimin = pm25table[level][2];
      float aqimax = pm25table[level][3];
      float pct = (pm25round-pm25min)/(pm25max-pm25min);
      float aqi = pct*(aqimax-aqimin)+aqimin;
      return (int)aqi;
    }
  }
  return 500;
}
*/

const char* getLevelOfHealthConcern(int aqi) {
  // https://www.epa.gov/outdoor-air-quality-data/air-data-basic-information#:~:text=An%20AQI%20value%20of%20100,generally%20thought%20of%20as%20satisfactory.
  if (aqi<=50) {
    return "Good";
  } else if (aqi <=100) {
    return "Moderate";
  } else if (aqi <= 150) {
    return "Unhealthy for Sensitive Groups";
  } else if (aqi <= 200) {
    return "Unhealthy";
  } else if (aqi <= 300) {
    return "Very Unhealthy";
  } else {
    return "Hazardous";
  }
}

// This gives more accurate data for forest fire smoke.  PurpleAir gives you this conversion option labeled "US EPA"
// https://cfpub.epa.gov/si/si_public_record_report.cfm?dirEntryId=353088&Lab=CEMM
// file:///Users/thargrove/Downloads/SENSORDATACLEANINGANDCORRECTIONAPPLICATIONONTHEAIRNOWFIREANDSMOKEAPP%20(1)%20(2).PDF
/*
Copy-paste from the PDF
y={0 ≤ x <30: 0.524*x - 0.0862*RH + 5.75}
y={30≤ x <50: (0.786*(x/20 - 3/2) + 0.524*(1 - (x/20 - 3/2)))*x -0.0862*RH + 5.75}
y={50 ≤ x <210: 0.786*x - 0.0862*RH + 5.75}
y={210 ≤ x <260: (0.69*(x/50 – 21/5) + 0.786*(1 - (x/50 – 21/5)))*x - 0.0862*RH*(1 - (x/50 – 21/5)) + 2.966*(x/50 – 21/5) + 5.75*(1 - (x/50 – 21/5)) + 8.84*(10^{-4})*x^{2}*(x/50 – 21/5)}
y={260 ≤ x: 2.966 + 0.69*x + 8.84*10^{-4}*x^2}

y= corrected PM2.5 µg/m3
x= PM2.5 cf_atm (lower)
RH= Relative humidity as measured by the PurpleAir
*/
int usaEpaStandardPm2d5Adjustment(int pm25_standard, int relative_humidity)
{
  // Rename so we can copy-paste above
  float x = pm25_standard;
  float RH = relative_humidity;
  if (x<30) {
    return 0.524 * x - 0.0862 * RH + 5.75;
  } else if(x<50) {
    return (0.786 * (x/20.0 - 3.0/2.0) + 0.524 * (1.0 - (x/20.0 - 3.0/2.0))) * x - 0.0862 * RH + 5.75;
  } else if(x<210) {
    return 0.786 * x - 0.0862 * RH + 5.75;
  } else if(x<260) {
    return (0.69 * (x/50.0 - 21.0/5.0) + 0.786 * (1.0 - (x/50.0 - 21.0/5.0))) * x - 0.0862 * RH * (1.0 - (x/50.0 - 21.0/5.0)) + 2.966 * (x/50.0 - 21.0/5.0) + 5.75 * (1.0 - (x/50.0 - 21.0/5.0)) + 8.84 * pow(10.0, -4.0) * pow(x,2.0) * (x/50.0 - 21.0/5.0);
  } else {
    return 2.966 + 0.69 * x + 8.84 * pow(10.0, -4.0) * pow(x, 2.0);
  }
}