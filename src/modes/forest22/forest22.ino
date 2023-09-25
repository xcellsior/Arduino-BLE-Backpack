// include statements
// include statements
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
	#include <avr/power.h>
#endif
// BLE 
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

// definitions
#define BGND_RED 0
#define BGND_GREEN 0
#define BGND_BLUE 4
#define PIN 6
#define PIXELS 512
#define M_2PI   6.283185307179586476925286766559005768394338798750
#define M_1_2PI 0.159154943091895335768883763372514362034459645740
#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.6.6"
#define MODE_LED_BEHAVIOUR "MODE"

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
extern const unsigned char forest[];
extern const unsigned char forest2[];
extern const unsigned char rocketship[];
extern const unsigned char flower[];
extern const unsigned char offbrandpikachu[];
extern const unsigned char pokemon[];
extern const unsigned char rainbow[];
extern const unsigned char unicornx2[];
extern const unsigned char bluetoothConnect[];
extern const unsigned char hummingbird[];
extern const unsigned char diamond[];
extern const unsigned char diamond_axe[];
extern const unsigned char diamond_hoe[];
extern const unsigned char diamond_pickaxe[];
extern const unsigned char diamond_shovel[];
extern const unsigned char diamond_sword[];
extern const unsigned char chroma[];
extern const unsigned char ale[];


int delayval = 100;
uint8_t index1 = 0;
int charRead, charRead2;
uint32_t colorTable[rows+1];
long randNumber;

// Struct containing peripheral info
typedef struct
{
  char name[16+1];

  uint16_t conn_handle;

  // Each prph need its own bleuart client service
  BLEClientUart bleuart;
} prph_info_t;
prph_info_t prphs[BLE_MAX_CONNECTION];
uint8_t connection_num = 0; // for blink pattern

// initialization
void setup()
{
  randomSeed(analogRead(0));
  for (int row = 0; row < rows+1; row++)
  {
    colorTable[row] = gamma(0, sinWave(row, 128, 0, 128, 1), sinWave(row, 128, 90, 128, 1));
  }
  Serial.begin(115200);

  #if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif
  
  Serial.println("Bluefruit52 BLEUART Example");
  Serial.println("---------------------------\n");

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setName("Da BaqPaq");
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();

  
	strip.begin();
  strip.setBrightness(65);
	strip.show(); // initialize all pixels to 'off'
  clearBoard();
	/* power-on self test */
	// colorWipe(strip.Color(16, 16, 16), 1); // white
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}
/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
// main
void loop()
{
//  if ( Bluefruit.Central.connected() ){
//        // default MTU with an extra byte for string terminator
//    char buf[20+1] = { 0 };
//    
//    // Read from HW Serial (normally USB Serial) and send to all peripherals
//    if ( Serial.readBytes(buf, sizeof(buf)-1) )
//    {
//      sendAll(buf);
//    }
//    int command = bleuart.read();
//    switch (command) {
//      case 'm': // run Mario
//      case 'M':
//      {
//        runMario();
//        break;
//      }
//      case 'b': // run breathing
//      case 'B':
//      {
//        //runBreathing(128);
//        rainbowCycle();
//        break;
//      }
//    }
//  }

  if (!Bluefruit.Central.connected())
  {
    image = bluetoothConnect;
    load();
    strip.show();
  }
  else
    clearBoard();
  while(bleuart.available()){
    //int command = bleuart.read();
    uint8_t ch;
    ch = (uint8_t) bleuart.read();
    Serial.write(ch);
    switch (ch) {
      case 'm': // run Mario
      case 'M':
      {
        runMario();
        break;
      }
      case 'l': // run Mario
      case 'L':
      {
        runMalleo();
        break;
      }
//      case 'b': // run breathing
//      case 'B':
//      {
//        int idx = 0;
//        while(1){
//          char brightness[2];
//          charRead = bleuart.read();
//          brightness[idx++] = charRead; 
//
//          if (idx > 1){
//            idx = 0;
//            // set brightness
//            uint32_t brightnessInt = brightness[0];
//            brightnessInt |= brightness[0] << 24;
//            brightnessInt |= brightness[1] << 16;
//            Serial.write(brightnessInt);
//            strip.setBrightness(brightnessInt);
//            strip.show();
//          }
//          if ((char)charRead == 'q' || (char)charRead == 'Q')
//          break;
//        }
//        
//        //runBreathing(128);
//        rainbowCycle();
//        break;
//      }
     case '1':
     {
      strip.setBrightness(10);
      strip.show();
      break;
     }

     case '2':
     {
      strip.setBrightness(20);
      strip.show();
      break;
     }
          case '3':
     {
      strip.setBrightness(30);
      strip.show();
      break;
     }
     case '4':
     {
      strip.setBrightness(40);
      strip.show();
      break;
     }

     case '5':
     {
      strip.setBrightness(50);
      strip.show();
      break;
     }
          case '6':
     {
      strip.setBrightness(60);
      strip.show();
      break;
     }
     case '7':
     {
      strip.setBrightness(70);
      strip.show();
      break;
     }

     case '8':
     {
      strip.setBrightness(80);
      strip.show();
      break;
     }
          case '9':
     {
      strip.setBrightness(90);
      strip.show();
      break;
     }
     case 'r':
     {
      rainbowCycle();
      break;
     }
     case 'f':
     {
      frwc();
      break;
     }
     case 't':
     {
      theaterChaseRainbow();
      break;
     }
      case 'q':
      case 'Q':
      {
        clearBoard();
        break;
      }
      case 'c':
      case 'C':
      {
        chaos();
        break;
      }
      case 'd':
      case 'D':
      {
        minecraftDiamonds();
        break;
      }
    }
  }
	
	// display item
// for (int i = 0; i < 20; i++) {
//    image = mario1;
//  load();
//  strip.show();
//  delay(100);
//
//      image = mario2;
//  load();
//  strip.show();
//  delay(100);
//
//      image = marioStanding;
//  load();
//  strip.show();
//  delay(100);
//  }

}

