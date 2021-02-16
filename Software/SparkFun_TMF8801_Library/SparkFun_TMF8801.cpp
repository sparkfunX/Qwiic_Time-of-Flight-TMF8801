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

#include <stdio.h>
#include "SparkFun_TMF8801.h"

bool TMF8801::begin(uint8_t address, TwoWire& wirePort)
{
	// Initialize the selected I2C interface 
	bool ready = tmf8801_io.begin(address, wirePort);
	
	// If the interface is not ready or TMF8801 is unreacheable return false
	if(ready == false)
	{
		lastError = TMF8801_Errors::I2C_COMM_ERROR;
		return false;
	}
	
	// Reset TMF8801. Since it clears itself, we don't need to clear it
	tmf8801_io.setRegisterBit(TMF8801_Registers::ENABLE, ENABLE_cpu_reset);
	
	ready = cpuReady();
	if (ready == false)
	{
		lastError = TMF8801_Errors::CPU_RESET_TIMEOUT;
		return false;
	}

	// Are we really talking to a TMF8801 ?
	uint8_t value = tmf8801_io.readSingleByte(TMF8801_Registers::ID);
	if (value != CHIP_ID_NUMBER)
	{
		lastError = TMF8801_Errors::WRONG_CHIP_ID;
		return false;
	}
	
	// Load the measurement application and wait until it's ready
	tmf8801_io.writeSingleByte(TMF8801_Registers::APPREQID, APPLICATION);
	ready = applicationReady();
	if (ready == false)
	{
		lastError = TMF8801_Errors::CPU_LOAD_APPLICATION_ERROR;
		return false;
	}
	
	// Get hardware data
	readHardwareData();

	bool result = false;
	if (performFactoryCalibration == true)
	{
		result = doFactoryCalibration();
	}
	
	// Set calibration data
	tmf8801_io.writeSingleByte(TMF8801_Registers::COMMAND, COMMAND_CALIBRATION);
	tmf8801_io.writeMultipleBytes(TMF8801_Registers::FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
	tmf8801_io.writeMultipleBytes(TMF8801_Registers::STATE_DATA_WR_0, ALGO_STATE, sizeof(ALGO_STATE));
	
	// Configure the application - values were taken from AN0597, pp. 22
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA7, 0x03);
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA7, 0x03);
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA6, 0x23);
	
	//uint8_t gpio = static_cast<uint8_t>(gpio1_prog);
	//gpio = gpio << 4;
	//gpio |= static_cast<uint8_t>(gpio0_prog);
	
	//tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA5, gpio); 
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA5, 0X00); 
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA4, 0x00);
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA3, 0x00);
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA2, 0x64); 
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA1, 0xFF);
	tmf8801_io.writeSingleByte(TMF8801_Registers::CMD_DATA0, 0xFF);
	
	// Start the application
	tmf8801_io.writeSingleByte(TMF8801_Registers::COMMAND, COMMAND_MEASURE);
	
	delay(50);
	
	// Set lastError no NONE
	lastError = TMF8801_Errors::NONE;
	return true;
}

bool TMF8801::cpuReady()
{
	uint16_t counter = 0;
	
	// Wait for CPU_READY_TIMEOUT mSec until TMF8801 is ready
	do
	{
		bool ready = tmf8801_io.isBitSet(TMF8801_Registers::ENABLE, ENABLE_cpu_ready);
		if (ready == false)
		{
			counter++;
			delay(10);
		}
		else
		{
			return true;
		}
	} while (counter < CPU_READY_TIMEOUT) ;

	// If TMF8801 CPU is not ready, return false
	return false;
}

bool TMF8801::dataAvailable()
{
	uint8_t result = tmf8801_io.readSingleByte(TMF8801_Registers::REGISTER_CONTENTS);
	return result == COMMAND_RESULT;
}

bool TMF8801::applicationReady()
{
	uint16_t counter = 0;
	
	// Wait for APPLICATION_READY_TIMEOUT mSec until TMF8801 is ready
	do
	{
		bool ready = (tmf8801_io.readSingleByte(TMF8801_Registers::APPID) == APPLICATION);
		if (ready == false)
		{
			counter++;
			delay(10);
		}
		else
		{
			return true;
		}
	} while (counter < APPLICATION_READY_TIMEOUT) ;

	// If application is not ready, return false
	return false;
}

