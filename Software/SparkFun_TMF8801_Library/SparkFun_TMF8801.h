/*
  This is a library written for the AMS TMF-8801 Time-of-flight sensor
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 15th, 2021
  This file is the core of the TMF-8801 ToF sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __TMF8801_LIBRARY__
#define __TMF8801_LIBRARY__

#include <Wire.h>
#include "SparkFun_TMF8801_Constants.h"
#include "SparkFun_TMF8801_IO.h"

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

struct __attribute__((packed)) ResultData
{
	uint8_t resultNumber;
	uint8_t resultInfo;
	uint16_t distancePeak;
};

struct __attribute__((packed)) HardwareData
{
	uint8_t appMajorVersion;
	uint8_t appMinorRevision;	
	uint8_t hwVersionNumber;
	uint16_t serialNumber;
};

class TMF8801
{
private:
	ResultData resultData;
	HardwareData hardwareData;
	uint8_t address;
	TMF8801_IO tmf8801_io;	
	TMF8801_Errors lastError;	
	bool doFactoryCalibration();
	bool cpuReady();
	bool applicationReady();
	void doMeasurement();
	void readHardwareData();
	
public:
	TMF8801_GPIO_PROG gpio1_prog = TMF8801_GPIO_PROG::LOW_OUTPUT;
	TMF8801_GPIO_PROG gpio0_prog = TMF8801_GPIO_PROG::LOW_OUTPUT;
	bool performFactoryCalibration = false;	
	uint8_t calibrationData[14] = { 0 };	
	TMF8801() {}	
	bool begin(uint8_t address = DEFAULT_I2C_ADDR, TwoWire& wirePort = Wire);	
	bool dataAvailable();
	uint8_t getStatus();
	TMF8801_Errors getLastError();	
	uint16_t getDistance();	
	const ResultData& getResultData();
	void enableInterrupt();
	void disableInterrupt();
	void clearInterruptFlag();
	const HardwareData& getHardwareData();
	bool measurementEnabled();
};

#endif