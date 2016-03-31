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
