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

RunningStatistics inputStats_R;
RunningStatistics inputStats_S;
RunningStatistics inputStats_T;
// -------- akhir dari Sensor Tegangan ----------- //

// deklarasi awal nama fungsi (WAJIB)
void bacaTigaTegangan();

void setup() {
  Serial.begin(BAUD_RATE);
  // Properti sensor tegangan
  inputStats_R.setWindowSecs(windowLength);
  inputStats_S.setWindowSecs(windowLength);
  inputStats_T.setWindowSecs(windowLength);
}

void loop()
{
  bacaTigaTegangan();


}


// fungsi - fungsi

void bacaTigaTegangan()
{

  bacaTegangan_R = analogRead(PIN_SENSOR_TEGANGAN_R);
  inputStats_R.input(bacaTegangan_R);

  bacaTegangan_S = analogRead(PIN_SENSOR_TEGANGAN_S);
  inputStats_S.input(bacaTegangan_S);

  bacaTegangan_T = analogRead(PIN_SENSOR_TEGANGAN_T);
  inputStats_T.input(bacaTegangan_T);

  if ((unsigned long)(millis() - previousMillis) >= printPeriod) {
    previousMillis = millis();

    bacaTegangan_R = intercept + slope * inputStats_R.sigma(); //Calibartions for offset and amplitude
    bacaTegangan_R = bacaTegangan_R * (49.3231);             //Further calibrations for the amplitude

    bacaTegangan_S = intercept + slope * inputStats_S.sigma(); //Calibartions for offset and amplitude
    bacaTegangan_S = bacaTegangan_S * (49.3231);             //Further calibrations for the amplitude

    bacaTegangan_T = intercept + slope * inputStats_T.sigma(); //Calibartions for offset and amplitude
    bacaTegangan_T = bacaTegangan_T * (49.3231);             //Further calibrations for the amplitude

    Serial.print( "Voltage R: " );
    Serial.println( bacaTegangan_R );
    Serial.print( "Voltage S: " );
    Serial.println( bacaTegangan_S );
    Serial.print( "Voltage T: " );
    Serial.println( bacaTegangan_T );

  }
}
