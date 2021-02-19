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

class TMF8801
{
private:
	// Default values
	byte commandDataValues[8] = {0x03, 0x23, 0x44, 0x00, 0x00, 0x64, 0xD8, 0xA4 };
	byte resultNumber;
	byte resultInfo;
	int distancePeak;
	byte address;
	TMF8801_IO tmf8801_io;	
	byte lastError;	
	bool cpuReady();
	bool applicationReady();
	void doMeasurement();
	void updateCommandData8();

public:
	byte gpio1_prog = MODE_LOW_OUTPUT;
	byte gpio0_prog = MODE_LOW_OUTPUT;
	byte calibrationData[14] = { 0xC1, 0x22, 0x0, 0x1C, 0x9, 0x40, 0x8C, 0x98, 0xA, 0x15, 0xCE, 0x9C, 0x1, 0xFC };

	TMF8801() {}	
	bool begin(byte address = DEFAULT_I2C_ADDR, TwoWire& wirePort = Wire);	
	bool dataAvailable();
	bool isConnected();
	byte getStatus();
	byte getLastError();	
	int getDistance();	
	void enableInterrupt();
	void disableInterrupt();
	void clearInterruptFlag();
	bool measurementEnabled();
	void setGPIO0Mode(byte gpioMode);
	byte getGPIO0Mode();
	void setGPIO1Mode(byte gpioMode);
	byte getGPIO1Mode();
	void setSamplingPeriod(byte period);
	byte getSamplingPeriod();
	byte getRegisterValue(byte reg);
	void setRegisterValue(byte reg, byte value);
	void getRegisterMultipleValues(byte reg, byte* buffer, byte length);
	void setRegisterMultipleValues(byte reg, const byte* buffer, byte length);
	bool getCalibrationData(byte* calibrationResults);
	void setCalibrationData(const byte* newCalibrationData);
	byte getHardwareVersion();
	byte getApplicationVersionMajor();
	byte getApplicationVersionMinor();
	int getSerialNumber();
	byte getMeasurementReliability();
	byte getMeasurementStatus();
	byte getMeasurementNumber();
	void resetBoard();
	void wakeUpDevice();
};

#endif