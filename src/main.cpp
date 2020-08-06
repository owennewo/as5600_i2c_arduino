#include <Arduino.h>
#include <Wire.h>
#include "AS5600.h"

AS5600 pot;

void setup() {
  
  Serial.begin(115200);
  while (!Serial);

  Wire.begin();
  Wire.setClock(400000);
  
  AS5600_Config config = pot.getConfig();
  Serial.printf("config: %u %u %u %u %u %u %u\n", config.powerMode, config.hysteresis, config.outputStage, config.pwmFrequency, config.slowFilter, config.fastFilterThreshold, config.watchdog);
  
}

void loop() {

  delay(200);
  
  uint16_t angle = pot.getAngle();
  uint16_t magnitude = pot.getMagnitude();
  uint16_t angleDegrees = map(angle, 0, 4095, 0, 360);  

  AS5600_Status status = pot.getStatus();
  Serial.printf("angle=%u angleDegress=%u magnitude=%u detected: %u, high=%u low=%u \n", angle, angleDegrees, magnitude, status.magnetDetected, status.magnetHigh, status.magnetLow);

      
}