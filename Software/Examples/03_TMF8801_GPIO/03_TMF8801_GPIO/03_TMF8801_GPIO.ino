/*
  Using the TMF8801 Time-of-Flight sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: February 22nd, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17716

  This example illustrates how to set a GPIO pin as output.
  Allowed values 
  The actual measurement status will depend if algImmediateInterrupt is set or not.
  The default value is algImmediateInterrupt cleared (0).
  Further information can be found in the device's datasheet.
  Measurement number is incremented every time there is a unique answer. Value rolls back to zero when it reaches 255 
  and is reset upon device reset.

  Hardware Connections:
  - Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  - Attach a LED anode to GPIO0 through a 1k ohm resistor and it's cathode to ground
  - Attach a 10k resistor from GPIO1 pin to 3.3V
  - Plug the sensor onto the shield
  - Serial.print it out at 115200 baud to serial monitor.
*/

#include <Wire.h>
#include "SparkFun_TMF8801_Library.h"

TMF8801 tmf8801;

// Holds the current GPIO modes
byte gpio0mode;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  if (tmf8801.begin() == true)
  {
    tmf8801.enableInterrupt();
    Serial.print("TMF8801 serial number ");
    Serial.print(tmf8801.getSerialNumber());
    Serial.println(" connected");
    Serial.print("Chip revision: ");
    Serial.println(tmf8801.getHardwareVersion());
    Serial.print("App version: ");
    Serial.print(tmf8801.getApplicationVersionMajor());
    Serial.print(".");
    Serial.println(tmf8801.getApplicationVersionMinor());
  }
  else
  {
    Serial.println("TMF8801 connection failed.");
    Serial.print("Status register = 0x");
    Serial.println(tmf8801.getStatus(), HEX);
    printErrorMessage();
    Serial.println("System halted.");
    while (true);
  }
  
  // Device will stop measuring if GPIO1 is pulled low.
  tmf8801.setGPIO1Mode(MODE_LOW_INPUT);

  // Initialize gpio0mode variable;
  gpio0mode = MODE_LOW_OUTPUT;
}


void loop()
{
  // Restart the application when ENABLE pin returns to HIGH
  if (!tmf8801.isConnected())
  {
    Serial.println("Not connected or ENABLE pin is low.");
    tmf8801.wakeUpDevice();
    tmf8801.begin();
  }

  if (tmf8801.dataAvailable())
  {
    Serial.print("Distance: ");
    Serial.print(tmf8801.getDistance());
    Serial.println(" mm");
  }

  // Toggle GPIO0 mode in every sampling cycle
  if (gpio0mode == MODE_LOW_OUTPUT)
  {
    tmf8801.setGPIO0Mode(MODE_HIGH_OUTPUT);
  }
  else
  {
    tmf8801.setGPIO0Mode(MODE_LOW_OUTPUT);
  }
  
  // Poll GPIO0 current mode so it can be used in the next iteration
  gpio0mode = tmf8801.getGPIO0Mode();

  // Wait half a second and start over
  delay(500);
}

// This function will print a user-friendly error message.
void printErrorMessage()
{
  switch (tmf8801.getLastError())
  {
  case ERROR_I2C_COMM_ERROR:
    Serial.println("Error: I2C communication error");
    break;

  case ERROR_CPU_RESET_TIMEOUT:
    Serial.println("Error: Timeout on CPU reset");
    break;

  case ERROR_WRONG_CHIP_ID:
    Serial.println("Error: Chip ID mismatch");
    break;

  case ERROR_CPU_LOAD_APPLICATION_ERROR:
    Serial.println("Error: Load application error");
    break;

  default:
    break;
  }
}
