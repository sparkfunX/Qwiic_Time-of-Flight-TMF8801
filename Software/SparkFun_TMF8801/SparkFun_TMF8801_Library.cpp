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
#include "SparkFun_TMF8801_Library.h"

bool TMF8801::begin(byte address, TwoWire& wirePort)
{
	// Initialize the selected I2C interface 
	bool ready = tmf8801_io.begin(address, wirePort);

	// If the interface is not ready or TMF8801 is unreacheable return false
	if (ready == false)
	{
		lastError = ERROR_I2C_COMM_ERROR;
		return false;
	}

	// Reset TMF8801. Since it clears itself, we don't need to clear it
	tmf8801_io.setRegisterBit(REGISTER_ENABLE_REG, CPU_RESET);

	ready = cpuReady();
	if (ready == false)
	{
		lastError = ERROR_CPU_RESET_TIMEOUT;
		return false;
	}

	// Are we really talking to a TMF8801 ?
	byte value = tmf8801_io.readSingleByte(REGISTER_ID);
	if (value != CHIP_ID_NUMBER)
	{
		lastError = ERROR_WRONG_CHIP_ID;
		return false;
	}

	// Load the measurement application and wait until it's ready
	tmf8801_io.writeSingleByte(REGISTER_APPREQID, APPLICATION);
	ready = applicationReady();
	if (ready == false)
	{
		lastError = ERROR_CPU_LOAD_APPLICATION_ERROR;
		return false;
	}

	// Set calibration data
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_CALIBRATION);
	tmf8801_io.writeMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
	tmf8801_io.writeMultipleBytes(REGISTER_STATE_DATA_WR_0, ALGO_STATE, sizeof(ALGO_STATE));

	// Configure the application - values were taken from AN0597, pp. 22
	updateCommandData8();	

	// Start the application
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_MEASURE);

	delay(10);

	// Set lastError no NONE
	lastError = ERROR_NONE;
	return true;
}

bool TMF8801::cpuReady()
{
	short counter = 0;

	// Wait for CPU_READY_TIMEOUT mSec until TMF8801 is ready
	do
	{
		bool ready = tmf8801_io.isBitSet(REGISTER_ENABLE_REG, CPU_READY);
		if (ready == false)
		{
			counter++;
			delay(10);
		}
		else
		{
			return true;
		}
	} while (counter < CPU_READY_TIMEOUT);

	// If TMF8801 CPU is not ready, return false
	return false;
}

bool TMF8801::dataAvailable()
{
	byte result = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
	return result == COMMAND_RESULT;
}

bool TMF8801::isConnected()
{
	bool twiConnected = tmf8801_io.isConnected();
	if (!twiConnected)
		return false;

	return (tmf8801_io.readSingleByte(REGISTER_ID) == CHIP_ID_NUMBER);
}

bool TMF8801::applicationReady()
{
	short counter = 0;

	// Wait for APPLICATION_READY_TIMEOUT mSec until TMF8801 is ready
	do
	{
		bool ready = (tmf8801_io.readSingleByte(REGISTER_APPID) == APPLICATION);
		if (ready == false)
		{
			counter++;
			delay(10);
		}
		else
		{
			return true;
		}
	} while (counter < APPLICATION_READY_TIMEOUT);

	// If application is not ready, return false
	return false;
}

byte TMF8801::getLastError()
{
	return lastError;
}

bool TMF8801::getCalibrationData(byte* calibrationResults)
{
	lastError = ERROR_NONE;
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_FACTORY_CALIBRATION);
	uint32_t calibrationStart = millis();

	byte value;
	do
	{
		value = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
		if (value == CONTENT_CALIBRATION)
		{
			tmf8801_io.readMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationResults, CALIBRATION_DATA_LENGTH);
			return true;
		}

		delay(100);
	} while (millis() - calibrationStart < 10000);

	lastError = ERROR_FACTORY_CALIBRATION_ERROR;
	return false;
}

void TMF8801::setCalibrationData(const byte* newCalibrationData)
{
	memcpy(calibrationData, newCalibrationData, CALIBRATION_DATA_LENGTH);

	tmf8801_io.setRegisterBit(REGISTER_ENABLE_REG, CPU_RESET);

	bool ready = false;
	do
	{
		ready = cpuReady();
	} while (!ready);

	tmf8801_io.writeSingleByte(REGISTER_APPREQID, APPLICATION);

	ready = false;
	do
	{
		ready = applicationReady();
	} while (!ready);

	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_CALIBRATION);
	tmf8801_io.writeMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
	tmf8801_io.writeMultipleBytes(REGISTER_STATE_DATA_WR_0, ALGO_STATE, sizeof(ALGO_STATE));

	updateCommandData8();

	// Start the application
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_MEASURE);

	delay(50);
}

byte TMF8801::getApplicationVersionMajor()
{
	return tmf8801_io.readSingleByte(REGISTER_APPREV_MAJOR);
}

byte TMF8801::getApplicationVersionMinor()
{
	return tmf8801_io.readSingleByte(REGISTER_APPREV_MINOR);
}

byte TMF8801::getHardwareVersion()
{
	return tmf8801_io.readSingleByte(REGISTER_REVID);
}

