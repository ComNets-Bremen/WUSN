#include <SoftwareSerial.h> 



#define SENSOR_PWPIN    7         //Sensor Power
#define SENSOR_PIN      8         //Sensor data pin


SoftwareSerial sdi(SENSOR_PIN, 0);

void setup() {
  Serial.begin(115200);
  sdi.begin(1200);
  pinMode(SENSOR_PWPIN, OUTPUT);
  digitalWrite(SENSOR_PWPIN, HIGH);
  delay(500);
  digitalWrite(SENSOR_PWPIN, LOW);
  delay(500);
  digitalWrite(SENSOR_PWPIN, HIGH);

}

void loop() {
  while (sdi.available() > 0){
    Serial.write(sdi.read());
  }

}

/*
 * Output: 12:01:13.175 ->  2493.4 27.8 22 gDb
<TAB><calibratedCountsVWC> <temperature> <electricalConductivity><CR><sensorType><Checksum><CRC>

 */
