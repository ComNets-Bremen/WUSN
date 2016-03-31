#ifndef RV8523ALARM_h
#define RV8523ALARM_h

#include <inttypes.h>

class RV8523ALARM
{
  public:
    RV8523ALARM();

		void resetCtrl();
		void getWeekday(uint8_t *weekday);
		void setAlarmTime(uint8_t min, uint8_t hour, uint8_t day, uint8_t weekday);
		void getAlarmConfig(uint8_t *min, uint8_t *hour, uint8_t *day, 
					uint8_t *weekday, uint8_t *mAlarm, uint8_t *hAlarm, uint8_t *dAlarm,
					uint8_t *walarm);
		void getAlarmTime(uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *weekday);
		uint8_t setAlarmType(char type);
		/*type: 'm' || 'M': minute, 'h' || 'H': hour, 'd' || 'D': day*/
		uint8_t clearAlarmType(char type);
		void activateAlarm();
		void deactivateAlarm();
		void resetInterrupt();
		uint8_t readRegister(uint8_t reg);

  private:
    uint8_t bin2bcd(uint8_t val);
		uint8_t bcd2bin(uint8_t val);
		void writeRegBit(uint8_t reg, uint8_t bitnumber, uint8_t value);
		void activateAlarmType(uint8_t reg);
		void deactivateAlarmType(uint8_t reg);
};

#endif //RV8523ALARM_h
