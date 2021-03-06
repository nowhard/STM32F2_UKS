#include "hd44780.h"

/* Private */
void HD44780_InitPins(void);
void HD44780_Cmd(uint8_t cmd);
void HD44780_Cmd4bit(uint8_t cmd);
void HD44780_Cmd8bit(uint8_t cmd);
void HD44780_Data(uint8_t data);
void HD44780_CursorSet(uint8_t col, uint8_t row);

typedef struct {
	uint8_t DisplayControl;
	uint8_t DisplayFunction;
	uint8_t DisplayMode;
	uint8_t Rows;
	uint8_t Cols;
	uint8_t currentX;
	uint8_t currentY;
} HD44780_Options_t;

HD44780_Options_t HD44780_Opts;

#define HD44780_SET_8BITMODE

void HD44780_Init(uint8_t cols, uint8_t rows) {
	DELAY_Init();
	
	HD44780_InitPins();
	
	//At least 40ms
	HD44780_Delay(40000);
	
	HD44780_Opts.Rows = rows;
	HD44780_Opts.Cols = cols;
	
	HD44780_Opts.currentX = 0;
	HD44780_Opts.currentY = 0;
	


#ifdef HD44780_SET_8BITMODE
	HD44780_Opts.DisplayFunction = HD44780_8BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
#else
	HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
#endif

	if (rows > 1) {
		HD44780_Opts.DisplayFunction |= HD44780_2LINE;
	}
	
	//Try to set 4bit mode
	HD44780_Cmd4bit(0x03);
	HD44780_Delay(4500);
	
	//Second try
	HD44780_Cmd4bit(0x03);
	HD44780_Delay(150);
	
	//Third goo!
	HD44780_Cmd4bit(0x03);
	HD44780_Delay(250);

#ifndef HD44780_SET_8BITMODE
	//Set 4-bit interface
	HD44780_Cmd4bit(0x02);
	HD44780_Delay(150);
#endif
	
	//set # lines, font size, etc.
	HD44780_Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

	//turn the display on with no cursor or blinking default
	HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
	HD44780_DisplayOn();

	//Clear lcd
	HD44780_Clear();

	//Default font directions
	HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
	HD44780_Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

	HD44780_Delay(150);
}

void HD44780_Reinit(uint8_t cols, uint8_t rows)
{
	HD44780_Opts.Rows = rows;
	HD44780_Opts.Cols = cols;

	HD44780_Opts.currentX = 0;
	HD44780_Opts.currentY = 0;



#ifdef HD44780_SET_8BITMODE
	HD44780_Opts.DisplayFunction = HD44780_8BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
#else
	HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
#endif

	if (rows > 1) {
		HD44780_Opts.DisplayFunction |= HD44780_2LINE;
	}

	//Try to set 4bit mode
//	HD44780_Cmd4bit(0x03);
//	HD44780_Delay(4500);
//
//	//Second try
//	HD44780_Cmd4bit(0x03);
//	HD44780_Delay(150);
//
//	//Third goo!
//	HD44780_Cmd4bit(0x03);
//	HD44780_Delay(250);

#ifndef HD44780_SET_8BITMODE
	//Set 4-bit interface
	HD44780_Cmd4bit(0x02);
	HD44780_Delay(150);
#endif

	//set # lines, font size, etc.
	HD44780_Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

	//turn the display on with no cursor or blinking default
	HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
	HD44780_DisplayOn();

	//Clear lcd
	HD44780_Clear();

	//Default font directions
	HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
	HD44780_Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

	HD44780_Delay(150);
}

