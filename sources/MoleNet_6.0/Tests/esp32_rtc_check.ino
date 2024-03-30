?* ESP32 RTC test */
#include <ESP32Time.h>

ESP32Time rtc(3600);  // offset in seconds GMT+1

void setup() {
  Serial.begin(115200);
  rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30  
}

void loop() {
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format   
  struct tm timeinfo = rtc.getTimeStruct(); 
  delay(1000);
}
