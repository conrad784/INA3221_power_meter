#include <SDL_Arduino_INA3221.h>

#include <Arduino.h>
#include <U8x8lib.h>

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

SDL_Arduino_INA3221 ina3221;
// the three channels of the INA3221 named for SunAirPlus Solar Power Controller channels (www.switchdoc.com)
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3

unsigned long currentMillis = 0;
unsigned long last_refresh = 0;
unsigned long refresh_rate = 200;

float current_ma_1 = 0;
float current_ma_2 = 0;
float current_ma_3 = 0;

float mah_1 = 0;
float mah_2 = 0;
float mah_3 = 0;

void setup(void)
{

  Serial.begin(9600);
  ina3221.begin();
  
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
    mAh_1 += current_ma1 / ((1000/sample_rate) * 3600);
    mAh_2 += current_ma2 / ((1000/sample_rate) * 3600);
    mAh_3 += current_ma3 / ((1000/sample_rate) * 3600);
  }

  
  serial_print_channel(busvoltage1, shuntvoltage1, loadvoltage1, current_ma1, "CHANNEL_1");
  serial_print_channel(busvoltage2, shuntvoltage2, loadvoltage2, current_ma2, "CHANNEL_2");
  serial_print_channel(busvoltage3, shuntvoltage3, loadvoltage3, current_ma3, "CHANNEL_3");
  
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,1,"Power Monitor");

  print_consumption_line(current_ma1, mah_1, 2);
  print_consumption_line(current_ma2, mah_2, 3);
  print_consumption_line(current_ma3, mah_3, 4);
  

  const char voltage[8];
  dtostrf(busvoltage1, 3, 2, voltage);
  u8x8.drawString(0,7,voltage);
  u8x8.drawString(7,7,"V");
  

  u8x8.refreshDisplay();		// only required for SSD1606/7  
  delay(refresh_rate);
}

void print_consumption_line(float ma, float mah, int row)
{
  const char ma_char[8];
  dtostrf(ma, 3, 1, ma_char);

  const char mah_char[8];
  dtostrf(mah, 3, 2, mah_char);
   
  u8x8.drawString(0,row,ma_char);
  u8x8.drawString(5,row,"mA");
  u8x8.drawString(8,row,mah_char);
  u8x8.drawString(13,row,"mAh");
}

void serial_print_channel(float busvoltage, float shuntvoltage, float loadvoltage, float current, char* channel_name)
{
  Serial.print(channel_name); Serial.print(" Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print(channel_name); Serial.print(" Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print(channel_name); Serial.print(" Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print(channel_name); Serial.print(" Current:       "); Serial.print(current); Serial.println(" mA");
  Serial.println("");
}