//pick a random thing to display
void chaos(){
  while(1){
  randNumber = random(15);
  int timer = 1500;
  switch(randNumber){
    case 1:{
      image = flower;
      lsdg(timer);
      break;
    }
    case 2:{
      image = marioStanding;
      lsd(timer);
      break;
    }
    case 3:{
      image = offbrandpikachu;
      lsdg(timer);
      break;
    }
    case 4:{
      image = rocketship;
      lsdg(timer);
      break;
    }
    case 5:{
      image = forest2;
      load();
      strip.show();
      delay(timer);
      break;
    }
    case 6:{
      image = forest;
      load();
      strip.show();
      delay(timer);
      break;
    }
    case 7:{
      image = pokemon;
      load();
      strip.show();
      delay(timer);
      break;
    }
    case 8:{
      image = rainbow;
      lsdg(timer);
      break;
    }
    case 9:{
      image = unicornx2;
      lsdg(timer);
      break;
    }
    case 10:{
      image = hummingbird;
      lsdg(timer);
      break;
    }
    case 11:{
      image = diamond_axe;
      load();
      strip.show();

      delay(timer);
      break;
    }
    case 12:{
      image = diamond_pickaxe;
      load();
      strip.show();

      delay(timer);
      break;
    }
    case 13:{
      image = diamond_sword;
      load();
      strip.show();

      delay(timer);
      break;
    }
    case 14:{
      image = ale;
      lsdg(timer);
      break;
    }
    case 15:{
      image = chroma;
      lsdg(timer);
      break;
    }
//    case 16:{
//      image = flower;
//      load();
//      strip.show();
//
//      delay(timer);
//      break;
//    }
//    case 17:{
//      image = flower;
//      load();
//      strip.show();
//
//      delay(timer);
//      break;
//    }
//    case 18:{
//      image = flower;
//      load();
//      strip.show();
//
//      delay(timer);
//      break;
//    }
  }
  charRead = bleuart.read();
    if ((char)charRead == 'q' || (char)charRead == 'Q')
        break;
  }
}

// Load Strip Delay
void lsd(int timer){
    load();
    strip.show();
    delay(timer);
}