void HD44780_InitPins(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(HD44780_RCC|HD44780_LIGHT_RCC, ENABLE);
	
	//Common settings
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_InitStruct.GPIO_Pin = HD44780_RS_PIN|HD44780_E_PIN|HD44780_D0_PIN|HD44780_D1_PIN|HD44780_D2_PIN|HD44780_D3_PIN|HD44780_D4_PIN|HD44780_D5_PIN|HD44780_D6_PIN|HD44780_D7_PIN;
	GPIO_Init(HD44780_PORT, &GPIO_InitStruct);
	//GPIO_WriteBit(HD44780_RS_PORT, HD44780_RS_PIN, Bit_RESET);
	HD44780_PORT->BSRRH=HD44780_RS_PIN|HD44780_E_PIN|HD44780_D0_PIN|HD44780_D1_PIN|HD44780_D2_PIN|HD44780_D3_PIN|HD44780_D4_PIN|HD44780_D5_PIN|HD44780_D6_PIN|HD44780_D7_PIN;
	//RS
//	GPIO_InitStruct.GPIO_Pin = HD44780_RS_PIN;
//	GPIO_Init(HD44780_RS_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_RS_PORT, HD44780_RS_PIN, Bit_RESET);
//
//	//E
//	GPIO_InitStruct.GPIO_Pin = HD44780_E_PIN;
//	GPIO_Init(HD44780_E_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_E_PORT, HD44780_E_PIN, Bit_RESET);
//
//	//D0
//	GPIO_InitStruct.GPIO_Pin = HD44780_D0_PIN;
//	GPIO_Init(HD44780_D0_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D0_PORT, HD44780_D0_PIN, Bit_RESET);
//
//	//D1
//	GPIO_InitStruct.GPIO_Pin = HD44780_D1_PIN;
//	GPIO_Init(HD44780_D1_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D1_PORT, HD44780_D1_PIN, Bit_RESET);
//
//	//D2
//	GPIO_InitStruct.GPIO_Pin = HD44780_D2_PIN;
//	GPIO_Init(HD44780_D2_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D2_PORT, HD44780_D2_PIN, Bit_RESET);
//
//	//D3
//	GPIO_InitStruct.GPIO_Pin = HD44780_D3_PIN;
//	GPIO_Init(HD44780_D3_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D3_PORT, HD44780_D3_PIN, Bit_RESET);
//
//	//D4
//	GPIO_InitStruct.GPIO_Pin = HD44780_D4_PIN;
//	GPIO_Init(HD44780_D4_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D4_PORT, HD44780_D4_PIN, Bit_RESET);
//
//	//D5
//	GPIO_InitStruct.GPIO_Pin = HD44780_D5_PIN;
//	GPIO_Init(HD44780_D5_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D5_PORT, HD44780_D5_PIN, Bit_RESET);
//
//	//D6
//	GPIO_InitStruct.GPIO_Pin = HD44780_D6_PIN;
//	GPIO_Init(HD44780_D6_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D6_PORT, HD44780_D6_PIN, Bit_RESET);
//
//	//D7
//	GPIO_InitStruct.GPIO_Pin = HD44780_D7_PIN;
//	GPIO_Init(HD44780_D7_PORT, &GPIO_InitStruct);
//	GPIO_WriteBit(HD44780_D7_PORT, HD44780_D7_PIN, Bit_RESET);
	//LIGHT
	GPIO_InitStruct.GPIO_Pin = HD44780_LIGHT_PIN;
	GPIO_Init(HD44780_LIGHT_PORT, &GPIO_InitStruct);
	GPIO_WriteBit(HD44780_LIGHT_PORT, HD44780_LIGHT_PIN, Bit_SET);
	//HD44780_LIGHT_PORT->BSRRL=HD44780_LIGHT_PIN;
}

void HD44780_Clear(void) {
	HD44780_Cmd(HD44780_CLEARDISPLAY);
	HD44780_Delay(2000);
}

void HD44780_Cmd(uint8_t cmd) {
	HD44780_RS_LOW;
#ifdef HD44780_SET_8BITMODE
	HD44780_Cmd8bit(cmd);
#else
	HD44780_Cmd4bit(cmd >> 4);			//High nibble
	HD44780_Cmd4bit(cmd & 0x0F);			//Low nibble
#endif
}

void HD44780_Data(uint8_t data) {
	HD44780_RS_HIGH;
#ifdef HD44780_SET_8BITMODE
	HD44780_Cmd8bit(data);
#else
	HD44780_Cmd4bit(data >> 4);			//High nibble
	HD44780_Cmd4bit(data & 0x0F);		//Low nibble
#endif
}

#define HD44780_DATA_BUS_4_BIT_SHIFT 6
#define HD44780_DATA_BUS_4_BIT_MASK 0xFC3F
#define HD44780_DATA_BUS_4_BIT_MASK_INV 0x03C0

