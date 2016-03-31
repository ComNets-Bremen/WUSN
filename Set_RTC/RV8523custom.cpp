/*
  RV8523custom RTC Lib for Arduino
  by Watterott electronic (www.watterott.com)
 */

#include <inttypes.h>
#if defined(__AVR__)
# include <avr/io.h>
#endif
#if ARDUINO >= 100
# include "Arduino.h"
#else
# include "WProgram.h"
#endif
#include <Wire.h>
#include "RV8523custom.h"


#define I2C_ADDR (0xD0>>1)


//-------------------- Constructor --------------------


RV8523custom::RV8523custom(void)
{
  Wire.begin();

  return;
}


//-------------------- Public --------------------
void RV8523custom::set_second(uint8_t second)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x03));
  Wire.write(bin2bcd(second));
  Wire.endTransmission();

  return;
}

void RV8523custom::set_minute(uint8_t minute)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x04));
  Wire.write(bin2bcd(minute));
  Wire.endTransmission();

  return;
}

void RV8523custom::set_hour(uint8_t hour)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x05));
  Wire.write(bin2bcd(hour));
  Wire.endTransmission();

  return;
}

void RV8523custom::set_day(uint8_t day)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x06));
  Wire.write(bin2bcd(day));
  Wire.endTransmission();

  return;
}

void RV8523custom::set_Weekday(uint8_t weekday)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x07));
  Wire.write(bin2bcd(weekday));
  Wire.endTransmission();
  
  return;
}

void RV8523custom::get_Weekday(uint8_t *weekday)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x07));
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDR, 1);
  *weekday = bcd2bin(Wire.read() & 0x07); //day of week
  return;
}

void RV8523custom::set_month(uint8_t month)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x08));
  Wire.write(bin2bcd(month));
  Wire.endTransmission();

  return;
}
void RV8523custom::set_year(uint16_t year)
{
  if(year > 2000)
  {
    year -= 2000;
  }

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x09));
  Wire.write(bin2bcd(year));
  Wire.endTransmission();

  return;
}


void RV8523custom::set_second(int sec)
{
  return set_second((uint8_t)sec);
}

void RV8523custom::set_minute(int minute)
{
  return set_minute((uint8_t)minute);
}

void RV8523custom::set_hour(int hour)
{
  return set_hour((uint8_t)hour);
}

void RV8523custom::set_day(int day)
{
  return set_day((uint8_t)day);
}

void RV8523custom::set_Weekday(int weekday)
{
  return set_Weekday((uint8_t)weekday);
}

void RV8523custom::set_month(int month)
{
  return set_month((uint8_t)month);
}

void RV8523custom::set_year(int year)
{
  return set_year((uint16_t)year);
}

//-------------------- Private --------------------


uint8_t RV8523custom::bin2bcd(uint8_t val)
{
  return val + 6 * (val / 10);
}


uint8_t RV8523custom::bcd2bin(uint8_t val)
{
  return val - 6 * (val >> 4);
}
