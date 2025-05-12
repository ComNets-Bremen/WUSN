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

#ifndef RV8523custom_h
#define RV8523custom_h


#include <inttypes.h>


class RV8523custom
{
  public:
    RV8523custom();
    void start(void);
    void set_second(uint8_t second);
    void set_minute(uint8_t hour);
    void set_hour(uint8_t hour);
    void set_day(uint8_t day);
    void set_Weekday(uint8_t weekday);
    void get_Weekday(uint8_t *weekday);
    void set_month(uint8_t month);
    void set_year(uint16_t year);
    void set_second(int sec);
    void set_minute(int minute);
    void set_hour(int hour);
    void set_day(int day);
    void set_Weekday(int weekday);
    void set_month(int month);
    void set_year(int year);
    
  private:
    uint8_t bin2bcd(uint8_t val);
    uint8_t bcd2bin(uint8_t val);
};


#endif //RV8523custom_h