TMF8801_Errors TMF8801::getLastError()
{
	return lastError;
}

bool TMF8801::doFactoryCalibration()
{
	performFactoryCalibration = false;
	
	lastError = TMF8801_Errors::NONE;
	
	tmf8801_io.writeSingleByte(TMF8801_Registers::COMMAND, COMMAND_FACTORY_CALIBRATION);
	uint32_t calibrationStart = millis();
	
	uint8_t value;
	Serial.println("Calibrating... please wait...");
	do
	{
		value = tmf8801_io.readSingleByte(TMF8801_Registers::REGISTER_CONTENTS);
		if (value == CONTENT_CALIBRATION)
		{
			tmf8801_io.readMultipleBytes(TMF8801_Registers::FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
			Serial.println("Please update calibrationData array in your sketch with the following values:");
			Serial.print("{");
			for (int i = 0; i < sizeof(calibrationData); i++)
			{
				Serial.print("0x");
				Serial.print(calibrationData[i], HEX);
				if (i < (sizeof(calibrationData) - 1))
					Serial.print(",");
				else
					Serial.println("};");
			}
			Serial.println("System halted...");
			while (true) ;
		}
		
		delay(100);		
	} while (millis() - calibrationStart < 10000);
	
	lastError = TMF8801_Errors::FACTORY_CALIBRATION_ERROR;
	return false;
}

uint8_t TMF8801::getStatus()
{
	return tmf8801_io.readSingleByte(TMF8801_Registers::STATUS);
}

void TMF8801::doMeasurement()
{
	uint8_t buffer[4];
	tmf8801_io.readMultipleBytes(TMF8801_Registers::RESULT_NUMBER, buffer, sizeof(buffer));
	memcpy(&resultData, buffer, sizeof(buffer));
}

uint16_t TMF8801::getDistance()
{
	doMeasurement();
	return resultData.distancePeak;
}

const ResultData& TMF8801::getResultData()
{
	return resultData;
}

void TMF8801::enableInterrupt()
{
	uint8_t registerValue = tmf8801_io.readSingleByte(TMF8801_Registers::INT_ENAB);
	registerValue |= INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(TMF8801_Registers::INT_ENAB, registerValue);
}

void TMF8801::disableInterrupt()
{
	uint8_t registerValue = tmf8801_io.readSingleByte(TMF8801_Registers::INT_ENAB);
	registerValue &= ~INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(TMF8801_Registers::INT_ENAB, registerValue);
}

void TMF8801::clearInterruptFlag()
{
	uint8_t registerValue = tmf8801_io.readSingleByte(TMF8801_Registers::INT_STATUS);
	registerValue |= INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(TMF8801_Registers::INT_STATUS, registerValue);
}

const HardwareData& TMF8801::getHardwareData()
{
	return hardwareData;
}

void TMF8801::readHardwareData()
{
	uint8_t result;
	uint8_t serial[2];
	
	// Get major application revision
	hardwareData.appMajorVersion = tmf8801_io.readSingleByte(TMF8801_Registers::APPREV_MAJOR);
	hardwareData.appMinorRevision = tmf8801_io.readSingleByte(TMF8801_Registers::APPREV_MINOR);
	hardwareData.hwVersionNumber = tmf8801_io.readSingleByte(TMF8801_Registers::ID);
	
	//Get serial number
	tmf8801_io.writeSingleByte(TMF8801_Registers::COMMAND, COMMAND_SERIAL);
	result = tmf8801_io.readSingleByte(TMF8801_Registers::REGISTER_CONTENTS);
	do
	{
		delay(10);
		result = tmf8801_io.readSingleByte(TMF8801_Registers::REGISTER_CONTENTS);
	} while (result != COMMAND_SERIAL);
	
	tmf8801_io.readMultipleBytes(TMF8801_Registers::STATE_DATA_0, serial, sizeof(serial));
	memcpy(&hardwareData.serialNumber, serial, sizeof(serial));
}

bool TMF8801::measurementEnabled()
{	
	uint8_t result = getResultData().resultInfo;
	result = result >> 6;
	return result == 0;
}