#include <Filters.h>
#include "header.h"
#include "io_pin.h"

// Variabel

// ------- Sensor Tegangan AC ------- //
int sensorTeganganAC_R = 0;
int sensorTeganganAC_S = 0;
int sensorTeganganAC_T = 0;

float bacaTegangan_R;
float bacaTegangan_S;
float bacaTegangan_T;

const float testFrequency = 50;
const float windowLength = 40.0 / testFrequency;   // how long to average the signal, for statistist
const float intercept = -0.04;  // adjust untuk kalibrasi
const float slope = 0.0518;   // adjust untuk kalibrasi

unsigned long printPeriod = 1000;     //Refresh rate
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(BAUD_RATE);
}

void loop()
{
  RunningStatistics inputStats;
  inputStats.setWindowSecs( windowLength );

  while ( true )
  {
    bacaTegangan_R = analogRead(PIN_SENSOR_TEGANGAN_R);
    inputStats.input(bacaTegangan_R);

    if ((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis();

      bacaTegangan_R = intercept + slope * inputStats.sigma(); //Calibartions for offset and amplitude
      bacaTegangan_R = bacaTegangan_R * (49.3231);             //Further calibrations for the amplitude

      Serial.print( "Voltage: " );
      Serial.println( bacaTegangan_R );

    }
  }
}