int TMF8801::getSerialNumber()
{
	int serial = 0;
	byte value[2];

	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_SERIAL);
	byte result = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
	do
	{
		delay(10);
		result = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
	} while (result != COMMAND_SERIAL);

	tmf8801_io.readMultipleBytes(REGISTER_STATE_DATA_0, value, 2);
	serial = value[1];
	serial = serial << 8;
	serial |= value[0];
	return serial;
}

byte TMF8801::getMeasurementReliability()
{
	return (resultInfo & 0x3f);
}

byte TMF8801::getMeasurementStatus()
{
	return (resultInfo >> 6);
}

byte TMF8801::getMeasurementNumber()
{
	return resultNumber;
}

void TMF8801::resetBoard()
{
	tmf8801_io.setRegisterBit(REGISTER_ENABLE_REG, CPU_RESET);

	bool ready = false;
	do
	{
		ready = cpuReady();
	} while (!ready);

	tmf8801_io.writeSingleByte(REGISTER_APPREQID, APPLICATION);

	ready = false;
	do
	{
		ready = applicationReady();
	} while (!ready);

	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_CALIBRATION);
	tmf8801_io.writeMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
	tmf8801_io.writeMultipleBytes(REGISTER_STATE_DATA_WR_0, ALGO_STATE, sizeof(ALGO_STATE));

	updateCommandData8();

	// Start the application
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_MEASURE);

	delay(50);
}

void TMF8801::wakeUpDevice()
{
	byte result;
	do
	{
		tmf8801_io.writeSingleByte(REGISTER_ENABLE_REG, 0x01);
		result = tmf8801_io.readSingleByte(REGISTER_ENABLE_REG);
		delay(100);
	} while (result != 0x41);
}

byte TMF8801::getStatus()
{
	return tmf8801_io.readSingleByte(REGISTER_STATUS);
}

void TMF8801::doMeasurement()
{
	byte buffer[4];
	tmf8801_io.readMultipleBytes(REGISTER_RESULT_NUMBER, buffer, sizeof(buffer));
	resultNumber = buffer[0];
	resultInfo = buffer[1];
	distancePeak = buffer[3];
	distancePeak = distancePeak << 8;
	distancePeak += buffer[2];
}

int TMF8801::getDistance()
{
	clearInterruptFlag();
	doMeasurement();
	return distancePeak;
}

void TMF8801::enableInterrupt()
{
	byte registerValue = tmf8801_io.readSingleByte(REGISTER_INT_ENAB);
	registerValue |= INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(REGISTER_INT_ENAB, registerValue);
}

void TMF8801::disableInterrupt()
{
	byte registerValue = tmf8801_io.readSingleByte(REGISTER_INT_ENAB);
	registerValue &= ~INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(REGISTER_INT_ENAB, registerValue);
}

void TMF8801::clearInterruptFlag()
{
	byte registerValue = tmf8801_io.readSingleByte(REGISTER_INT_STATUS);
	registerValue |= INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(REGISTER_INT_STATUS, registerValue);
}

void TMF8801::updateCommandData8()
{
	tmf8801_io.writeMultipleBytes(REGISTER_CMD_DATA7, commandDataValues, sizeof(commandDataValues));
}

bool TMF8801::measurementEnabled()
{
	byte result = resultInfo;
	result = result >> 6;
	return result == 0;
}

void TMF8801::setGPIO0Mode(byte gpioMode)
{
	if (gpioMode > MODE_HIGH_OUTPUT)
		return;

	byte currentRegisterValue;

	currentRegisterValue = tmf8801_io.readSingleByte(CMD_DATA_5);
	currentRegisterValue &= 0xf0;
	currentRegisterValue |= gpioMode;
	
	commandDataValues[CMD_DATA_5] = currentRegisterValue;

	resetBoard();
}

byte TMF8801::getGPIO0Mode()
{
	return (tmf8801_io.readSingleByte(REGISTER_COMMAND) & 0x0f);
}

void TMF8801::setGPIO1Mode(byte gpioMode)
{
	if (gpioMode > MODE_HIGH_OUTPUT)
		return;

	byte currentRegisterValue;

	currentRegisterValue = tmf8801_io.readSingleByte(CMD_DATA_5);
	currentRegisterValue &= 0x0f;
	currentRegisterValue |= (gpioMode << 4);
	commandDataValues[CMD_DATA_5] = currentRegisterValue;

	resetBoard();
}

byte TMF8801::getGPIO1Mode()
{
	return (tmf8801_io.readSingleByte(REGISTER_COMMAND) >> 4);
}

void TMF8801::setSamplingPeriod(byte period)
{
	commandDataValues[CMD_DATA_2] = period;
	resetBoard();
}

byte TMF8801::getSamplingPeriod()
{
	return tmf8801_io.readSingleByte(CMD_DATA_2);
}

byte TMF8801::getRegisterValue(byte reg)
{
	return tmf8801_io.readSingleByte(reg);
}

void TMF8801::setRegisterValue(byte reg, byte value)
{
	tmf8801_io.writeSingleByte(reg, value);
}

void TMF8801::getRegisterMultipleValues(byte reg, byte* buffer, byte length)
{
	tmf8801_io.readMultipleBytes(reg, buffer, length);
}

void TMF8801::setRegisterMultipleValues(byte reg, const byte* buffer, byte length)
{
	tmf8801_io.writeMultipleBytes(reg, buffer, length);
}
