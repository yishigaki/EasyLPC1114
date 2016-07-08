#pragma once

#include "LPC11xx.h"
#include "digital_io.h"
#include "timer.h"

#define LCD_DATABUS_LENGTH 4  //4bit
#define COMMAND 0
#define CHARACTER 1
#define NUM_CHAR_COL 16  //the number of characters in column
#define NUM_CHAR_ROW 2	//the number of characters in row
#define MAX_DIGIT 8		//最大桁数
#define DEFAULT_PRECISION 2  //小数点以下桁数（デフォルト）

class CharLCD {
private:
	BusOut *DB;  //data bus
	DigitalOut *RS;  //register select
	DigitalOut *E;   //enable
	uint8_t precision;  //小数点以下の桁数  小数点以下は切り捨て
public:
	CharLCD(BusOut &databus, DigitalOut &regsel, DigitalOut &enable);
	void write(const uint8_t code, bool func);
	void write_cmd(const uint8_t cmd);
	void print_char(const uint8_t data);
	void init();
	void setCursor(uint8_t col, uint8_t row);
	void clearAll();
	void setprecision(uint8_t pre){precision = pre;}
	void print(const char *ptr);
	void print_sequence(const char *ptr, const uint32_t delay_ms);
	void printIntDecimal(int32_t number);
	void printFloatDecimal(float number);  //小数点以下は切り捨て
};

uint32_t power(uint32_t number, uint32_t exponent);
float fpower(float number, uint32_t exponent);
