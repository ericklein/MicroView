#include <avr/pgmspace.h>
#include <MicroView.h>
#include <SPI.h>

/*
Screen memory buffer 64 x 48 divided by 8 = 384 bytes
Screen memory buffer is required because in SPI mode, the host cannot read the GDRAM of the controller.  This buffer serves as a scratch RAM for graphical functions.
*/
static uint8_t screenmemory [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0,
	0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
	0xC0, 0xC0, 0xE0, 0xE0, 0xF0, 0x78, 0xF8, 0xBC, 0xFC, 0xDE, 0xDE, 0xCF, 0xCF, 0xC7, 0xC3, 0xC3,
	0xC7, 0x87, 0x8F, 0x0F, 0x1E, 0x1E, 0x3C, 0x78, 0x78, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFF, 0xEF, 0xE7, 0xC7,
	0xC3, 0x83, 0x01, 0x01, 0x00, 0x0F, 0x1F, 0x3F, 0x71, 0x6E, 0xEE, 0xEF, 0xF3, 0xF7, 0xF6, 0xF9,
	0xFD, 0xDD, 0xEB, 0x7F, 0x7E, 0xBC, 0xC0, 0xC0, 0xE0, 0xE0, 0xF0, 0xF1, 0x79, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x0F, 0x1F, 0x1E, 0xFC, 0xFD, 0xF9, 0x7B,
	0xF3, 0xE7, 0xEF, 0xCF, 0xDE, 0x9E, 0xBC, 0x3C, 0x78, 0x78, 0xF0, 0xF0, 0xF0, 0xF8, 0xF8, 0xFC,
	0xBC, 0xFE, 0xFE, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00,
	0x00, 0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x1E, 0x3F, 0x3F, 0x3F, 0x1F, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x1F, 0x3F, 0x3F, 0x1F, 0x03, 0x07,
	0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F,
	0x0F, 0x07, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void MICROVIEW::begin() {
	// Setting up SPI pins
	pinMode(MOSI, OUTPUT);
	pinMode(SCK, OUTPUT);
	pinMode(DC, OUTPUT);
	pinMode(RESET, OUTPUT);
	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
	
	sckport     = portOutputRegister(digitalPinToPort(SCK));
	sckpinmask  = digitalPinToBitMask(SCK);
	mosiport    = portOutputRegister(digitalPinToPort(MOSI));
	mosipinmask = digitalPinToBitMask(MOSI);
	csport      = portOutputRegister(digitalPinToPort(CS));
	cspinmask   = digitalPinToBitMask(CS);
	dcport      = portOutputRegister(digitalPinToPort(DC));
	dcpinmask   = digitalPinToBitMask(DC);
	
	digitalWrite(RESET, HIGH);
	// VDD (3.3V) goes high at start, lets just chill for 5 ms
	delay(5);
	// bring reset low
	digitalWrite(RESET, LOW);
	
	// Setup SPI frequency
	SPI.setClockDivider(SPI_CLOCK_DIV2); 
	SPI.begin();
	
	// wait 10ms
	delay(10);
	// bring out of reset
	digitalWrite(RESET, HIGH);

	// Init sequence for 64x48 OLED module
	command(DISPLAYOFF);			// 0xAE

	command(SETDISPLAYCLOCKDIV);	// 0xD5
	command(0x80);					// the suggested ratio 0x80

	command(SETMULTIPLEX);			// 0xA8
	command(0x2F);

	command(SETDISPLAYOFFSET);		// 0xD3
	command(0x0);					// no offset

	command(SETSTARTLINE | 0x0);	// line #0

	command(CHARGEPUMP);			// enable charge pump
	command(0x14);

	command(NORMALDISPLAY);			// 0xA6
	command(DISPLAYALLONRESUME);	// 0xA4

	command(SEGREMAP | 0x1);

	command(COMSCANDEC);

	command(SETCOMPINS);			// 0xDA
	command(0x12);

	command(SETCONTRAST);			// 0x81
	command(0x8F);

	command(SETPRECHARGE);			// 0xd9
	command(0xF1);
	
	command(SETVCOMDESELECT);			// 0xDB
	command(0x40);

	command(DISPLAYON);				//--turn on oled panel
}

void MICROVIEW::command(uint8_t c) {
	// Hardware SPI
	*csport |= cspinmask;	// CS HIGH
	*dcport &= ~dcpinmask;	// DC LOW
	*csport &= ~cspinmask;	// CS LOW
	SPI.transfer(c);
	*csport |= cspinmask;	// CS HIGH
}

void MICROVIEW::data(uint8_t c) {
	// Hardware SPI
	*csport |= cspinmask;	// CS HIGH
	*dcport |= dcpinmask;	// DC HIGH
	*csport &= ~cspinmask;	// CS LOW
	SPI.transfer(c);
	*csport |= cspinmask;	// CS HIGH
}

void MICROVIEW::setPageAddress(uint8_t add) {
	add=0xb0|add;
	command(add);
	return;
}

void MICROVIEW::setColumnAddress(uint8_t add) {
	command((0x10|(add>>4))+0x02);
	command((0x0f&add));
	return;
}


/* 
	Clear GDRAM inside the LCD controller - mode = ALL
	Clear screen page buffer - mode = PAGE
*/
void MICROVIEW::clear(uint8_t mode) {
	uint8_t page=6, col=0x40;
	if (mode==ALL) {

		for (int i=0;i<8; i++) {
			setPageAddress(i);
			setColumnAddress(0);
			for (int j=0; j<0x80; j++) {
				data(0);
			}
		}
	}
	else
	{
		memset(screenmemory,0,384);			// (64 x 48) / 8 = 384
		display();
	}
}

void MICROVIEW::display(void) {
	uint8_t i, j;
	
	for (i=0; i<6; i++) {
		setPageAddress(i);
		setColumnAddress(0);
		for (j=0;j<0x40;j++) {
			data(screenmemory[i*0x40+j]);
		}
	}
}

void MICROVIEW::pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t mode) {
	if ((x<0) ||  (x>LCDWIDTH-1) || (y<0) || (y>LCDHEIGHT-1))
	return;
	
	if (mode==XOR) {
		screenmemory[x+ (y/8)*LCDWIDTH] ^= _BV((y%8));
	}
	else {
		if (color==WHITE)
		screenmemory[x+ (y/8)*LCDWIDTH] |= _BV((y%8));
		else
		screenmemory[x+ (y/8)*LCDWIDTH] &= ~_BV((y%8)); 
	}
	
	//display();
}

