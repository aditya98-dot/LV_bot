#include <Filters.h>
#include "header.h"
#include "io_pin.h"
#include "SoftwareSerial.h"

// Variabel dan objek
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

RunningStatistics inputStats_R; // untuk phasa R
RunningStatistics inputStats_S; // untuk phasa S
RunningStatistics inputStats_T; // untuk phasa T

// -------- akhir dari Sensor Tegangan ----------- //

// -------- SIM800C ------- //
SoftwareSerial SIM800C(4, 5);
// -------- akhir dari SIM800C --------- //

// -------- Sensor Pintu ----- //
bool statePintu;
// -------- akhir dari Sensor Pintu ----- //


// deklarasi awal nama fungsi (WAJIB)
void bacaTigaTegangan();
void kirimSMS(String pesan);
void setupSim800();



void setup() {
  Serial.begin(BAUD_RATE);
  // Properti sensor tegangan
  inputStats_R.setWindowSecs(windowLength);
  inputStats_S.setWindowSecs(windowLength);
  inputStats_T.setWindowSecs(windowLength);

  // Properti SIM800C
  SIM800C.begin(9600);
  setupSim800();

  // properti sensor pintu
  pinMode(PIN_SENSOR_PINTU, INPUT_PULLUP);
}

void loop()
{
  /*
        Alur program:
      1. baca tegangan (ketiga fasa)
      2. jika tegangan tidak memenuhi kondis (mungkin lebih dari 3 kondisi), kirim SMS dengan pesan sesuai kendala
      3. baca kondisi pintu
      4. jika terbuka kirim pesan
      5. ulang dari awal
  */

  //1. baca tegangan (ketiga fasa)
  bacaTigaTegangan();

//  2. jika tegangan tidak memenuhi kondis (mungkin lebih dari 3 kondisi), kirim SMS dengan pesan sesuai kendala
  if (bacaTegangan_R < AMBANG_BATAS_PHASA_MATI)
  {
    kirimSMS("Fase di R mati!");
    while (1);
  }

  // 3. baca kondisi pintu
  bool statePintu = digitalRead(PIN_SENSOR_PINTU);
  //  Serial.println(statePintu);

  // 4. jika terbuka kirim pesan
  if (statePintu == STATE_PINTU_TERBUKA)
  {
    kirimSMS("Pintu LVboard terbuka!");
    while (1);
  }



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

/* Paremeter
    pesan --> string (perhatikan jumlah karakter ! jangang lebih dari serial buffer 64 bytes / characters)
*/
void setupSim800 ()
{
  SIM800C.println("AT+CMGF=1");
  delay(1000);
  SIM800C.println("AT+CNMI=2,2,0,0,0");
  SIM800C.println("AT+CMGF=1");
  delay(1000);
  SIM800C.flush();
}

void kirimSMS(String pesan)
{
  String msg = pesan;

  SIM800C.println("AT+CMGS=\"081337787295\"\r" ); // nomor telepon diubah di sini!
  delay(1000);
  SIM800C.println(msg);
  delay(100);
  SIM800C.println((char)26);
  delay(1000);
}
