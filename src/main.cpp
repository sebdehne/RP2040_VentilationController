#include <Arduino.h>
#include <SPI.h>
#include "dac_dfr0971.h"
#include "SmartHomeServerClientWifi.h"

void setup() {
  DacDfr0971.setup();
}

void loop() {
  SmartHomeServerClientWifi.run();
}

