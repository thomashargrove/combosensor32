#ifndef aqi_h
#define aqi_h

int convertPm25ToUsAqi(int pm25_standard);
const char* getLevelOfHealthConcern(int aqi);
int usaEpaStandardPm2d5Adjustment(int pm25_standard, int relative_humidity);

#endif