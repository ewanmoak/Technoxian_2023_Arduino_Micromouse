#ifndef menu_h
#define menu_h

#define encoderStepsMenu 30
#define I2C_ADDRESS 0x3C

#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>

extern SSD1306AsciiAvrI2c oled;

void oledSetup(void);
void updateEncoder(void);
void displayMenu(void);

#endif