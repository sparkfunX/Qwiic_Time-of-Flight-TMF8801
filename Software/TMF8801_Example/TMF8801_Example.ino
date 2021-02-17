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

#include <stdint.h>
#include "SparkFun_TMF8801.h"

bool toggle = false;
TMF8801 tmf8801;

void printErrorMessage()
{
	switch (tmf8801.getLastError())
	{
	case TMF8801_Errors::I2C_COMM_ERROR :
		Serial.println("Error: I2C communication error");
		break;
	
	case TMF8801_Errors::CPU_RESET_TIMEOUT:
		Serial.println("Error: Timeout on CPU reset");
		break;
	
	case TMF8801_Errors::WRONG_CHIP_ID:
		Serial.println("Error: Chip ID mismatch");
		break;
	
	case TMF8801_Errors::CPU_LOAD_APPLICATION_ERROR:
		Serial.println("Error: Load application error");
		break;
	
	default:
		break;
	}
}

void setup()
{
	Serial.begin(9600);
	while (!Serial) {}
	Wire.begin();
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
  
	// Uncomment line below to begin factory calibration but read datasheet DS000648 item 7.6.1 before proceeding
	//tmf8801.performFactoryCalibration = true;
  
	// After calibrating is done paste the contents of the array provided via serial in the line below
	uint8_t tempCalibrationData[] = { 0xC1, 0x22, 0x0, 0x1C, 0x9, 0x40, 0x8C, 0x98, 0xA, 0x15, 0xCE, 0x9C, 0x1, 0xFC };
  
	memcpy(tmf8801.calibrationData, tempCalibrationData, sizeof(tempCalibrationData));
	
	bool ready = tmf8801.begin();
	tmf8801.enableInterrupt();
	
	HardwareData hwData = tmf8801.getHardwareData();
  
	if (ready == true)
	{
		Serial.println("TMF8801 ok !");
		Serial.print("Chip revision: ");
		Serial.println(hwData.hwVersionNumber);
		Serial.print("App version: ");
		Serial.print(hwData.appMajorVersion);
		Serial.print(".");
		Serial.println(hwData.appMinorRevision);
		Serial.print("Chip serial number: ");
		Serial.println(hwData.serialNumber);
	}
	else
	{
		Serial.println("TMF8801 fail !");
		Serial.print("Status register = 0x");
		Serial.println(tmf8801.getStatus(), HEX);
		printErrorMessage();
	}
	
	tmf8801.setSamplingPeriod(0xff);
	
	// Configure GPIO as output
	tmf8801.setGPIOMode(0, TMF8801_GPIO_MODE::MODE_HIGH_OUTPUT);
	tmf8801.setGPIOMode(1, TMF8801_GPIO_MODE::MODE_HIGH_OUTPUT);
}

void loop()
{
	uint8_t reliability;
	uint8_t measurementStatus;
	if (tmf8801.dataAvailable())
	{   
		tmf8801.clearInterruptFlag();
		Serial.print("Distance: ");
		Serial.print(tmf8801.getDistance());
		Serial.print(" mm - ");
	
		reliability = tmf8801.getResultData().resultInfo;
		measurementStatus = reliability >> 6;
		reliability &= 0x3f;
		Serial.print("Measurement reliability (0 = worst, 63 = best): ");
		Serial.println(reliability);

		delay(500);
		
		toggle = !toggle;
		if (toggle)
		{
			tmf8801.setGPIOMode(1, TMF8801_GPIO_MODE::MODE_HIGH_OUTPUT);
		}
		else
		{
			tmf8801.setGPIOMode(1, TMF8801_GPIO_MODE::MODE_LOW_OUTPUT);
		}
	}
}
