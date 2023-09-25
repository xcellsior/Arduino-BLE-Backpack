// include statements
// include statements
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
	#include <avr/power.h>
#endif

// definitions
#define BGND_RED 0
#define BGND_GREEN 0
#define BGND_BLUE 0
#define PIN 6
#define PIXELS 512

// global variables
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);
const uint8_t columns = 16;
const uint8_t rows = 32;
extern const uint8_t gammaTable[]; // gamma correction table, in ROM
const uint8_t* image; // pointer to select item to display
// items in ROM
extern const unsigned char mario1[];
extern const unsigned char mario2[];
extern const unsigned char marioStanding[];
extern const unsigned char malleo[];
extern const unsigned char malleo2[];
extern const unsigned char malleo3[];
extern const unsigned char forest2[];
extern const unsigned char rocketship[];
extern const unsigned char flower[];
extern const unsigned char offbrandpikachu[];
extern const unsigned char pokemon[];
extern const unsigned char rainbow[];
extern const unsigned char unicornx2[];

int delayval = 100;

// initialization
void setup()
{
  Serial.begin(9600);
	strip.begin();
  strip.setBrightness(25);
	strip.show(); // initialize all pixels to 'off'
	/* power-on self test */
	// colorWipe(strip.Color(16, 16, 16), 1); // white
}

// main
void loop()
{

  for (int i = 0; i < 20; i++) {
    image = mario1;
  load();
  strip.show();
  delay(100);

      image = mario2;
  load();
  strip.show();
  delay(100);

      image = marioStanding;
  load();
  strip.show();
  delay(100);
  }
	
	// display item
	image = rocketship;
	load();
	strip.show();
	delay(1000);

    image = flower;
  load();
  strip.show();
  delay(1000);

    image = offbrandpikachu;
  load();
  strip.show();
  delay(1000);

    image = pokemon;
  load();
  strip.show();
  delay(1000);

    image = rainbow;
  load();
  strip.show();
  delay(1000);
      image = unicornx2;
  load();
  strip.show();
  delay(1000);
  
	// display item
//	image = malleo2;
//	load();
//	strip.show();
//	delay(100);
//	// display item
//	image = malleo3;
//	load();
//	strip.show();
//	delay(100);
	// display item
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
          strip.setPixelColor((row*columns) + column, strip.Color(red, green, blue));
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
          strip.setPixelColor((row*columns) + column, strip.Color(red, green, blue));
        else
          strip.setPixelColor((row*columns) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
      }
    }
  }
}
// load an item from ROM to frame buffer in RAM
void load2()
{
	uint16_t byte = 23; // start at 8 for 16 pix columns
	for (int row = 0; row < rows; row++)
	{
		if (row % 2 == 1) { // odd row, scan forward
			for (int column = 0; column < (columns / 2 ); column++)
			{
        byte++;
				uint8_t red = pgm_read_byte(&image[byte++]);
				uint8_t green = pgm_read_byte(&image[byte++]);
				uint8_t blue = pgm_read_byte(&image[byte++]);
				if (red || green || blue)
					strip.setPixelColor((row*(columns/2)) + column, strip.Color(red, green, blue, 1));
				else
					strip.setPixelColor((row*(columns/2)) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
        byte--;
			}
		}
		else { // even row, scan backwards
			for (int column = 0; column < columns /2; column++)
			{
				uint8_t blue = pgm_read_byte(&image[byte--]);
				uint8_t green = pgm_read_byte(&image[byte--]);
				uint8_t red = pgm_read_byte(&image[byte--]);
				if (red || green || blue)
					strip.setPixelColor((row*(columns/2)) + column, strip.Color(red, green, blue, 1));
				else
					strip.setPixelColor((row*(columns/2)) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
			}
		}
		byte += 16 * 3;
	}
  byte = 3 * 512 - 24;
  for (int row = 0; row < 32; row++){
    if (row %2 == 0) // even row, bottom up, forward scan
    {
      for (int column = 0; column < columns/2; column++){
        uint8_t red = pgm_read_byte(&image[byte++]);
        uint8_t green = pgm_read_byte(&image[byte++]);
        uint8_t blue = pgm_read_byte(&image[byte++]);
        if (red || green || blue)
          strip.setPixelColor(256 + (row*(columns/2)) + column, strip.Color(red, green, blue, 1));
        else
          strip.setPixelColor(256 + (row*(columns/2)) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
      }
    }
    else { // odd
      for (int column = 0; column < columns/2; column++){
        byte--;
        uint8_t blue = pgm_read_byte(&image[byte--]);
        uint8_t green = pgm_read_byte(&image[byte--]);
        uint8_t red = pgm_read_byte(&image[byte--]);
        if (red || green || blue)
          strip.setPixelColor(256 + (row*(columns/2)) + column, strip.Color(red, green, blue, 1));
        else
          strip.setPixelColor(256 + (row*(columns/2)) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
        byte++;
      }
    }
    byte -= 16 * 3;
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
