/*
  This is a library written for the AMS TMF-8801 Time-of-flight sensor
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 15th, 2021
  This file handles I2C communications between the board and the host microcontroller.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include "SparkFun_TMF8801_IO.h"
#include "SparkFun_TMF8801_Constants.h"

bool TMF8801_IO::begin(uint8_t address, TwoWire& wirePort)
{
	_i2cPort = &wirePort;
	_address =address;
	_i2cPort->begin();
	return isConnected();
}

bool TMF8801_IO::isConnected()
{
	_i2cPort->beginTransmission(_address);
	if (_i2cPort->endTransmission() != 0)
		return (false);
	return (true); 
}

void TMF8801_IO::writeMultipleBytes(TMF8801_Registers const registerName, const uint8_t* buffer, uint8_t const packetLength)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(static_cast<uint8_t>(registerName));
	for (uint8_t i = 0; i < packetLength; i++) 
		_i2cPort->write(buffer[i]);
	
	_i2cPort->endTransmission();
}

void TMF8801_IO::readMultipleBytes(TMF8801_Registers const registerName, uint8_t* buffer, uint8_t const packetLength)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(static_cast<uint8_t>(registerName));
	_i2cPort->endTransmission();

	_i2cPort->requestFrom(_address, packetLength);
	for (uint8_t i = 0; (i < packetLength) && _i2cPort->available(); i++)
		buffer[i] = _i2cPort->read();
}

uint8_t TMF8801_IO::readSingleByte(TMF8801_Registers const registerName)
{
	uint8_t result;
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(static_cast<uint8_t>(registerName));
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, 1U);
	result = _i2cPort->read();
	return result;
}

void TMF8801_IO::writeSingleByte(TMF8801_Registers const registerName, uint8_t const value)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(static_cast<uint8_t>(registerName));
	_i2cPort->write(value);
	_i2cPort->endTransmission();
}

void TMF8801_IO::setRegisterBit(TMF8801_Registers const registerName, uint8_t const bitPosition)
{
	uint8_t value = readSingleByte(registerName);
	value |= (1 << bitPosition);
	writeSingleByte(registerName, value);
}

void TMF8801_IO::clearRegisterBit(TMF8801_Registers const registerName, uint8_t const bitPosition)
{
	uint8_t value = readSingleByte(registerName);
	value &= ~(1 << bitPosition);
	writeSingleByte(registerName, value);
}

bool TMF8801_IO::isBitSet(TMF8801_Registers const registerName, uint8_t const bitPosition)
{
	uint8_t value = readSingleByte(registerName);
	uint8_t mask = 1 << bitPosition;
	if (value & mask)
		return true;
	else
		return false;
}