// Load Strip Delay w/ Gamma
void lsdg(int timer){
    lwg();
    strip.show();
    delay(timer);
}
void frwc(){
  while(1){
    image = hummingbird;
    lsdg(1000);
    image = chroma;
    lsdg(1000);
    image = ale;
    lsdg(1000);
    
    charRead = bleuart.read();
    if ((char)charRead == 'q' || (char)charRead == 'Q')
        break;
  }
}
// moving rainbow
void rainbowCycle()
{
  uint16_t i, j;
  while(true)
  {
    for(j = 0; j < 256*5; j++) // 5 cycles of all colors on wheel
    {
      for(i=0; i< strip.numPixels(); i++)
      {
        strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      delay(20);
      charRead = bleuart.read();
      if ((char)charRead == 'q' || (char)charRead == 'Q')
        return;
    }
  }
}

void clearBoard()
{
  for(uint16_t i=0; i<PIXELS; i++) 
  {
    strip.setPixelColor(i, strip.Color(0,0,0)); // off
  }
  strip.show();
}

void minecraftDiamonds()
{
  while(true)
  {
    // display item
    image = diamond;
    load();
    strip.show();
    delay(1000);
    // display item
    image = diamond_axe;
    load();
    strip.show();
    delay(1000);
    // display item
    image = diamond_hoe;
    load();
    strip.show();
    delay(1000);
    // display item
    image = diamond_pickaxe;
    load();
    strip.show();
    delay(1000);
    // display item
    image = diamond_shovel;
    load();
    strip.show();
    delay(1000);
    // display item
    image = diamond_sword;
    load();
    strip.show();
    delay(1000);
    charRead = bleuart.read();
    if ((char)charRead == 'q' || (char)charRead == 'Q')
      break;
  }
}
/**
 * Helper function to send a string to all connected peripherals
 */
void sendAll(const char* str)
{
  Serial.print("[Send to All]: ");
  Serial.println(str);
  
  for(uint8_t id=0; id < BLE_MAX_CONNECTION; id++)
  {
    prph_info_t* peer = &prphs[id];

    if ( peer->bleuart.discovered() )
    {
      peer->bleuart.print(str);
    }
  }
}
// input a value 0 to 255 to get a color value (transition r - g - b - back to r)
uint32_t wheel(byte position)
{
  position = 255 - position;
  if(position < 85)
    return strip.Color(255 - position * 3, 0, position * 3);
  if(position < 170)
  {
    position -= 85;
    return strip.Color(0, position * 3, 255 - position * 3);
  }
  position -= 170;
  return strip.Color(position * 3, 255 - position * 3, 0);
}


uint32_t gamma(uint8_t red, uint8_t green, uint8_t blue)
{
  return strip.Color(pgm_read_byte(&gammaTable[red]), pgm_read_byte(&gammaTable[green]), pgm_read_byte(&gammaTable[blue]));
}
uint8_t sinWave(uint8_t row, uint8_t amplitude, uint8_t phi, int8_t offset, int8_t skew)
{
  return ((uint8_t) (amplitude/2)*sin(phi-(M_2PI/(rows+skew))*row)) + offset;
}
void runBreathing(uint16_t wait)
{
  // fill frame buffers with rows
  while(true)
  {
    for(int row = 0; row < rows; row++)
    {
      setRow(row, colorTable[index1++]);
      if (index1 > 16)
        index1 = 0;
    }
    strip.show(); // paint frame
    delay(wait); // wait
    charRead = bleuart.read();
    if ((char)charRead == 'q' || (char)charRead == 'Q')
      break;
  }
}
void runMario()
{
  while(true)
  { 
    // display item
    image = mario1;
    load();
    strip.show();
    delay(100);
    // display item
    image = mario2;
    load();
    strip.show();
    delay(100);
    // display item
    image = marioStanding;
    load();
    strip.show();
    delay(100);
    // get char from bluetooth
    charRead = bleuart.read(); //this line breaks the loop
    if ((char)charRead == 'q' || (char)charRead == 'Q')
      break;
  }
}
void runMalleo()
{
  while(true)
  { 
    // display item
    image = malleo;
    load();
    strip.show();
    delay(100);
    // display item
    image = malleo2;
    load();
    strip.show();
    delay(100);
    // display item
    image = malleo3;
    load();
    strip.show();
    delay(100);
    // get char from bluetooth
    charRead = bleuart.read(); //this line breaks the loop
    if ((char)charRead == 'q' || (char)charRead == 'Q')
      break;
  }
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
				if (red || green || blue){
					strip.setPixelColor((row*columns) + column, strip.Color(red, green, blue));
				}
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
//load with gamma correction
void lwg()
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
        if (red || green || blue){
          strip.setPixelColor((row*columns) + column, gamma(red,green,blue));
        }
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
          strip.setPixelColor((row*columns) + column, gamma(red,green,blue));
        else
          strip.setPixelColor((row*columns) + column, strip.Color(BGND_RED, BGND_GREEN, BGND_BLUE));
      }
    }
  }
}

void theaterChaseRainbow()
{
  while(true)
  {
    for (int j = 0; j < 256; j++) // cycle all 256 colors in the wheel
    {
      for (int q = 0; q < 3; q++)
      {
        for (uint16_t i = 0; i < strip.numPixels(); i += 3)
        {
          strip.setPixelColor(i+q, wheel((i+j) % 255)); //turn every third pixel on
        }
        strip.show();
        delay(50);
        for (uint16_t i = 0; i < strip.numPixels(); i += 3)
        {
          strip.setPixelColor(i+q, 0); //turn every third pixel off
        }
      }
      charRead = bleuart.read();
      if ((char)charRead == 'q' || (char)charRead == 'Q')
        return;
    }
  }
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
/**
 * Find the connection handle in the peripheral array
 * @param conn_handle Connection handle
 * @return array index if found, otherwise -1
 */
int findConnHandle(uint16_t conn_handle)
{
  for(int id=0; id<BLE_MAX_CONNECTION; id++)
  {
    if (conn_handle == prphs[id].conn_handle)
    {
      return id;
    }
  }

  return -1;  
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
