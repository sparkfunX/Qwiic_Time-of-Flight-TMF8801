/*
  This is a library written for the AMS TMF-8801 Time-of-flight sensor
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 15th, 2021
  This file is an example of usage for the TMF-8801 ToF sensor.

  PLEASE NOTICE: You must perform a factory calibration on first use of the board.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "SparkFun_TMF8801_Library.h"

bool toggle = false;
TMF8801 tmf8801;

int count = 0;

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

	// Configure GPIO as output
	tmf8801.setGPIO0Mode(MODE_HIGH_OUTPUT);
	tmf8801.setGPIO1Mode(MODE_HIGH_OUTPUT);
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
		Serial.print("Measurement reliability (0 = worst, 63 = best): ");
		Serial.println(tmf8801.getMeasurementReliability());
		Serial.print("Measurement status: ");
		Serial.println(tmf8801.getMeasurementStatus());
		Serial.print("Measurement number: ");
		Serial.println(tmf8801.getMeasurementNumber());

	}

	delay(1000);
}