// bresenham's algorithm 
void MICROVIEW::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color, uint8_t mode) {
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;}

	for (; x0<x1; x0++) {
		if (steep) {
			pixel(y0, x0, color, mode);
		} else {
			pixel(x0, y0, color, mode);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}	
}

void MICROVIEW::lineH(uint8_t x, uint8_t y, uint8_t width, uint8_t color, uint8_t mode) {
	line(x,y,x+width,y,color,mode);
}

void MICROVIEW::lineV(uint8_t x, uint8_t y, uint8_t height, uint8_t color, uint8_t mode) {
	line(x,y,x,y+height,color,mode);
}

void MICROVIEW::rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color , uint8_t mode) {
	lineH(x,y, width, color, mode);
	lineV(x,y, height, color, mode);
	lineH(x,y+height-1, width, color, mode);
	lineV(x+width-1, y, height, color, mode);
}

void MICROVIEW::rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color , uint8_t mode) {
	for (int i=x; i<x+width;i++) {
		lineV(i,y, height, color, mode);
	}
}

void MICROVIEW::circle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color, uint8_t mode) {
  int8_t f = 1 - radius;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * radius;
  int8_t x = 0;
  int8_t y = radius;

  pixel(x0, y0+radius, color, mode);
  pixel(x0, y0-radius, color, mode);
  pixel(x0+radius, y0, color, mode);
  pixel(x0-radius, y0, color, mode);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    pixel(x0 + x, y0 + y, color, mode);
    pixel(x0 - x, y0 + y, color, mode);
    pixel(x0 + x, y0 - y, color, mode);
    pixel(x0 - x, y0 - y, color, mode);
    
    pixel(x0 + y, y0 + x, color, mode);
    pixel(x0 - y, y0 + x, color, mode);
    pixel(x0 + y, y0 - x, color, mode);
    pixel(x0 - y, y0 - x, color, mode);
    
  }
}

void MICROVIEW::stopScroll(void){
	command(DEACTIVATESCROLL);
}

void MICROVIEW::scrollRight(uint8_t start, uint8_t stop){
	if (stop<start)		// stop must be larger or equal to start
	return;
	stopScroll();		// need to disable scrolling before starting to avoid memory corrupt
	command(RIGHTHORIZONTALSCROLL);
	command(0x00);
	command(start);
	command(0x7);		// scroll speed frames , TODO
	command(stop);
	command(0x00);
	command(0xFF);
	command(ACTIVATESCROLL);
}



