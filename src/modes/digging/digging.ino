// include statements
// include statements
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
	#include <avr/power.h>
#endif

// definitions
#define BGND_RED 0
#define BGND_GREEN 0
#define BGND_BLUE 4
#define PIN 6
#define PIXELS 256

// global variables
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);
const uint8_t columns = 16;
const uint8_t rows = 16;
extern const uint8_t gammaTable[]; // gamma correction table, in ROM
const uint8_t* image; // pointer to select item to display
// items in ROM
extern const unsigned char clay[];
extern const unsigned char coal_ore[];
extern const unsigned char diamond_ore[];
extern const unsigned char dirt[];
extern const unsigned char emerald_ore[];
extern const unsigned char gold_ore[];
extern const unsigned char gravel[];
extern const unsigned char iron_ore[];
extern const unsigned char lapis_ore[];
extern const unsigned char obsidian[];
extern const unsigned char redstone_ore[];
extern const unsigned char sand[];
extern const unsigned char sandstone[];
extern const unsigned char stone[];

// initialization
void setup()
{
	strip.begin();
	strip.show(); // initialize all pixels to 'off'
	/* power-on self test */
	// colorWipe(strip.Color(16, 16, 16), 1); // white
}

// main
void loop()
{
	// display item
	image = clay;
	load();
	strip.show();
	delay(1000);
	// display item
	image = coal_ore;
	load();
	strip.show();
	delay(1000);
	// display item
	image = diamond_ore;
	load();
	strip.show();
	delay(1000);
	// display item
	image = dirt;
	load();
	strip.show();
	delay(1000);
	// display item
	image = emerald_ore;
	load();
	strip.show();
	delay(1000);
	// display item
	image = gold_ore;
	load();
	strip.show();
	delay(600);
	// display item
	image = gravel;
	load();
	strip.show();
	delay(1000);
	// display item
	image = iron_ore;
	load();
	strip.show();
	delay(1000);
	// display item
	image = lapis_ore;
	load();
	strip.show();
	delay(1000);
	// display item
	image = obsidian;
	load();
	strip.show();
	delay(1000);
	// display item
	image = redstone_ore;
	load();
	strip.show();
	delay(1000);
	// display item
	image = sand;
	load();
	strip.show();
	delay(1000);
	// display item
	image = sandstone;
	load();
	strip.show();
	delay(1000);
	// display item
	image = stone;
	load();
	strip.show();
	delay(1000);
}
/*
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
*/
// return gamma-corrected color value
uint32_t gamma(uint8_t red, uint8_t green, uint8_t blue)
{
	return strip.Color(pgm_read_byte(&gammaTable[red]), pgm_read_byte(&gammaTable[green]), pgm_read_byte(&gammaTable[blue]));
}

// load an item from ROM to frame buffer in RAM
void load()
{
	uint16_t byte = 0;
	for (int row = 0; row < rows; row++)
	{
		if (row % 2 == 1) // odd row, scan forward
		{
			for (int column = 0; column < columns; column++)
			{
				uint8_t red = pgm_read_byte(&image[byte++]);
				uint8_t green = pgm_read_byte(&image[byte++]);
				uint8_t blue = pgm_read_byte(&image[byte++]);
				if (red || green || blue)
					strip.setPixelColor((row*columns) + column, gamma(red, green, blue));
				else
					strip.setPixelColor((row*columns) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
			}
		}
		else // even row, scan backwards
		{
			for (int column = columns-1; column > -1; column--)
			{
				uint8_t red = pgm_read_byte(&image[byte++]);
				uint8_t green = pgm_read_byte(&image[byte++]);
				uint8_t blue = pgm_read_byte(&image[byte++]);
				if (red || green || blue)
					strip.setPixelColor((row*columns) + column, gamma(red, green, blue));
				else
					strip.setPixelColor((row*columns) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
			}
		}
	}
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