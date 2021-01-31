#include <SDL_Arduino_INA3221.h>

#include <Arduino.h>
#include <U8x8lib.h>

//#define DEBUG_MILLIS
//#define SERIAL_OUTPUT
// can only be used with enabled serial output
//#define DEBUG_INA3221

//U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
U8X8_SSD1327_EA_W128128_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);


// default is R100
#define INA3221_ADDRESS (0x40)
#undef SHUNT_RESISTOR_VALUE
#define SHUNT_RESISTOR_VALUE (0.01)  // default shunt resistor value of 0.1 Ohm
SDL_Arduino_INA3221 ina3221(INA3221_ADDRESS, SHUNT_RESISTOR_VALUE);
// the three channels of the INA3221 named for SunAirPlus Solar Power Controller channels (www.switchdoc.com)
const static int CHANNEL_1 = 1;
const static int CHANNEL_2 = 2;
const static int CHANNEL_3 = 3;

unsigned long current_millis = 0;
unsigned long last_refresh = 0;

unsigned long refresh_rate = 100;

// keep track of used mAh
float mAh_1 = 0;
float mAh_2 = 0;
float mAh_3 = 0;

void setup(void)
{

  #ifdef SERIAL_OUTPUT
  Serial.begin(115200);
  #endif
  ina3221.begin();
  
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Static Display content
  u8x8.drawString(0,1,"Power: ");
  u8x8.drawString(5, 7, "V");
  u8x8.drawString(5, 8, "V");
  u8x8.drawString(5, 9, "V");

  u8x8.drawString(5, 2, "mA");
  u8x8.drawString(13, 2, "mAh");
  u8x8.drawString(5, 3, "mA");
  u8x8.drawString(13, 3, "mAh");
  u8x8.drawString(5, 4, "mA");
  u8x8.drawString(13, 4, "mAh");

  u8x8.drawString(5, 6, "mA");
  u8x8.drawString(13, 6, "mAh");

  u8x8.drawString(13, 11, "min");


}

void loop(void)
{
  // needed variables to calculate consumption over time
  current_millis = millis();
  unsigned long actual_millis = current_millis - last_refresh;
  last_refresh = current_millis;

  // get values from ina3221
  float busvoltage1 = ina3221.getBusVoltage_V(CHANNEL_1);
  float shuntvoltage1 = ina3221.getShuntVoltage_mV(CHANNEL_1);
  float current_mA1 = ina3221.getCurrent_mA(CHANNEL_1);
  float loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);
  
  float busvoltage2 = ina3221.getBusVoltage_V(CHANNEL_2);
  float shuntvoltage2 = ina3221.getShuntVoltage_mV(CHANNEL_2);
  float current_mA2 = ina3221.getCurrent_mA(CHANNEL_2);
  float loadvoltage2 = busvoltage2 + (shuntvoltage2 / 1000);

  float busvoltage3 = ina3221.getBusVoltage_V(CHANNEL_3);
  float shuntvoltage3 = ina3221.getShuntVoltage_mV(CHANNEL_3);
  float current_mA3 = ina3221.getCurrent_mA(CHANNEL_3);
  float loadvoltage3 = busvoltage3 + (shuntvoltage3 / 1000);
  
  /* 
     Calculate consumption of all channels in mAh
     Our interval is in milliseconds --> 3600*1000
  */
  float part_of_hour = 3600000 / actual_millis ;
  mAh_1 += current_mA1 / part_of_hour;
  mAh_2 += current_mA2 / part_of_hour;
  mAh_3 += current_mA3 / part_of_hour;

  /*
    Begin drawing to display.
   */
  //const char power[8];
  //dtostrf(busvoltage1*shuntvoltage1, 3, 2, power);
  //u8x8.drawString(7, 1, power);
  //u8x8.drawString(12, 1, "mW");

  print_consumption_line(current_mA1, mAh_1, 2);
  print_consumption_line(current_mA2, mAh_2, 3);
  print_consumption_line(current_mA3, mAh_3, 4);
  
  // global consumption
  print_consumption_line(current_mA1+current_mA2+current_mA3, mAh_1+mAh_2+mAh_3, 6);

  // last line with voltage and time
  // for me all outputs are provided with same voltage
  const char voltage_1[6];
  char voltage_2[6] = {0};
  char voltage_3[6] = {0};
  dtostrf(busvoltage1, 5, 2, voltage_1);
  dtostrf(busvoltage2, 5, 2, voltage_2);
  dtostrf(busvoltage3, 5, 2, voltage_3);
  u8x8.drawString(0, 7, voltage_1);
  u8x8.drawString(0, 8, voltage_2);
  u8x8.drawString(0, 9, voltage_3);


  float runtime_minutes = float(current_millis)/1000/60;
  char runtime_minutes_char[6] = {0};
  dtostrf(runtime_minutes, 4, 1, runtime_minutes_char);
  u8x8.drawString(9, 11, runtime_minutes_char);

  // debug millis refresh rate
  #ifdef DEBUG_MILLIS
  u8x8.clearLine(5);
  //u8x8.drawString(0, 5, "Millis: ");
  u8x8.setCursor(9, 5);
  u8x8.print(actual_millis);
  #endif

  // debug serial output
  #ifdef SERIAL_OUTPUT
  Serial.println("------------------------------");
  Serial.print("Current epoch: "); Serial.print(current_millis); Serial.print(" : "); Serial.print(runtime_minutes); Serial.println(" min");
  Serial.print("Actual Millis: "); Serial.println(actual_millis);
  serial_print_channel(busvoltage1, shuntvoltage1, loadvoltage1, current_mA1, mAh_1, "CHANNEL_1");
  serial_print_channel(busvoltage2, shuntvoltage2, loadvoltage2, current_mA2, mAh_2, "CHANNEL_2");
  serial_print_channel(busvoltage3, shuntvoltage3, loadvoltage3, current_mA3, mAh_3, "CHANNEL_3");
  #endif

  delay(refresh_rate);
}

void print_consumption_line(float mA, float mAh, int row)
{
  char mA_char[8];
  dtostrf(mA, 5, 1, mA_char);
  u8x8.drawString(0, row, mA_char);

  char mAh_char[8];
  dtostrf(mAh, 5, 1, mAh_char);
  u8x8.drawString(8, row, mAh_char);
}

void serial_print_channel(float busvoltage, float shuntvoltage, float loadvoltage, float current, float consumption, char* channel_name)
{
  Serial.print(channel_name); Serial.print(" BusVoltage:  "); Serial.print(busvoltage); Serial.println(" V");
  #ifdef DEBUG_INA3221
  Serial.print(channel_name); Serial.print(" ShuntVoltage:"); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print(channel_name); Serial.print(" LoadVoltage: "); Serial.print(loadvoltage); Serial.println(" V");
  #endif
  Serial.print(channel_name); Serial.print(" Current:     "); Serial.print(current); Serial.println(" mA");
  Serial.print(channel_name); Serial.print(" Consumption: "); Serial.print(consumption); Serial.println(" mAh");
  Serial.println("");
}
