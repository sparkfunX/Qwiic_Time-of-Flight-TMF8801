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

#ifndef __TMF8801_LIBRARY_IO__
#define __TMF8801_LIBRARY_IO__

#include <stdint.h>
#include <Wire.h>
#include "SparkFun_TMF8801_Constants.h"

class TMF8801_IO
{
private:
	TwoWire* _i2cPort;
	byte _address;

public:
	TMF8801_IO() {}
	bool begin(byte address, TwoWire& wirePort);
	bool isConnected();
	byte readSingleByte(byte registerAddress);
	void writeSingleByte(byte registerAddress, byte value);
	void readMultipleBytes(byte registerAddress, byte* buffer, byte packetLength);
	void writeMultipleBytes(byte registerAddress, const byte* buffer, byte packetLength);
	void setRegisterBit(byte registerAddress, byte bitPosition);
	void clearRegisterBit(byte registerAddress, byte bitPosition);
	bool isBitSet(byte registerAddress, byte bitPosition);
};
#endif