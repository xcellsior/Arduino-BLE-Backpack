// include statements
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <math.h>

// definitions
#define PIN 6
#define M_2PI   6.283185307179586476925286766559005768394338798750
#define M_1_2PI 0.159154943091895335768883763372514362034459645740

// global variables
Adafruit_NeoPixel strip = Adafruit_NeoPixel(512, PIN, NEO_GRB + NEO_KHZ800);
const uint8_t columns = 16;
const uint8_t rows = 32;
extern const uint8_t gammaTable[]; // gamma correction table
uint8_t indexer = 0;
uint32_t colorTable[rows+1];

// initialization
void setup()
{
	strip.begin();
	strip.show(); // initialize all pixels to 'off'
	/* create color table */
	for (int row = 0; row < rows+1; row++)
	{
		colorTable[row] = gamma(0, sinWave(row, 128, 0, 128, 1), sinWave(row, 128, 90, 128, 1));
	}
	/* power-on self test */
	colorWipe(strip.Color(16, 0, 0), 1); // Red
	colorWipe(strip.Color(0, 16, 0), 2); // Green
	colorWipe(strip.Color(0, 0, 16), 3); // Blue
}

// main
void loop()
{
	breathing(128);
}

// display one frame of breathing animation
void breathing(uint16_t wait)
{
	// fill frame buffers with rows
	for(int row = 0; row < rows; row++)
	{
		setRow(row, colorTable[indexer++]);
		if (indexer > 16)
			indexer = 0;
	}
	strip.show(); // paint frame
	delay(wait); // wait
}

// fill the dots one after the other with a color
void colorWipe(uint32_t color, uint8_t wait)
{
	for(uint16_t i = 0; i < strip.numPixels(); i++)
	{
		strip.setPixelColor(i, color);
		strip.show();
		delay(wait);
	}
}

// return gamma-corrected color value
uint32_t gamma(uint8_t red, uint8_t green, uint8_t blue)
{
	return strip.Color(pgm_read_byte(&gammaTable[red]), pgm_read_byte(&gammaTable[green]), pgm_read_byte(&gammaTable[blue]));
}

// set given row to given color
void setRow(uint8_t row, uint32_t color)
{
	// loop through pixels in row
	for (uint8_t pixel = 0; pixel < columns; pixel++)
	{
		strip.setPixelColor(row*columns+pixel, color);
	}
}

// generate color sin wave with amplitude, phase, and offset
uint8_t sinWave(uint8_t row, uint8_t amplitude, uint8_t phi, int8_t offset, int8_t skew)
{
	return ((uint8_t) (amplitude/2)*sin(phi-(M_2PI/(rows+skew))*row)) + offset;
}

// gamma correction table
const uint8_t PROGMEM gammaTable[] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
  };
