// Program to exercise the MD_MAX72XX library
// Uses most of the functions in the library
#include <MD_MAX72xx.h>

// Turn on debug statements to the serial output
#define DEBUG 1

#if DEBUG
#define PRINT(s, x) \
  { \
    Serial.print(F(s)); \
    Serial.print(x); \
  }
#define PRINTS(x) Serial.print(F(x))
#define PRINTD(x) Serial.println(x, DEC)

#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)

#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN 13   // or SCK
#define DATA_PIN 11  // or MOSI
#define CS_PIN 10    // or SS

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Specific SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, SPI1, CS_PIN, MAX_DEVICES);
// Arbitrary pins
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// We always wait a bit between updates of the display
#define DELAYTIME 80  // in milliseconds




void scrollText(const char *p) {
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts

  PRINTS("\nScrolling text");
  mx.clear();

  while (*p != '\0') {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

    for (uint8_t i = 0; i <= charWidth; i++)  // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      mx.setColumn(0, (i < charWidth) ? cBuf[i] : 0);
      delay(DELAYTIME);
    }
  }
}



void rows()
// Demonstrates the use of setRow()
{
  PRINTS("\nRows 0->7");
  mx.clear();

  for (uint8_t row = 0; row < ROW_SIZE; row++) {
    mx.setRow(row, 0xff);
    delay(2 * DELAYTIME);
    mx.setRow(row, 0x00);
  }
}



void columns()
// Demonstrates the use of setColumn()
{
  PRINTS("\nCols 0->max");
  mx.clear();

  for (uint8_t col = 0; col < mx.getColumnCount(); col++) {
    mx.setColumn(col, 0xff);
    delay(DELAYTIME / MAX_DEVICES);
    mx.setColumn(col, 0x00);
  }
}

void cross()
// Combination of setRow() and setColumn() with user controlled
// display updates to ensure concurrent changes.
{
  PRINTS("\nMoving cross");
  mx.clear();
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // diagonally down the display R to L
  for (uint8_t i = 0; i < ROW_SIZE; i++) {
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setColumn(j, i, 0xff);
      mx.setRow(j, i, 0xff);
    }
    mx.update();
    delay(DELAYTIME);
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setColumn(j, i, 0x00);
      mx.setRow(j, i, 0x00);
    }
  }

  // moving up the display on the R
  for (int8_t i = ROW_SIZE - 1; i >= 0; i--) {
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setColumn(j, i, 0xff);
      mx.setRow(j, ROW_SIZE - 1, 0xff);
    }
    mx.update();
    delay(DELAYTIME);
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setColumn(j, i, 0x00);
      mx.setRow(j, ROW_SIZE - 1, 0x00);
    }
  }

  // diagonally up the display L to R
  for (uint8_t i = 0; i < ROW_SIZE; i++) {
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setColumn(j, i, 0xff);
      mx.setRow(j, ROW_SIZE - 1 - i, 0xff);
    }
    mx.update();
    delay(DELAYTIME);
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setColumn(j, i, 0x00);
      mx.setRow(j, ROW_SIZE - 1 - i, 0x00);
    }
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void bullseye()
// Demonstrate the use of buffer based repeated patterns
// across all devices.
{
  PRINTS("\nBullseye");
  mx.clear();
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  for (uint8_t n = 0; n < 3; n++) {
    byte b = 0xff;
    int i = 0;

    while (b != 0x00) {
      for (uint8_t j = 0; j < MAX_DEVICES + 1; j++) {
        mx.setRow(j, i, b);
        mx.setColumn(j, i, b);
        mx.setRow(j, ROW_SIZE - 1 - i, b);
        mx.setColumn(j, COL_SIZE - 1 - i, b);
      }
      mx.update();
      delay(3 * DELAYTIME);
      for (uint8_t j = 0; j < MAX_DEVICES + 1; j++) {
        mx.setRow(j, i, 0);
        mx.setColumn(j, i, 0);
        mx.setRow(j, ROW_SIZE - 1 - i, 0);
        mx.setColumn(j, COL_SIZE - 1 - i, 0);
      }

      bitClear(b, i);
      bitClear(b, 7 - i);
      i++;
    }

    while (b != 0xff) {
      for (uint8_t j = 0; j < MAX_DEVICES + 1; j++) {
        mx.setRow(j, i, b);
        mx.setColumn(j, i, b);
        mx.setRow(j, ROW_SIZE - 1 - i, b);
        mx.setColumn(j, COL_SIZE - 1 - i, b);
      }
      mx.update();
      delay(3 * DELAYTIME);
      for (uint8_t j = 0; j < MAX_DEVICES + 1; j++) {
        mx.setRow(j, i, 0);
        mx.setColumn(j, i, 0);
        mx.setRow(j, ROW_SIZE - 1 - i, 0);
        mx.setColumn(j, COL_SIZE - 1 - i, 0);
      }

      i--;
      bitSet(b, i);
      bitSet(b, 7 - i);
    }
  }

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void stripe()
// Demonstrates animation of a diagonal stripe moving across the display
// with points plotted outside the display region ignored.
{
  const uint16_t maxCol = MAX_DEVICES * ROW_SIZE;
  const uint8_t stripeWidth = 10;

  PRINTS("\nEach individually by row then col");
  mx.clear();

  for (uint16_t col = 0; col < maxCol + ROW_SIZE + stripeWidth; col++) {
    for (uint8_t row = 0; row < ROW_SIZE; row++) {
      mx.setPoint(row, col - row, true);
      mx.setPoint(row, col - row - stripeWidth, false);
    }
    delay(DELAYTIME);
  }
}



void bounce()
// Animation of a bouncing ball
{
  const int minC = 0;
  const int maxC = mx.getColumnCount() - 1;
  const int minR = 0;
  const int maxR = ROW_SIZE - 1;

  int nCounter = 0;

  int r = 0, c = 2;
  int8_t dR = 1, dC = 1;  // delta row and column

  PRINTS("\nBouncing ball");
  mx.clear();

  while (nCounter++ < 200) {
    mx.setPoint(r, c, false);
    r += dR;
    c += dC;
    mx.setPoint(r, c, true);
    delay(DELAYTIME);

    if ((r == minR) || (r == maxR))
      dR = -dR;
    if ((c == minC) || (c == maxC))
      dC = -dC;
  }
}

void setup() {
#if DEBUG
  Serial.begin(57600);
#endif
  PRINTS("\n[MD_MAX72XX Test & Demo]");

  if (!mx.begin())
    PRINTS("\nMD_MAX72XX initialization failed");
}

void loop() {


  // Matrix animations
  scrollText("    I MAKE COOL SHIT.               ");
  rows();
  columns();
  cross();
  stripe();
  bullseye();
  bounce();

  delay(100);  // Slightly longer delay for visibility
}