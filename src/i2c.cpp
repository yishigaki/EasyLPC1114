#include "i2c.h"

volatile uint32_t I2CMasterState = I2C_IDLE;
volatile uint32_t timeout = 0;

volatile uint8_t I2CWriteBuffer[BUFSIZE];
volatile uint8_t I2CReadBuffer[BUFSIZE];
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;

void I2C_IRQHandler(void)
{
  uint8_t StatValue;

  timeout = 0;
  /* this handler deals with master read and master write only */
  StatValue = LPC_I2C->STAT;
  switch ( StatValue )
  {
	case 0x08:			/* A Start condition is issued. */
	WrIndex = 0;
	LPC_I2C->DAT = I2CWriteBuffer[WrIndex++];
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	break;

	case 0x10:			/* A repeated started is issued */
	RdIndex = 0;
	/* Send SLA with R bit set, */
	LPC_I2C->DAT = I2CWriteBuffer[WrIndex++];
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	break;

	case 0x18:			/* Regardless, it's a ACK */
	if ( I2CWriteLength == 1 ){
	  LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	  I2CMasterState = I2C_NO_DATA;
	}else{
	  LPC_I2C->DAT = I2CWriteBuffer[WrIndex++];
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
	if ( WrIndex < I2CWriteLength ){
	  LPC_I2C->DAT = I2CWriteBuffer[WrIndex++]; /* this should be the last one */
	}else{
	  if ( I2CReadLength != 0 ){
		LPC_I2C->CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
	  }else{
		LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
		I2CMasterState = I2C_OK;
	  }
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x30:
	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	I2CMasterState = I2C_NACK_ON_DATA;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x40:	/* Master Receive, SLA_R has been sent */
	if ( (RdIndex + 1) < I2CReadLength ){
	  /* Will go to State 0x50 */
	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	}else{
	  /* Will go to State 0x58 */
	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK after data is received */
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
	I2CReadBuffer[RdIndex++] = LPC_I2C->DAT;
	if ( (RdIndex + 1) < I2CReadLength ){
	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	}else{
	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x58:
	I2CReadBuffer[RdIndex++] = LPC_I2C->DAT;
	I2CMasterState = I2C_OK;
	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = I2CONCLR_SIC;	/* Clear SI flag */
	break;

	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	I2CMasterState = I2C_NACK_ON_ADDRESS;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x38:		/* Arbitration lost, in this example, we don't
					deal with multiple master situation */
	default:
	I2CMasterState = I2C_ARBITRATION_LOST;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;
  }
  return;
}


I2C_Master::I2C_Master(uint8_t i2c_mode)
{
	LPC_SYSCON->PRESETCTRL |= (0x1<<1);   //I2C reset de-asserted
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);  //system clock 供給

	uint32_t i2c_pclk = SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV;
	uint32_t i2c_freq;
	uint32_t half_cycle;
	//set pin functions & I2C frequency
	if(i2c_mode==STANDARD_MODE){
		LPC_IOCON->PIO0_4 = 0x01;  //SCL
		LPC_IOCON->PIO0_5 = 0x01;  //SDA
		i2c_freq = 1e5;				//100kHz
		half_cycle = i2c_pclk / i2c_freq / 2;

	}else if(i2c_mode==FAST_MODE){
		LPC_IOCON->PIO0_4 = 0x01;  //SCL
		LPC_IOCON->PIO0_5 = 0x01;  //SDA
		i2c_freq = 4e5;				//400kHz
		half_cycle = i2c_pclk / i2c_freq / 2;

	}else if(i2c_mode==FAST_MODE_PLUS){
		LPC_IOCON->PIO0_4 = 0x201;  //SCL
		LPC_IOCON->PIO0_5 = 0x201;  //SDA
		i2c_freq = 1e6;				//1MHz
		half_cycle = i2c_pclk / i2c_freq / 2;
	}

	LPC_I2C->SCLH = half_cycle;
	LPC_I2C->SCLL = half_cycle;

	//clear all bit in control register
	LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;

	 NVIC_EnableIRQ(I2C_IRQn);  //enable interrupt

	 LPC_I2C->CONSET = I2CONSET_I2EN; //I2C interface enable

}

uint32_t I2C_Master::write(uint8_t address, const char *data, uint32_t length)
{
	I2CWriteLength = length + 1; // address + data
	I2CReadLength = 0;

	//copy data to buffer
	I2CWriteBuffer[0] = address << 1;
	for(uint32_t i=1; i<I2CWriteLength; i++){
		I2CWriteBuffer[i] = data[i-1];
	}
	uint32_t state = engine();

	return state;
}

uint32_t I2C_Master::read(uint8_t address, char *data, uint32_t length)
{
	I2CWriteLength = 1;		// address
	I2CReadLength = length; // data

	//copy data to buffer
	I2CWriteBuffer[0] = (address << 1) | 0x01;  //add read flag(0x01)

	uint32_t state = engine();

	for(uint32_t i=0; i<I2CReadLength; i++){
		data[i] = I2CReadBuffer[i];
	}

	return state;
}

uint32_t I2C_Master::start()
{
	uint32_t timeout = 0;
	uint32_t retVal = false;

	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */

	/*--- Wait until START transmitted ---*/
	while( 1 ){
		if ( I2CMasterState == I2C_STARTED ){
		  retVal = true;
		  break;
		}
		if ( timeout >= MAX_TIMEOUT ){
		  retVal = false;
		  break;
		}
		timeout++;
	}
	return retVal;
}

uint32_t I2C_Master::stop()
{
	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	LPC_I2C->CONCLR = I2CONCLR_SIC;  /* Clear SI flag */

	/*--- Wait for STOP detected ---*/
	while( LPC_I2C->CONSET & I2CONSET_STO );
	return true;
}

uint32_t I2C_Master::engine()
{
	RdIndex = 0;
	WrIndex = 0;

	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */

	I2CMasterState = I2C_BUSY;

	while ( I2CMasterState == I2C_BUSY ){
		if ( timeout >= MAX_TIMEOUT ){
		  I2CMasterState = I2C_TIME_OUT;
		  break;
		}
		timeout++;
	}
	LPC_I2C->CONCLR = I2CONCLR_STAC;

	return I2CMasterState;

}
