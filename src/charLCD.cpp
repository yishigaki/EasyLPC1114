#include "charLCD.h"

CharLCD::CharLCD(BusOut &databus, DigitalOut &regsel, DigitalOut &enable)
{
	DB = &databus;
	RS = &regsel;
	E = &enable;

	precision = DEFAULT_PRECISION;
}

void CharLCD::write(const uint8_t code, bool func)
{
	//data bus
	DB->write(code >> 4);

	//set function
	if(func == COMMAND){
		RS->write(0);
	}else{
		RS->write(1);
	}

	wait_cycle(2);

	//enable
	E->write(1);
	wait_cycle(10);
	E->write(0);
}

void CharLCD::write_cmd(const uint8_t cmd)
{
	write(cmd, COMMAND);
	write(cmd<<4, COMMAND);

	if((cmd == 0x01)||(cmd == 0x02)){ //clear display or return home
		wait_ms(2);   //wait 2ms
	}else{
		wait_us(50);	//wait 50us
	}
}

void CharLCD::print_char(const uint8_t data)
{
	write(data, CHARACTER);
	write(data<<4, CHARACTER);

	wait_us(50);  //wait 50us
}

void CharLCD::init()
{
	wait_ms(30);
	write(0x30, COMMAND);
	wait_ms(5);
	write(0x30, COMMAND);
	wait_ms(1);
	write(0x30, COMMAND);
	wait_ms(1);
	write(0x20, COMMAND);
	wait_ms(1);
	write_cmd(0x28);  //function set 4bit mode
	//write_cmd(0x08);  //display mode
	write_cmd(0x0C);
	write_cmd(0x06);  //entry mode
	write_cmd(0x02);  //cursor home

	clearAll();
}

void CharLCD::setCursor(uint8_t col, uint8_t row)
{
	uint8_t address;
	if(col >= NUM_CHAR_COL) col = 15;
	if(row >= NUM_CHAR_ROW) row = 1;
	address = 0x40 * row + col;
	write_cmd(0x80 | address);  //set cursor
}

void CharLCD::clearAll()
{
	write_cmd(0x01);
}

void CharLCD::print(const char *ptr)
{
	while(*ptr != 0){
		print_char(*ptr++);
	}
}

void CharLCD::print_sequence(const char *ptr, const uint32_t delay_ms)
{
	while(*ptr != 0){
		print_char(*ptr++);
		wait_ms(delay_ms);
	}
}

void CharLCD::printIntDecimal(int32_t number)
{
	//check sign
	if(number < 0){
		print_char('-');
		number = -number;
	}

	uint8_t numEachDigit;

	bool non_zero = false;
	uint32_t digit = power(10, MAX_DIGIT-1); //10^MAX_DIGIT
	//一桁ごとに数字をチェック 一桁ずつ表示
	for(uint8_t i=0; i<MAX_DIGIT; i++){
		numEachDigit = (uint8_t)(number / digit);

		non_zero |= (numEachDigit != 0);   //数字の頭に0が付かないように
		if(non_zero){
			print_char(0x30|numEachDigit); //ASCIIコードに変換して表示
		}

		number = number % digit;
		digit /= 10;
	}

	//一つもゼロでない数字がない場合
	if(non_zero == false){
		print_char(0x30); //print zero
	}
}

void CharLCD::printFloatDecimal(float number)
{
	//check sign
	if(number < 0){
		print_char('-');
		number = -number;
	}

	//rounding
	float scale = fpower(10.0, precision+1);
	float noPointNum_f = number * scale;
	uint32_t noPointNum_i = (uint32_t)noPointNum_f;
	uint32_t remainder = noPointNum_i % 10;  //一桁目

	if(remainder >= 5){
		noPointNum_i += 10;  //rounding
	}

	uint8_t numEachDigit;

	uint32_t digit_upperPoint = MAX_DIGIT - precision;  //小数点より上の桁数

	bool non_zero = false;
	uint32_t digit = power(10, MAX_DIGIT);
	//一桁ごとに数字をチェック
	for(uint8_t i=0; i<digit_upperPoint; i++){
		numEachDigit = (uint8_t)(noPointNum_i / digit);

		non_zero |= (numEachDigit != 0);   //数字の頭に0が付かないように
		if(non_zero){
			print_char(0x30|numEachDigit); //ASCIIコードに変換して表示
		}

		//number = number - (numEachDigit * digit); //modulo
		noPointNum_i = noPointNum_i % digit;
		digit /= 10;
	}
	//一つもゼロでない数字がない場合
	if(non_zero == false){
		print_char(0x30); //print zero
	}

	print_char('.');  //小数点の表示

	//小数点以下の表示
	for(uint8_t i=0; i<precision; i++){
		numEachDigit = (uint8_t)(noPointNum_i / digit);
		print_char(0x30|numEachDigit); //ASCIIコードに変換して表示
		//number = number - (numEachDigit * digit); //modulo
		noPointNum_i = noPointNum_i % digit;
		digit /= 10;
	}
}

uint32_t power(uint32_t number, uint32_t exponent)
{
	uint32_t val = 1;
	for(uint32_t i=0; i<exponent; i++){
		val *= number;
	}
	return val;
}

float fpower(float number, uint32_t exponent)
{
	float val = 1.0;
	for(uint32_t i=0; i<exponent; i++){
		val *= number;
	}
	return val;
}
