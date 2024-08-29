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
 *       Filename:  rv8523_clock_alarm.h
 *
 *    Description:  A combined Real Time and Alarm Library for RV8523.
 *                  Header File.
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
#ifndef MOLENET_RV8523_RTC_AND_ALARM_HPP_INCLUDED
#define MOLENET_RV8523_RTC_AND_ALARM_HPP_INCLUDED

#define DEBUG
#include "serial_print.h"

#include <Wire.h>
#include "TimeLib.h"   // https://github.com/PaulStoffregen/Time
// Comment it if UNIX Time support is not required

class RV8523_RTC
{
    public:  
        enum TimeMode
        {   TM_12HOUR, TM_24HOUR };
        
        enum Register
        {
            CONTROL1 = 0x00,
            CONTROL2 = 0x01,
            CONTROL3 = 0x02,
            SECONDS  = 0x03,
            MINUTES  = 0x04,
            HOURS    = 0x05,
            DAYS     = 0x06,
            WEEKDAYS = 0x07,
            MONTHS   = 0x08,
            YEARS    = 0x09,
            MINUTE_ALARM     = 0x0A,
            HOUR_ALARM       = 0x0B,
            DAY_ALARM        = 0x0C,
            WEEKDAY_ALARM    = 0x0D,
            FREQUENCY_OFFSET = 0x0E,
            TIMER_AND_CLKOUT = 0x0F,
            TIMER_A_CLOCK    = 0x10,
            TIMER_A          = 0x11,
            TIMER_B_CLOCK    = 0x12,
            TIMER_B          = 0x13
        };
        
        enum AlarmType
        {   AT_MINUTE = MINUTE_ALARM,
            AT_HOUR,
            AT_DAY,
            AT_WEEKDAY,
            AT_INVALID,
            AT_INCREMENTAL_MINUTE
        };
          
        RV8523_RTC();
        
        // RTC Functions
        void initializeRTC();
        void startRTC(void);
        void stopRTC(void);
        void setTimeMode(TimeMode mode);   

        void setClockTime(const uint8_t &sec, const uint8_t &min, const uint8_t &hour, const uint8_t &day, const uint8_t &month, const uint8_t &year);
        void getClockTime(uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *month, uint8_t *year);
        uint8_t getCurrentMinutes();

#ifdef _Time_h        
        void setClockTime(const uint32_t &time);          
        uint32_t getClockTime(void);        
#endif
        
        // Alarm Functions
        
        /** \breif Start Alarm with easy configuration
         *  \param INT_PINT interrupt pin of micocontroller attached to int1 of rtc 
         *  \param type Alarm type
         *  \param min Alarm minute
         *  \param hour Alarm hour
         *  \param day Alarm day 
         *  \param weekday Alarm weekday
         * 
         *  This funcation can be used to easy configure and active alarm along
         *  with attaching the interrupts.
         */
        void beginAlarm(const uint8_t &INT_PIN, AlarmType type, const uint8_t &min, uint8_t const &hour, const uint8_t &day, const uint8_t &weekday);
        void beginAlarm(const uint8_t &INT_PIN, const uint8_t &ALARMS, const uint8_t &min, uint8_t const &hour, const uint8_t &day, const uint8_t &weekday);
        
        static void alarmInterrupt();
        
        /** \breif Alarm Interrput bool variable
         * 
         *  If beginAlarm() function is used, then whenever alarm occurs, this variable will be set to true.
         *  On fulfilling the task on alarm, this variable must be set again to false manually.
         *  See alaram_test_easy example for details.
         */
        static bool alarmInt;
        
        /** \breif Clear All Alarms
         *  This fucntion will clear all alarms.
         */
        void clearAllAlarm();
        
        /** \breif Set alarm type
         *  \param type type of Alarm
         *  This function will set the alarm of given type. 
         *  ***IMPORTANT***
         *  ** This function must be call after calling function setAlarmTime() otherwise
         *     all alarms types will be activated which could be not desired. Setting AT_MINUTE
         *     will set up alarm  evey (Alarm Minute) in every hour [24 times in day]. Seting AT_MINUTE and AT_HOUR
         *     will set up alarm only on (Alaram Minute, Alarm Hour) [1 time in day]. and so on are so forth.
         *  ** If only one type of alarm is required, then function clearAllAlarm() must be called before this function.
         */ 
        void setAlarmType(AlarmType type);
        
        /** \breif Set Alarm Time
         *  This function set the time of alarm. After calling this function, setAlarmType() must be called to set
         *  the type of Alarm.
         */
        void setAlarmTime(uint8_t min, uint8_t hour, uint8_t day, uint8_t weekday);
        void setAlarmType(AlarmType type, bool on);
        
        void getAlarmTime(uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *weekday);
        void getAlarmConfig(uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *weekday,
                            uint8_t *mAlarm, uint8_t *hAlarm, uint8_t *dAlarm, uint8_t *walarm);         
        
        void resetAlarm();

        void activateAlarm();
        void deactivateAlarm();
        
        void resetInterrupt();
        void resetCtrl();
                
        // Common Functions
        uint8_t readRegister(Register reg);         
        void batterySwitchOver(int on);
        void debugCurrentTime();
        void debugAlarm();
     
        //void getWeekday(uint8_t *weekday);
         
    private:
        uint8_t bin2bcd(uint8_t val);
        uint8_t bcd2bin(uint8_t val);            
        void writeRegBit(uint8_t reg, uint8_t bitnumber, uint8_t value);
        
        uint8_t m_min;
        AlarmType m_Type;
        uint8_t m_INT_PIN;
};

#endif  // MOLENET_RV8523_RTC_AND_ALARM_HPP_INCLUDED