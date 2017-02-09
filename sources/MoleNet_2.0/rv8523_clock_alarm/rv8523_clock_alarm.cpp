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
 ******************************************************************************
 * ============================================================================
 *
 *       Filename:  rv8523_clock_alarm.cpp
 *
 *    Description:  A combined Real Time and Alarm Library for RV8523.
 *                  Implementation File.
 *                      
 *                  The purpose of this library is to integarte time and alarm 
 *                  functionalities together in one library. UNIX Time support 
 *                  functions are also added in new Library which requires a 
 *                  "Time library" from 
 *                     https://github.com/PaulStoffregen/Time
 *                     
 *                   * RV8523 Library is obtained from 
 *                     https://github.com/watterott/Arduino-Libs
 * 
 *                   * RV8523ALARM Library is part of MoleNet project at 
 *                     COMNETS Department at University of Bremen, Germany.     
 *
 *        Version:  1.0
 *        Created:  12/08/2016 22:58:41 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Muhammad Haseeb (MSc. Student-IAE University of Bremen), 
 *                  mhaseeb@uni-bremen.de, 
 *                  hasee.b@hotmail.com
 *   Organization:  COMNETS, Communication Networks Department
 *                  University of Bremen, Bremen, Germany
 *
 * ============================================================================
 */

#include "rv8523_clock_alarm.h"

#include <inttypes.h>
#if defined(__AVR__)
# include <avr/io.h>
#endif
#if ARDUINO >= 100
# include "Arduino.h"
#else
# include "WProgram.h"
#endif

#define I2C_ADDR (0xD0>>1)

// Static Variable
bool RV8523_RTC::alarmInt = false;


RV8523_RTC::RV8523_RTC(): m_min(0), m_Type(AlarmType::AT_INVALID)
{
    Wire.begin();
}

void RV8523_RTC::initializeRTC() 
{
    setTimeMode(RV8523_RTC::TM_24HOUR);
    startRTC();
    batterySwitchOver(1);     
}

void RV8523_RTC::startRTC(void)
{
    uint8_t val;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x00)); //control 1
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    val = Wire.read();

    if(val & (1<<5))
    {
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(byte(0x00)); //control 1
        Wire.write(val & ~(1<<5)); //clear STOP (bit 5)
        Wire.endTransmission();
    }
}

void RV8523_RTC::stopRTC(void)
{
    uint8_t val;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x00)); //control 1
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    val = Wire.read();

    if(!(val & (1<<5)))
    {
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(byte(0x00)); //control 1
        Wire.write(val | (1<<5)); //set STOP (bit 5)
        Wire.endTransmission();
    }
}

void RV8523_RTC::setTimeMode(TimeMode mode)
{
    uint8_t val;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x00)); //control 1
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    val = Wire.read();

    if(val & (1<<3))
    {
        if(mode == TM_12HOUR)
            val |= (1<<3);
        else if(mode == TM_24HOUR)
            val &= ~(1<<3);
        
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(byte(0x00)); //control 1   
        Wire.write(val);
        Wire.endTransmission();
    }
}

void RV8523_RTC::setClockTime(const uint8_t &sec, const uint8_t &min, const uint8_t &hour, const uint8_t &day, const uint8_t &month, const uint8_t &year)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x03));
    Wire.write(bin2bcd(sec));
    Wire.write(bin2bcd(min));
    Wire.write(bin2bcd(hour));
    Wire.write(bin2bcd(day));
    Wire.write(bin2bcd(0));
    Wire.write(bin2bcd(month));
    Wire.write(bin2bcd(year));
    Wire.endTransmission();

    return;    
}

void RV8523_RTC::getClockTime(uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *month, uint8_t *year)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x03));
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 7);
    *sec   = bcd2bin(Wire.read() & 0x7F);
    *min   = bcd2bin(Wire.read() & 0x7F);
    *hour  = bcd2bin(Wire.read() & 0x3F); //24 hour mode
    *day   = bcd2bin(Wire.read() & 0x3F);
            bcd2bin(Wire.read() & 0x07); //day of week
    *month = bcd2bin(Wire.read() & 0x1F);
    *year  = bcd2bin(Wire.read());
}

uint8_t RV8523_RTC::getCurrentMinutes()
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x03));
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 7);
    Wire.read();
    return bcd2bin(Wire.read() & 0x7F);    
}


#ifdef _Time_h  
void RV8523_RTC::setClockTime(const uint32_t &time)
{
    TimeElements tm;
    breakTime(time,tm);
    setClockTime(tm.Second, tm.Minute, tm.Hour, tm.Day, tm.Month, tm.Year);
}

