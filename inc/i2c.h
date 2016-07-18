#pragma once

#include "LPC11xx.h"

//I2C mode
#define STANDARD_MODE 0
#define FAST_MODE 1
#define FAST_MODE_PLUS 2

//----
#define BUFSIZE             64
#define MAX_TIMEOUT         0x00FFFFFF

#define READ_WRITE          0x01

//#define RD_BIT              0x01

#define I2C_IDLE              0
#define I2C_STARTED           1
#define I2C_RESTARTED         2
#define I2C_REPEATED_START    3
#define DATA_ACK              4
#define DATA_NACK             5
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_NACK_ON_DATA      9
#define I2C_ARBITRATION_LOST  10
#define I2C_TIME_OUT          11
#define I2C_OK                12
//----

// I2C Control Set Register
#define I2CONSET_I2EN       (0x1<<6)
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

//I2C Control clear Register
#define I2CONCLR_AAC        (0x1<<2)
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#ifdef __cplusplus
extern "C" {
#endif
//interrupt handler
void I2C_IRQHandler(void);

#ifdef __cplusplus
}
#endif

class I2C_Master {
private:
public:
	I2C_Master(uint8_t i2c_mode);
	uint32_t write(uint8_t address, const char *data, uint32_t length);  //address: 7bit mode
	uint32_t read(uint8_t address, char *data, uint32_t length);
	uint32_t start();
	uint32_t stop();
	uint32_t engine();
};
