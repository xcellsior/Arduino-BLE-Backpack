# Arduino-BLE-Backpack
A BLE-enabled LED matrix controller attached to a backpack with custom modes

![](https://github.com/xcellsior/Arduino-BLE-Backpack/blob/main/images/backpack.gif)

The idea behind this project is to create a matrix of LEDs that can be controlled from 
your smartphone via an Arduino. Specifically, this project uses a Bluetooth terminal application 
on smartphones to send UART messages to an Arduino. The Arduino then processes the UART 
message and applies the appropriate action. There are multiple actions that the Arduino can 
perform on the LED matrix. These include mathematically generated wave functions (like a 
rainbow that slowly moves across the LED matrix), or custom loaded 16x16 images that have 
been loaded into ROM. The flexible LED matrix can be mounted on a backpack and is powered 
through JST by a battery pack.


# System Architecture and Hardware Design

The application on the smartphone is any bluetooth terminal app. This communicates with the Adafruit Bluefruit Bluetooth LE Arduino Shield. 
The Arduino receives communication through the Bluefruit and acts according to the data received. The Arduino sends the relevant data to the LED matrix,
one pixel at a time. The Arduino and LED matrix are powered by a battery via USB.

The hardware includes: LED Neopixel matrix, an embedded system (Arduino), and an android device. The LED matrix consists of 256 individually addressable 
neopixels wired serially. Each neopixel utilizes the WS2812B integrated circuit underneath to drive the colors. It supports 24 bit color depth.
Each neopixel has four pins; two pins carry 5-volt power to drive the LED, the2222 other two pins are data-in and data-out for single wire communication bus.


It is critical to understand addressing system of the neopixel. The first neopixel in the string collects the first 24-bits sent by the Arduino, representing 8-bit values for green,
 red and blue in that order. Once it has collected 24-bits, it passes all subsequent bits along to the next neopixel. That pixel collects 24 bits for itself,
 then passes all remaining bits to the next pixel in the series. This is how to address each pixel using a single wire bus. Once the cycle has completed and 
 all pixels have been programmed, hold the line low for 50 microseconds to tell all WS2812Bs that the cycle is done and they should watch for their next set of bits.
 
 The data line between the neopixel string and pin 6 of our Arduino Uno is buffered using a 470-ohm resistor. This protects the first WS2812B in the chain from abrupt changes in pin voltage.
 The 5-volt power supply to the LED matrix is buffered using a 1 millifarad capacitor. This protects the power supply from sudden and dramatic changes in current consumption by the LEDs.
 256 LEDs at full brightness can draw almost three amperes, so switching from zero to 255 brightness in a tenth of a millisecond is stressful on most power supplies.
 This custom circuitry is mounted on free space available on the Bluefruit module.
 
 ![](https://github.com/xcellsior/Arduino-BLE-Backpack/blob/main/images/arduino.png)
 
 A three pin JST connector is used to connect this power circuitry.  The prototype backpack used a JST to USB adapter to supply power from a battery bank normally used to charge mobile devices.
 
 The Bluetooth network interface card is an Adafruit Bluefruit LE module. It communicates with the Arduino Uno using SPI.
 
# Software and User Controls

First, initialize the color table and initialize the neopixels and set them to ‘off’. The Bluetooth module is set to data mode.
 Next, in the main loop, it waits for a connection. When there is a connection, it waits to receive a UART message. It then runs the module based on the input message.
 The rainbow module is a mathematical sine function that prints 17 rows to the color table, giving it the appearance of motion. For still images or gif animations, it loads their byte values for each RGB color from ROM into the buffer in RAM.
 
 Available modes (from forest22.ino):
 
 'q' - quit to Bluetooth connect page
 
 'm' - run Mario
 
 'L' - run Malleo
 
 'r' - run rainbow
 
 'f' - run Forgotten Runes items
 
 't' - run rainbow theater chase
 
 'c' - chaos mode
 
 'd' - minecraft diamonds
 
 After connecting to the device with a Bluetooth terminal, send these characters to change the mode.