uint32_t RV8523_RTC::getClockTime()
{
    TimeElements tm;
    getClockTime(&tm.Second, &tm.Minute, &tm.Hour, &tm.Day, &tm.Month, &tm.Year);
    return makeTime(tm);
}
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////// Alarm Functions /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void RV8523_RTC::beginAlarm(const uint8_t &INT_PIN, AlarmType type, const uint8_t &min, uint8_t const &hour, const uint8_t &day, const uint8_t &weekday)
{
    resetCtrl();
    clearAllAlarm();
    if( type == AlarmType::AT_INCREMENTAL_MINUTE ) {
        m_Type = AlarmType::AT_INCREMENTAL_MINUTE;
        m_min = min;
        setAlarmTime( ( (getCurrentMinutes()+m_min) % 60), 0, 0, 0);
        setAlarmType(AlarmType::AT_MINUTE);
    }    
    else
    {
        m_Type = AlarmType::AT_INVALID;
        setAlarmTime(min, hour, day, weekday);
        setAlarmType(type);
    }
    activateAlarm();
    resetInterrupt();
    attachInterrupt(INT_PIN, RV8523_RTC::alarmInterrupt, FALLING);  
}

void RV8523_RTC::beginAlarm(const uint8_t &INT_PIN, const uint8_t &ALARMS, const uint8_t &min, uint8_t const &hour, const uint8_t &day, const uint8_t &weekday)
{
    this->m_INT_PIN = INT_PIN;
    resetCtrl();
    setAlarmTime(min, hour, day, weekday);

    clearAllAlarm();
    
    if( ALARMS & B00000001 )
        setAlarmType(AlarmType::AT_MINUTE);
    
    if( ALARMS & B00000010 )
        setAlarmType(AlarmType::AT_HOUR);
    
    if( ALARMS & B00000100 )
        setAlarmType(AlarmType::AT_DAY);
    
    if( ALARMS & B00001000 )
        setAlarmType(AlarmType::AT_WEEKDAY);
    
    if( ALARMS & B00010000 ) {
        m_Type = AlarmType::AT_INCREMENTAL_MINUTE;
        m_min = min;
        setAlarmTime( ( (getCurrentMinutes()+m_min) % 60), 0, 0, 0);
        clearAllAlarm();
        setAlarmType(AlarmType::AT_MINUTE);
    }
    else {
        m_Type = AlarmType::AT_INVALID;
    }
    
    activateAlarm();
    resetInterrupt();
    attachInterrupt(INT_PIN, RV8523_RTC::alarmInterrupt, FALLING);  
}


void RV8523_RTC::alarmInterrupt()   // [Static]
{
    alarmInt = true;
 //   PRINT_SLEEP_MODE_DEACTIVATED
}

void RV8523_RTC::resetAlarm()
{
    alarmInt = false;
    if( m_Type == AT_INCREMENTAL_MINUTE ) {
        beginAlarm(m_INT_PIN, B00010000, m_min, 0, 0, 0);
    }
    batterySwitchOver(true);
}
       

void RV8523_RTC::clearAllAlarm()
{
    // Clear All alarms
    uint8_t reg = RV8523_RTC::AT_MINUTE;
    for(uint8_t i = 0; i < 4; ++i) 
        setAlarmType(static_cast<AlarmType>(reg++), false);  
}    
    

void RV8523_RTC::setAlarmType(AlarmType type)
{

    // Set Alaram of given type
    setAlarmType(type, true);  
}

void RV8523_RTC::setAlarmTime(uint8_t min, uint8_t hour, uint8_t day, uint8_t weekday)
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

void RV8523_RTC::getAlarmTime(uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *weekday)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x0A));
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 4);
    *min      = bcd2bin(Wire.read() & 0x7F);
    *hour     = bcd2bin(Wire.read() & 0x3F); //24 hour mode
    *day      = bcd2bin(Wire.read() & 0x3F);
    *weekday  = bcd2bin(Wire.read() & 0x07);
}

void RV8523_RTC::getAlarmConfig(uint8_t *min, uint8_t *hour, uint8_t *day, 
    uint8_t *weekday, uint8_t *mAlarm, uint8_t *hAlarm, uint8_t *dAlarm,
    uint8_t *walarm)
{
    uint8_t tmp;
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x0A));
    Wire.endTransmission();

    Wire.requestFrom(I2C_ADDR, 4);
    tmp       = Wire.read();  
    *min      = bcd2bin(tmp & 0x7F);
    *mAlarm   = !( (tmp & 0x80) >> 7);
    tmp       = Wire.read(); 
    *hour     = bcd2bin(tmp & 0x3F); //24 hour mode
    *hAlarm   =  !( (tmp & 0x80) >> 7);
    tmp       = Wire.read(); 
    *day      = bcd2bin(tmp & 0x3F);
    *dAlarm   = !( (tmp & 0x80) >> 7);
    tmp       = Wire.read();
    *weekday  = bcd2bin(tmp & 0x07);
    *walarm   = !( (tmp & 0x80) >> 7);
}

