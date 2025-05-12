/*
 * MoleNet Maintenance App
 * 
 * ComNets, University of Bremen
 * Jens Dede <jd@comnets.uni-bremen.d>
 * 
*/

// RTC
#include <RV8523.h>          //from: https://github.com/watterott/Arduino-Libs

// External EEPROM (sensor data)
#include <SPIEEP.h>          //from: https://bitbucket.org/spirilis/spieep/

// Internal EEPROM (config)
#include <EEPROM.h>

#define WAIT_TIME_SHELL 5    // seconds
#define INTPIN          3    // pin for interrupt
#define EEPROM_CSPIN    9    // EEPROM CS

// Internal EEPROM  config identifiers
#define ADDR_ID         1    // address for id in flash

typedef struct _dateTimeObject {
  int16_t year;
  int8_t month;
  int8_t day;
  int8_t hour;
  int8_t minute;
  int8_t second;
} dateTime;

RV8523         rtc;
// External EEPROM
SPIEEP         eep(16,128,65535);

byte nodeId = 1;

void setup() {
  Serial.begin(9600);
  //pinMode(INTPIN,INPUT_PULLUP);
  rtc.set24HourMode();
  rtc.start();
  
  rtc.batterySwitchOver(1);

  //initialize external EEPROM
  eep.begin_spi(EEPROM_CSPIN);
  eep.wakeup();  
  nodeId = EEPROM.read(ADDR_ID);
  Serial.print("Node Id: ");
  Serial.println(nodeId);
  eep.sleepmode();
  
  simpleShell(WAIT_TIME_SHELL);

}

void loop() {
  // Main app. Empty for maintanance app
}


/****************************************************************/
/* * * * * * * * * *      Heper functions     * * * * * * * * * */
/****************************************************************/

/*
 * A simple shell with the following functions:
 * - Erase flash
 * - Set a net ID
 * - Set the time of the RTC
 * - Get the time of the RTC
 */
void simpleShell(int timeout){
  int number = 0;
  dateTime dt;
  Serial.print("SimpleShell (timeout: ");
  Serial.print(timeout);
  Serial.println(" s)");
  Serial.println("Commands:");
  Serial.println("e: Erase flash");
  Serial.println("s: Set id");
  Serial.println("t: Set time");
  Serial.println("p: Get time");
  unsigned long waitUntil = millis()+timeout*1000;
  while(waitUntil > millis()){
    if(Serial.available()>0){
      int input = Serial.read();
      switch (input){
        case 'e':
        case 'E':
          Serial.println("Erase flash?");
          if (userYes()){
            Serial.println("Erasing...");
            eep.wakeup();
            if (eep.chip_erase()){
              Serial.println("Erase done. Testing...");
            } else {
              Serial.println("Error");
              eep.sleepmode();
              return;
            }
            if (eep.test_chip()){
              Serial.println("Chip ok");
            } else {
              Serial.println("Error accessing flash!");
              eep.sleepmode();
              return;
            }
            eep.sleepmode();
            Serial.println("done");
          } else
            Serial.println("No");
          break;
        case 's':
        case 'S':
          Serial.println("Enter new node id (0-255):");
          number = getNumber();
          if (number >0){
            Serial.print("New id: ");
            Serial.println(number);
            EEPROM.write(ADDR_ID, (byte)number);
            Serial.print("Node ID is ");
            Serial.println((int)EEPROM.read(ADDR_ID));
          }
          break;
        case 't':
        case 'T':
          Serial.println("Year");
          dt.year = getNumber();
          if (dt.year < 0) return;
          Serial.println("Month");
          dt.month = getNumber();
          if (dt.month < 0) return;
          Serial.println("Day");
          dt.day = getNumber();
          if (dt.day < 0) return;
          Serial.println("Hour");
          dt.hour = getNumber();
          if (dt.hour < 0) return;
          Serial.println("Minute");
          dt.minute = getNumber();
          if (dt.minute < 0) return;
          Serial.println("Second");
          dt.second = getNumber();
          if (dt.second < 0) return;
          Serial.flush();
          Serial.print("Setting RTC to ");
          printDateTime(dt);
          setRtc(dt);
          break;
        case 'p':
        case 'P':
          Serial.println("Time from RTC: ");
          getRtc(&dt);
          printDateTime(dt);
          break;
        case '\n':
          break;
        default:
          Serial.print("Unknown command: ");
          Serial.println((char) input);
      }
    }
  }
}

/* Simple function to prevent accidential execution of functions
 *  Timeout 10 s, return False in case of timeout
 */
bool userYes(){
  clearSerialBuffer();
  // Check for new data
  int timeout = millis()+10000;
  while(millis()<timeout){
    if (Serial.available() > 0){
      int input = Serial.read();
      if (input == 'y' || input == 'Y'){
        return true;
      } else if (input == 'n' || input == 'N'){
        return false;
      } else {
        Serial.println("Possible answers: [yY] and [nN]");
      }
    }
  }
  return false;
}

/*
 * Read a number from the serial console
 */
int getNumber(){
  clearSerialBuffer();
  int timeout = millis()+10000;
  String data = "";
  Serial.println("Enter a number.");
  while(millis()<timeout){
    if(Serial.available() > 0){
      int input = Serial.read();
      switch(input){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          data += (char)input;
          break;
        case '\n':
          return data.toInt();
        default:
          return -1;
      }
    }
  }
  return data.toInt();
}

/*
 * Remove all data from the serial buffer
 */
void clearSerialBuffer(){
  // Remove all bytes from the buffer
  Serial.flush();
  delay(100);
  while(Serial.available()>0)
    Serial.read();
}

void setRtc(dateTime dt){
  rtc.set(dt.second, dt.minute, dt.hour, dt.day, dt.month, dt.year);
}

void getRtc(dateTime *dt){
  int s, m, h, d, M, y;
  rtc.get(&s, &m, &h, &d, &M, &y);
  dt->second = s;
  dt->minute = m;
  dt->hour = h;
  dt->day = d;
  dt->month = M;
  dt->year = y;
}

/*
 * Printout the datetime object
 */
void printDateTime(dateTime dt){
  String printString = "";
  printString += dt.year;
  printString += "-";
  printString += dt.month;
  printString += "-";
  printString += dt.day;
  printString += " ";
  printString += dt.hour;
  printString += ":";
  printString += dt.minute;
  printString += ":";
  printString += dt.second;
  Serial.println(printString);
}

