/******************************************************************************
 * MoleNet - A Wireless Underground Sensor Network
 *
 * Copyright (C) 2016, Communication Networks, University of Bremen, Germany
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * This file is part of MoleNet
 *
 ******************************************************************************/

#include "Arduino.h"
#include "Wire.h"
#include "RV8523ALARM.h"

#define I2C_ADDR (0xD0>>1)

RV8523ALARM::RV8523ALARM(void)
{
  Wire.begin();
  return;
}

void RV8523ALARM::getWeekday(uint8_t *weekday)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x07));
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDR, 1);
  *weekday = bcd2bin(Wire.read() & 0x07); //day of week
  return;
}

void RV8523ALARM::resetCtrl()
{
	writeRegBit(0x00,7,0);
	writeRegBit(0x00,5,0);
	writeRegBit(0x00,4,0);
	writeRegBit(0x00,2,0);
	writeRegBit(0x00,1,0);

	writeRegBit(0x01,7,0);
	writeRegBit(0x01,6,0);
	writeRegBit(0x01,5,0);
	writeRegBit(0x01,4,0);
	writeRegBit(0x01,3,0);
	writeRegBit(0x01,2,0);
	writeRegBit(0x01,1,0);
	writeRegBit(0x01,0,0);

	writeRegBit(0x02,3,0);
	writeRegBit(0x02,2,0);
	writeRegBit(0x02,1,0);
	writeRegBit(0x02,0,0);

	writeRegBit(0x0F,3,1);	//deactivate clockout
	writeRegBit(0x0F,4,1);
	writeRegBit(0x0F,5,1);
	return;
}

void RV8523ALARM::setAlarmTime(uint8_t min, uint8_t hour, uint8_t day, 
  uint8_t weekday)
{

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x0A));
  Wire.write(bin2bcd(min));
  Wire.write(bin2bcd(hour));
  Wire.write(bin2bcd(day));
  Wire.write(weekday);
  Wire.endTransmission();
  return;
}

void RV8523ALARM::getAlarmTime(uint8_t *min, uint8_t *hour, uint8_t *day,
  uint8_t *weekday)
{
	Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x0A));
  Wire.endTransmission();

	Wire.requestFrom(I2C_ADDR, 4);
	*min   	  = bcd2bin(Wire.read() & 0x7F);
  *hour  	  = bcd2bin(Wire.read() & 0x3F); //24 hour mode
  *day   	  = bcd2bin(Wire.read() & 0x3F);
  *weekday  = bcd2bin(Wire.read() & 0x07);
}

void RV8523ALARM::getAlarmConfig(uint8_t *min, uint8_t *hour, uint8_t *day, 
	uint8_t *weekday, uint8_t *mAlarm, uint8_t *hAlarm, uint8_t *dAlarm,
	uint8_t *walarm)
{
	uint8_t tmp;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x0A));
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDR, 4);
	tmp 		  = Wire.read();  
	*min   	  = bcd2bin(tmp & 0x7F);
	*mAlarm   = (tmp & 0x80);
	tmp 		  = Wire.read(); 
  *hour  	  = bcd2bin(tmp & 0x3F); //24 hour mode
	*hAlarm 	=	(tmp & 0x80);
	tmp 		  = Wire.read(); 
  *day   	  = bcd2bin(tmp & 0x3F);
	*dAlarm	  =	(tmp & 0x80);
	tmp       = Wire.read();
	*weekday  = bcd2bin(tmp & 0x07);
	*walarm   = bcd2bin(tmp & 0x80);
  return;
}

uint8_t RV8523ALARM::setAlarmType(char type) 
{
  //Type: m: minute, h: hour, d: day, w: weekday
	switch (type)
	{
	  case 'm':
	    activateAlarmType(0x0A);
	    return 1;
	  case 'h':
	    activateAlarmType(0x0B);
	    return 1;
	  case 'd':
	    activateAlarmType(0x0C);
	    return 1;
	  case 'w':
	    activateAlarmType(0x0D);
	    return 1;
	  default:
	    return 0;
	}
}

uint8_t RV8523ALARM::clearAlarmType(char type)
{
  switch (type)
  {
    case 'm':
      deactivateAlarmType(0x0A);
      return 1;
    case 'h':
      deactivateAlarmType(0x0B);
      return 1;
    case 'd':
      deactivateAlarmType(0x0C);
      return 1;
    case 'w':
      deactivateAlarmType(0x0D);
      return 1;
    default:
      return 0;
  }
}

void RV8523ALARM::activateAlarm()
{
	writeRegBit(0x00,1,1);
	return;
}

void RV8523ALARM::deactivateAlarm()
{
	writeRegBit(0x00,1,0);
	return;
}

void RV8523ALARM::resetInterrupt()
{
	writeRegBit(0x01,3,0);
	return;
}

uint8_t RV8523ALARM::readRegister(uint8_t reg)
{
	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.endTransmission();
	Wire.requestFrom(I2C_ADDR,1);
	return Wire.read();
}

//-------------------- Private --------------------


uint8_t RV8523ALARM::bin2bcd(uint8_t val)
{
  return val + 6 * (val / 10);
}


uint8_t RV8523ALARM::bcd2bin(uint8_t val)
{
  return val - 6 * (val >> 4);
}

void RV8523ALARM::writeRegBit(uint8_t reg, uint8_t bitnumber, uint8_t value)
{
	uint8_t regval;

	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.endTransmission();
	Wire.requestFrom(I2C_ADDR,1);
	regval = Wire.read();
	bitWrite(regval,bitnumber,value);
	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.write(regval);
	Wire.endTransmission();
	return;
}

void RV8523ALARM::activateAlarmType(uint8_t reg)
{
	uint8_t regval;

	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.endTransmission();
	Wire.requestFrom(I2C_ADDR,1);
	regval = Wire.read();
	bitClear(regval,7);
	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.write(regval);
	Wire.endTransmission();
	return;
}

void RV8523ALARM::deactivateAlarmType(uint8_t reg)
{
	uint8_t regval;

	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.endTransmission();
	Wire.requestFrom(I2C_ADDR,1);
	regval = Wire.read();
	bitSet(regval,7);
	Wire.beginTransmission(I2C_ADDR);
	Wire.write(byte(reg));
	Wire.write(regval);
	Wire.endTransmission();
	return;
}
