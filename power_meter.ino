#include <SDL_Arduino_INA3221.h>


#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

SDL_Arduino_INA3221 ina3221;
// the three channels of the INA3221 named for SunAirPlus Solar Power Controller channels (www.switchdoc.com)
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3

unsigned long currentMillis = 0;
unsigned long last_refresh = 0;
unsigned long refresh_rate = 200;

float current_mA_1 = 0;
float current_mA_2 = 0;
float current_mA_3 = 0;

float mAh_1 = 0;
float mAh_2 = 0;
float mAh_3 = 0;

void setup(void)
{

  //Wire.begin();
  Serial.begin(9600);
  ina3221.begin();
  //Serial.println("\nI2C Scanner");
  
  u8x8.begin();
  u8x8.setPowerSave(0);
  
  
}

void loop(void)
{
  currentMillis = millis();
  Serial.println("------------------------------");
  float shuntvoltage1 = 0;
  float busvoltage1 = 0;
  float current_mA1 = 0;
  float loadvoltage1 = 0;

  busvoltage1 = ina3221.getBusVoltage_V(CHANNEL_1);
  shuntvoltage1 = ina3221.getShuntVoltage_mV(CHANNEL_1);
  current_mA1 = ina3221.getCurrent_mA(CHANNEL_1);
  loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);
  
  float shuntvoltage2 = 0;
  float busvoltage2 = 0;
  float current_mA2 = 0;
  float loadvoltage2 = 0;

  busvoltage2 = ina3221.getBusVoltage_V(CHANNEL_2);
  shuntvoltage2 = ina3221.getShuntVoltage_mV(CHANNEL_2);
  current_mA2 = ina3221.getCurrent_mA(CHANNEL_2);
  loadvoltage2 = busvoltage2 + (shuntvoltage2 / 1000);

  float shuntvoltage3 = 0;
  float busvoltage3 = 0;
  float current_mA3 = 0;
  float loadvoltage3 = 0;

  busvoltage3 = ina3221.getBusVoltage_V(CHANNEL_3);
  shuntvoltage3 = ina3221.getShuntVoltage_mV(CHANNEL_3);
  current_mA3 = ina3221.getCurrent_mA(CHANNEL_3);
  loadvoltage3 = busvoltage3 + (shuntvoltage3 / 1000);

  if(last_refresh + refresh_rate <= currentMillis) {
    last_refresh += refresh_rate;
    const float sample_rate = (float)refresh_rate;
    mAh_1 += current_mA1 / ((1000/sample_rate) * 3600);
    mAh_2 += current_mA2 / ((1000/sample_rate) * 3600);
    mAh_3 += current_mA3 / ((1000/sample_rate) * 3600);
  }

  
  Serial.print("CHANNEL_1 Bus Voltage:   "); Serial.print(busvoltage1); Serial.println(" V");
  Serial.print("CHANNEL_1 Shunt Voltage: "); Serial.print(shuntvoltage1); Serial.println(" mV");
  Serial.print("CHANNEL_1 Load Voltage:  "); Serial.print(loadvoltage1); Serial.println(" V");
  Serial.print("CHANNEL_1 Current 1:       "); Serial.print(current_mA1); Serial.println(" mA");
  Serial.println("");

  Serial.print("CHANNEL_2 Bus Voltage 2:   "); Serial.print(busvoltage2); Serial.println(" V");
  Serial.print("CHANNEL_2 Shunt Voltage 2: "); Serial.print(shuntvoltage2); Serial.println(" mV");
  Serial.print("CHANNEL_2 Load Voltage 2:  "); Serial.print(loadvoltage2); Serial.println(" V");
  Serial.print("CHANNEL_2 Current 2:       "); Serial.print(current_mA2); Serial.println(" mA");
  Serial.println("");
  
  Serial.print("CHANNEL_3 Voltage 3:   "); Serial.print(busvoltage3); Serial.println(" V");
  Serial.print("CHANNEL_3 Voltage 3: "); Serial.print(shuntvoltage3); Serial.println(" mV");
  Serial.print("CHANNEL_3 Voltage 3:  "); Serial.print(loadvoltage3); Serial.println(" V");
  Serial.print("CHANNEL_3 Current 3:       "); Serial.print(current_mA3); Serial.println(" mA");
  Serial.println("");
  
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,1,"Power Monitor");
  const char result1[8];
  const char result2[8];
  const char result3[8];
  dtostrf(current_mA1, 3, 1, result1);
  dtostrf(current_mA2, 3, 1, result2);
  dtostrf(current_mA3, 3, 1, result3);

  const char mah1_char[8];
  const char mah2_char[8];
  const char mah3_char[8];
  dtostrf(mAh_1, 3, 2, mah1_char);
  dtostrf(mAh_2, 3, 2, mah2_char);
  dtostrf(mAh_3, 3, 2, mah3_char);
  
  u8x8.drawString(0,2,result1);
  u8x8.drawString(5,2,"mA");
  u8x8.drawString(8,2,mah1_char);
  u8x8.drawString(13,2,"mAh");

  u8x8.drawString(0,3,result2);
  u8x8.drawString(5,3,"mA");
  u8x8.drawString(8,3,mah2_char);
  u8x8.drawString(13,3,"mAh");

  u8x8.drawString(0,4,result3);
  u8x8.drawString(5,4,"mA");
  u8x8.drawString(8,4,mah3_char);
  u8x8.drawString(13,4,"mAh");



  const char voltage[8];
  dtostrf(busvoltage1, 3, 2, voltage);
  u8x8.drawString(0,7,voltage);
  u8x8.drawString(7,7,"V");
  

  u8x8.refreshDisplay();		// only required for SSD1606/7  
  delay(refresh_rate);
}