void HD44780_Cmd4bit(uint8_t cmd) {
//	GPIO_WriteBit(HD44780_D7_PORT, HD44780_D7_PIN, (cmd & 0x08) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D6_PORT, HD44780_D6_PIN, (cmd & 0x04) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D5_PORT, HD44780_D5_PIN, (cmd & 0x02) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D4_PORT, HD44780_D4_PIN, (cmd & 0x01) != 0 ? Bit_SET : Bit_RESET);
	uint16_t temp=(uint16_t)cmd;
	temp=(temp<<HD44780_DATA_BUS_4_BIT_SHIFT)&HD44780_DATA_BUS_4_BIT_MASK_INV;
	HD44780_PORT->ODR&=HD44780_DATA_BUS_4_BIT_MASK;
	HD44780_PORT->ODR|=temp;

	HD44780_E_BLINK;

	HD44780_PORT->ODR&=HD44780_DATA_BUS_4_BIT_MASK;
}
#define HD44780_DATA_BUS_SHIFT 2
#define HD44780_DATA_BUS_MASK 0xFC03
#define HD44780_DATA_BUS_MASK_INV 0x03FC
void HD44780_Cmd8bit(uint8_t cmd)
{
//	GPIO_WriteBit(HD44780_D7_PORT, HD44780_D7_PIN, (cmd & 0x80) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D6_PORT, HD44780_D6_PIN, (cmd & 0x40) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D5_PORT, HD44780_D5_PIN, (cmd & 0x20) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D4_PORT, HD44780_D4_PIN, (cmd & 0x10) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D3_PORT, HD44780_D3_PIN, (cmd & 0x08) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D2_PORT, HD44780_D2_PIN, (cmd & 0x04) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D1_PORT, HD44780_D1_PIN, (cmd & 0x02) != 0 ? Bit_SET : Bit_RESET);
//	GPIO_WriteBit(HD44780_D0_PORT, HD44780_D0_PIN, (cmd & 0x01) != 0 ? Bit_SET : Bit_RESET);

	uint16_t temp=(uint16_t)cmd;
	temp=(temp<<HD44780_DATA_BUS_SHIFT)&HD44780_DATA_BUS_MASK_INV;
	HD44780_PORT->ODR&=HD44780_DATA_BUS_MASK;
	HD44780_PORT->ODR|=temp;

	HD44780_E_BLINK;

//	GPIO_WriteBit(HD44780_D7_PORT, HD44780_D7_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D6_PORT, HD44780_D6_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D5_PORT, HD44780_D5_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D4_PORT, HD44780_D4_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D3_PORT, HD44780_D3_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D2_PORT, HD44780_D2_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D1_PORT, HD44780_D1_PIN,  Bit_RESET);
//	GPIO_WriteBit(HD44780_D0_PORT, HD44780_D0_PIN,  Bit_RESET);
	HD44780_PORT->ODR&=HD44780_DATA_BUS_MASK;
}

void HD44780_CursorSet(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	
	//Go to the beginning
	if (row >= HD44780_Opts.Rows) {
		row = 0;
	}
	
	HD44780_Opts.currentX = col;
	HD44780_Opts.currentY = row;
	
	HD44780_Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

void HD44780_Puts(uint8_t x, uint8_t y, char* str) {
	HD44780_CursorSet(x, y);
	while (*str) {
		if (HD44780_Opts.currentX >= HD44780_Opts.Cols) {
			HD44780_Opts.currentX = 0;
			HD44780_Opts.currentY++;
			HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		}
		if (*str == '\n') {
			HD44780_Opts.currentY++;
			HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		} else if (*str == '\r') {
			HD44780_CursorSet(0, HD44780_Opts.currentY);
		} else {
			HD44780_Data(*str);
			HD44780_Opts.currentX++;
		}
		str++;
	}
}

void HD44780_DisplayOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_DisplayOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_BlinkOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_BLINKON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_BlinkOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_CursorOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_CURSORON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_CursorOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
	HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780_ScrollLeft(void) {
	HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void HD44780_ScrollRight(void) {
	HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void HD44780_CreateChar(uint8_t location, uint8_t *data) {
	uint8_t i;
	location &= 0x07;	//8 Free locations for custom chars
	HD44780_Cmd(HD44780_SETCGRAMADDR | (location << 3));
	
	for (i = 0; i < 8; i++) {
		HD44780_Data(data[i]);
	}
}

void HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location) {
	HD44780_CursorSet(x, y);
	HD44780_Data(location);
}

