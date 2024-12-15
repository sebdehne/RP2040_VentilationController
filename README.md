# Wifi based Ventilation Controller which use EC motors

Firmware for the [Arduino Nano RP2040 Connect](https://store.arduino.cc/products/arduino-nano-rp2040-connect) which acts as a wifi bridge. Let's you
use simple IP/UDP to interact the [Gravity 2-Channel I2C DAC Module (0-10V)](https://www.dfrobot.com/product-2613.html) which controlls any
EC motor based Ventilation unit.

- Uses the [arduino-pico SDK](https://arduino-pico.readthedocs.io/en/latest/)
- Listens on UDP port 9000
- When a UDP-package is received with 2-byte payload, then this payload is translated 
  into a 16bit integer representing millivolts which the DAC module should output.

