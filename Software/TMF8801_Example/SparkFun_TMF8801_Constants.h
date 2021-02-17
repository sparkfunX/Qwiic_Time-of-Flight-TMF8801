/*
  This is a library written for the AMS TMF-8801 Time-of-flight sensor
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 15th, 2021
  This file holds all constants used by the TMF-8801 ToF sensor.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __TMF8801_CONSTANTS__
#define __TMF8801_CONSTANTS__

#include <stdint.h>

// Constants definitions
const uint8_t DEFAULT_I2C_ADDR = 0x41;
const uint8_t CPU_READY_TIMEOUT = 200;
const uint8_t APPLICATION_READY_TIMEOUT = 200;
const uint8_t CHIP_ID_NUMBER = 0x07; 
const uint8_t APPLICATION = 0xc0;
const uint8_t BOOTLOADER = 0x80;
const uint8_t COMMAND_CALIBRATION = 0x0b;
const uint8_t COMMAND_FACTORY_CALIBRATION = 0x0a;
const uint8_t COMMAND_MEASURE = 0x02;
const uint8_t CONTENT_CALIBRATION = 0x0a;
const uint8_t COMMAND_RESULT = 0x55;
const uint8_t INTERRUPT_MASK = 0x01;
const uint8_t COMMAND_SERIAL = 0x47;
const uint8_t COMMAND_STOP = 0xff;

// Values below were taken from AN000597, pp 22
const uint8_t ALGO_STATE[11] = { 0xB1, 0xA9, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Bit fields definitions
// Each bit position value has an UPPERCASE preffix which indicates which register it belongs to. 
// The lowercase values are the actual bit position in the register, i.e.
// ENABLE_cpu_reset = 7 denotes that cpu_reset is the seventh bit in ENABLE register
const uint8_t ENABLE_cpu_reset = 7;
const uint8_t ENABLE_cpu_ready = 6;

// This enum has all errors that can be written to Error variable
enum class TMF8801_Errors : uint8_t
{
	I2C_COMM_ERROR,
	CPU_RESET_TIMEOUT,
	WRONG_CHIP_ID,
	CPU_LOAD_APPLICATION_ERROR,
	FACTORY_CALIBRATION_ERROR,
	NONE
};

enum COMMAND_DATA_8_REGISTERS : uint8_t
{
	CMD_DATA_7,
	CMD_DATA_6,
	CMD_DATA_5,
	CMD_DATA_4,
	CMD_DATA_3,
	CMD_DATA_2,
	CMD_DATA_1,
	CMD_DATA_0
};

enum class TMF8801_GPIO_MODE : uint8_t
{
	MODE_INPUT,
	MODE_LOW_INPUT,
	MODE_HIGH_INPUT,
	MODE_VCSEL,
	MODE_LOW_OUTPUT,
	MODE_HIGH_OUTPUT,
	MODE_RESERVED_6,
	MODE_RESERVED_7,
	MODE_RESERVED_8,
	MODE_RESERVED_9,
	MODE_RESERVED_10,
	MODE_RESERVED_11,
	MODE_RESERVED_12,
	MODE_RESERVED_13,
	MODE_RESERVED_14,
	MODE_RESERVED_15,
};

// Registers definitions
enum class TMF8801_Registers : uint8_t
{
	APPID                  = 0x00,
	APPREQID               = 0x02,
	APPREV_MAJOR           = 0x01,
	APPREV_MINOR           = 0x12,
	APPREV_PATCH           = 0x13,
	CMD_DATA9              = 0x06,
	CMD_DATA8              = 0x07,
	CMD_DATA7              = 0x08,
	CMD_DATA6              = 0x09,
	CMD_DATA5              = 0x0A,
	CMD_DATA4              = 0x0B,
	CMD_DATA3              = 0x0C,
	CMD_DATA2              = 0x0D,
	CMD_DATA1              = 0x0E,
	CMD_DATA0              = 0x0F,
	COMMAND                = 0x10,
	PREVIOUS               = 0x11,
	STATUS                 = 0x1D,
	REGISTER_CONTENTS      = 0x1E,
	TID                    = 0x1F,
	RESULT_NUMBER          = 0x20,
	RESULT_INFO            = 0x21,
	DISTANCE_PEAK_0        = 0x22,
	DISTANCE_PEAK_1        = 0x23,
	SYS_CLOCK_0            = 0x24,
	SYS_CLOCK_1            = 0x25,
	SYS_CLOCK_2            = 0x26,
	SYS_CLOCK_3            = 0x27,
	STATE_DATA_0           = 0x28,
	STATE_DATA_1           = 0x29,
	STATE_DATA_2           = 0x2A,
	STATE_DATA_3           = 0x2B,
	STATE_DATA_4           = 0x2C,
	STATE_DATA_5           = 0x2D,
	STATE_DATA_6           = 0x2E,
	STATE_DATA_7           = 0x2F,
	STATE_DATA_8_XTALK_MSB = 0x30,
	STATE_DATA_9_XTALK_LSB = 0x31,
	STATE_DATA_10_TJ       = 0x32,
	REFERENCE_HITS_0       = 0x33,
	REFERENCE_HITS_1       = 0x34,
	REFERENCE_HITS_2       = 0x35,
	REFERENCE_HITS_3       = 0x36,
	OBJECT_HITS_0          = 0x37,
	OBJECT_HITS_1          = 0x38,
	OBJECT_HITS_2          = 0x39,
	OBJECT_HITS_3          = 0x3A,
	FACTORY_CALIB_0        = 0x20,
	STATE_DATA_WR_0        = 0x2E,
	ENABLE_REG             = 0xE0,
	INT_STATUS             = 0xE1,
	INT_ENAB               = 0xE2,
	ID                     = 0xE3,
	REVID                  = 0xE4
};
#endif