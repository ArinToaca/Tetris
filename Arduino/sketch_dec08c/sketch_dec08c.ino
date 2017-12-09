#include <SPI.h>
#include "aia-tft.h"

Ucglib_ST7735_18x128x128_HWSPI ucg(/*cd=*/ 9 , /*cs=*/ 10, /*reset=*/8);

void setup(void)
{
  delay(1000);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
}

void loop(void)
{
  ucg.setRotate90();
  ucg.setFont(ucg_font_ncenR14_tr);
  ucg.setPrintPos(0,25);
  ucg.setColor(255, 255, 255);
  ucg.print("Hello World!");
  delay(500);
}
