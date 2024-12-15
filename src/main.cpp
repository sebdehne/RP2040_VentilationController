#include <Arduino.h>
#include <SPI.h>
#include "dac_dfr0971.h"
#include "SmartHomeServerClientWifi.h"

void setup()
{
  delay(1000);
  Log.log("Welcome");

  DacDfr0971.setup();
  DacDfr0971.setDacMillivoltage(0);

  WiFi.setTimeout(30 * 1000);
  Log.log("Setup done");
}

void loop()
{
  SmartHomeServerClientWifi.run();
}