void RV8523_RTC::activateAlarm()
{
    writeRegBit(0x00,1,1);
    return;
}

void RV8523_RTC::deactivateAlarm()
{
    writeRegBit(0x00,1,0);
    return;
}

void RV8523_RTC::resetInterrupt()
{
    writeRegBit(0x01,3,0);
    return;
}

void RV8523_RTC::resetCtrl()
{   
    for(uint8_t i = 1; i <= 7; ++i)
        if( i==3 || i == 6)
            continue;
        else
            writeRegBit(0x00,i,0);    
    
    for(uint8_t i = 0; i <= 7; ++i)
        writeRegBit(0x01,i,0);

    for(uint8_t i = 0; i <= 3; ++i)
        writeRegBit(0x02,i,0);

    for(uint8_t i = 3; i <= 5; ++i)      //deactivate clockout
        writeRegBit(0x0F,i,1);
}

////////////////////////////////////////////////////////////////////////////
//////////////////// Common Functions ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
uint8_t RV8523_RTC::readRegister(Register reg)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(reg));
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR,1);
    return Wire.read();
}

void RV8523_RTC::batterySwitchOver(int on) //activate/deactivate battery switch over mode
{   
    uint8_t val;

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(0x02)); //control 3
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 1);
    val = Wire.read();
//     Serial.print("Battery ");
//     Serial.println(val, BIN);

//     if(val & 0xE0)
    {
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(byte(0x02)); //control 3
        if(on)
        {
        // Battery switchover function is enabled in direct switching mode
        // Battery low detection function is enabled             
        Wire.write(val & 0x20);//~0xE0); //battery switchover in standard mode
        }
        else
        {
        Wire.write(val | 0xE0);  //battery switchover disabled
        }
        Wire.endTransmission();
    }
}

void RV8523_RTC::debugCurrentTime()
{
    if(Serial)
    {
#ifdef _Time_h          
        TimeElements tm;
        getClockTime(&tm.Second, &tm.Minute, &tm.Hour, &tm.Day, &tm.Month, &tm.Year);   
  
        Serialprint("[RTC %"SCNd32" %d/%d/%d %d:%d:%d] ", getClockTime()
                                              , tm.Day, tm.Month, tm.Year
                                              , tm.Hour, tm.Minute, tm.Second);
#else
        uint8_t config[6] = {0};
        getClockTime(&config[0], &config[1], &config[2], 
                     &config[3], &config[4], &config[5]);
        
        Serialprint("[RTC %"SCNd32" %d/%d/%d %d:%d:%d] ", getClockTime()
                                                 , config[3], config[4], config[5]
                                                 , config[2], config[1], config[0]);       
#endif
    }
}

void RV8523_RTC::debugAlarm()
{
    if(Serial)
    {
        uint8_t config[8] = {0};
        getAlarmConfig(&config[0], &config[1], &config[2], &config[3],
                       &config[4], &config[5], &config[6], &config[7]);
        
        Serialprint("[Alarm %d|%d|%d|%d|%d %d|%d|%d|%d|%d] ", config[3], config[2], config[1], config[0], m_min,
                                                              config[7], config[6], config[5], config[4],
                                                              (m_Type == AT_INCREMENTAL_MINUTE? 1 : 0)
                   );
    }
}

// void RV8523_RTC::getWeekday(uint8_t *weekday)
// {
//   Wire.beginTransmission(I2C_ADDR);
//   Wire.write(byte(0x07));
//   Wire.endTransmission();
// 
//   Wire.requestFrom(I2C_ADDR, 1);
//   *weekday = bcd2bin(Wire.read() & 0x07); //day of week
//   return;
// }

//////////////////////////////////////////////////////////////////////////////
//////////////////// Private Functions ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

uint8_t RV8523_RTC::bin2bcd(uint8_t val)
{
  return val + 6 * (val / 10);
}

uint8_t RV8523_RTC::bcd2bin(uint8_t val)
{
  return val - 6 * (val >> 4);
}

void RV8523_RTC::writeRegBit(uint8_t reg, uint8_t bitnumber, uint8_t value)
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

void RV8523_RTC::setAlarmType(AlarmType type, bool on)   // private
{
    uint8_t regval;
    
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(type));
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR,1);
    regval = Wire.read();
    if(on)
        bitClear(regval,7);
    else
        bitSet(regval,7);
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(byte(type));
    Wire.write(regval);
    Wire.endTransmission();
    return;
}