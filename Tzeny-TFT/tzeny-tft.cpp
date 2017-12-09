 #include "tzeny-tft.h"
 #include <limits.h>
 #include "pins_arduino.h"
 #include "wiring_private.h"
 #include <SPI.h>
 #include "glcdfont.c"

 #define ST7735_TFTWIDTH  128
 #define ST7735_TFTHEIGHT 128

 inline uint16_t swapcolor(uint16_t x) { 
   return (x << 11) | (x & 0x07E0) | (x >> 11);
 }
 
 #if defined (SPI_HAS_TRANSACTION)
   static SPISettings mySPISettings;
 #elif defined (__AVR__) || defined(CORE_TEENSY)
   static uint8_t SPCRbackup;
   static uint8_t mySPCR;
 #endif
 
#ifndef pgm_read_byte
    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
    #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
    #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif 
 
#ifndef _swap_int16_t
    #define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


 tzeny_tft::tzeny_tft(int8_t cs, int8_t dc, int8_t rst) {
    _width = ST7735_TFTWIDTH;
    _height = ST7735_TFTHEIGHT;
    _cs   = cs;
    _dc   = dc;
    _rst  = rst;
    hwSPI = true;
    _sid  = _sclk = -1;
    _cp437    = false;
 }
 
 inline void tzeny_tft::spiwrite(uint8_t c) {
 
    if (hwSPI) {
    #if defined (SPI_HAS_TRANSACTION)
        SPI.transfer(c);
    #elif defined (__AVR__) || defined(CORE_TEENSY)
        SPCRbackup = SPCR;
        SPCR = mySPCR;
        SPI.transfer(c);
        SPCR = SPCRbackup;
    #elif defined (__arm__)
        SPI.setClockDivider(21); //4MHz
        SPI.setDataMode(SPI_MODE0);
        SPI.transfer(c);
    #endif
    } else 
    {
 
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
    #if defined(USE_FAST_IO)
        if(c & bit) *dataport |=  datapinmask;
        else        *dataport &= ~datapinmask;
        *clkport |=  clkpinmask;
        *clkport &= ~clkpinmask;
    #else
        if(c & bit) digitalWrite(_sid, HIGH);
        else        digitalWrite(_sid, LOW);
        digitalWrite(_sclk, HIGH);
        digitalWrite(_sclk, LOW);
    #endif
        }
    }
 }
 
 void tzeny_tft::writecommand(uint8_t c) {
    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)    SPI.beginTransaction(mySPISettings);
    #endif
    DC_LOW();
    CS_LOW();

    spiwrite(c);

    CS_HIGH();
    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)    SPI.endTransaction();
    #endif
 }
 
 void tzeny_tft::writedata(uint8_t c) {
    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)    SPI.beginTransaction(mySPISettings);
    #endif
    DC_HIGH();
    CS_LOW();
        
    spiwrite(c);

    CS_HIGH();
    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)    SPI.endTransaction();
    #endif
 }
 
 // Rather than a bazillion writecommand() and writedata() calls, screen
 // initialization commands and arguments are organized in these tables
 // stored in PROGMEM.  The table may look bulky, but that's mostly the
 // formatting -- storage-wise this is hundreds of bytes more compact
 // than the equivalent code.  Companion function follows.
 #define DELAY 0x80
 static const uint8_t PROGMEM
   Bcmd[] = {                  // Initialization commands for 7735B screens
     18,                       // 18 commands in list:
     ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
       50,                     //     50 ms delay
     ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
       255,                    //     255 = 500 ms delay
     ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
       0x05,                   //     16-bit color
       10,                     //     10 ms delay
     ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
       0x00,                   //     fastest refresh
       0x06,                   //     6 lines front porch
       0x03,                   //     3 lines back porch
       10,                     //     10 ms delay
     ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
       0x08,                   //     Row addr/col addr, bottom to top refresh
     ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
       0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                               //     rise, 3 cycle osc equalize
       0x02,                   //     Fix on VTL
     ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
       0x0,                    //     Line inversion
     ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
       0x02,                   //     GVDD = 4.7V
       0x70,                   //     1.0uA
       10,                     //     10 ms delay
     ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
       0x05,                   //     VGH = 14.7V, VGL = -7.35V
     ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
       0x01,                   //     Opamp current small
       0x02,                   //     Boost frequency
     ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
       0x3C,                   //     VCOMH = 4V
       0x38,                   //     VCOML = -1.1V
       10,                     //     10 ms delay
     ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
       0x11, 0x15,
     ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
       0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
       0x21, 0x1B, 0x13, 0x19, //      these config values represent)
       0x17, 0x15, 0x1E, 0x2B,
       0x04, 0x05, 0x02, 0x0E,
     ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
       0x0B, 0x14, 0x08, 0x1E, //     (ditto)
       0x22, 0x1D, 0x18, 0x1E,
       0x1B, 0x1A, 0x24, 0x2B,
       0x06, 0x06, 0x02, 0x0F,
       10,                     //     10 ms delay
     ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
       0x00, 0x02,             //     XSTART = 2
       0x00, 0x81,             //     XEND = 129
     ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
       0x00, 0x02,             //     XSTART = 1
       0x00, 0x81,             //     XEND = 160
     ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
       10,                     //     10 ms delay
     ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
       255 },                  //     255 = 500 ms delay
 
   Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
     15,                       // 15 commands in list:
     ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
       150,                    //     150 ms delay
     ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
       255,                    //     500 ms delay
     ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
       0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
     ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
       0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
     ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
       0x01, 0x2C, 0x2D,       //     Dot inversion mode
       0x01, 0x2C, 0x2D,       //     Line inversion mode
     ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
       0x07,                   //     No inversion
     ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
       0xA2,
       0x02,                   //     -4.6V
       0x84,                   //     AUTO mode
     ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
       0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
     ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
       0x0A,                   //     Opamp current small
       0x00,                   //     Boost frequency
     ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
       0x8A,                   //     BCLK/2, Opamp current small & Medium low
       0x2A,  
     ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
       0x8A, 0xEE,
     ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
       0x0E,
     ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
     ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
       0xC8,                   //     row addr/col addr, bottom to top refresh
     ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
       0x05 },                 //     16-bit color
 
   Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
     2,                        //  2 commands in list:
     ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
       0x00, 0x02,             //     XSTART = 0
       0x00, 0x7F+0x02,        //     XEND = 127
     ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
       0x00, 0x01,             //     XSTART = 0
       0x00, 0x9F+0x01 },      //     XEND = 159
 
   Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
     2,                        //  2 commands in list:
     ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
       0x00, 0x00,             //     XSTART = 0
       0x00, 0x7F,             //     XEND = 127
     ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
       0x00, 0x00,             //     XSTART = 0
       0x00, 0x9F },           //     XEND = 159
 
   Rcmd2green144[] = {              // Init for 7735R, part 2 (green 1.44 tab)
     2,                        //  2 commands in list:
     ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
       0x00, 0x00,             //     XSTART = 0
       0x00, 0x7F,             //     XEND = 127
     ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
       0x00, 0x00,             //     XSTART = 0
       0x00, 0x7F },           //     XEND = 127
 
   Rcmd2green160x80[] = {              // Init for 7735R, part 2 (mini 160x80)
     2,                        //  2 commands in list:
     ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
       0x00, 0x00,             //     XSTART = 0
       0x00, 0x7F,             //     XEND = 79
     ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
       0x00, 0x00,             //     XSTART = 0
       0x00, 0x9F },           //     XEND = 159
 
 
   Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
     4,                        //  4 commands in list:
     ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
       0x02, 0x1c, 0x07, 0x12,
       0x37, 0x32, 0x29, 0x2d,
       0x29, 0x25, 0x2B, 0x39,
       0x00, 0x01, 0x03, 0x10,
     ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
       0x03, 0x1d, 0x07, 0x06,
       0x2E, 0x2C, 0x29, 0x2D,
       0x2E, 0x2E, 0x37, 0x3F,
       0x00, 0x00, 0x02, 0x10,
     ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
       10,                     //     10 ms delay
     ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
       100 };                  //     100 ms delay
 
 
 // Companion code to the above tables.  Reads and issues
 // a series of LCD commands stored in PROGMEM byte array.
 void tzeny_tft::commandList(const uint8_t *addr) {
 
    uint8_t  numCommands, numArgs;
    uint16_t ms;

    numCommands = pgm_read_byte(addr++);   // Number of commands to follow
    while(numCommands--) 
    {                 // For each command...
        writecommand(pgm_read_byte(addr++)); //   Read, issue command
        numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
        ms       = numArgs & DELAY;          //   If hibit set, delay follows args
        numArgs &= ~DELAY;                   //   Mask out delay bit
        while(numArgs--) 
        {                   //   For each argument...
            writedata(pgm_read_byte(addr++));  //     Read, issue argument
        }

        if(ms) 
        {
            ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
            if(ms == 255) ms = 500;     // If 255, delay for 500 ms
            delay(ms);
        }
    }
 }
 
 
 // Initialization code common to both 'B' and 'R' type displays
 void tzeny_tft::commonInit(const uint8_t *cmdList) {
   ystart = xstart = colstart  = rowstart = 0; // May be overridden in init func
 
   pinMode(_dc, OUTPUT);
   pinMode(_cs, OUTPUT);
 
    #if defined(USE_FAST_IO)
        csport    = portOutputRegister(digitalPinToPort(_cs));
        dcport    = portOutputRegister(digitalPinToPort(_dc));
        cspinmask = digitalPinToBitMask(_cs);
        dcpinmask = digitalPinToBitMask(_dc);
    #endif
 
    if(hwSPI) { // Using hardware SPI
    #if defined (SPI_HAS_TRANSACTION)
        SPI.begin();
        mySPISettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
    #elif defined (__AVR__) || defined(CORE_TEENSY)
        SPCRbackup = SPCR;
        SPI.begin();
        SPI.setClockDivider(SPI_CLOCK_DIV4);
        SPI.setDataMode(SPI_MODE0);
        mySPCR = SPCR; // save our preferred state
        //Serial.print("mySPCR = 0x"); Serial.println(SPCR, HEX);
        SPCR = SPCRbackup;  // then restore
    #elif defined (__SAM3X8E__)
        SPI.begin();
        SPI.setClockDivider(21); //4MHz
        SPI.setDataMode(SPI_MODE0);
    #endif
    } else {
        pinMode(_sclk, OUTPUT);
        pinMode(_sid , OUTPUT);
        digitalWrite(_sclk, LOW);
        digitalWrite(_sid, LOW);

    #if defined(USE_FAST_IO)
        clkport     = portOutputRegister(digitalPinToPort(_sclk));
        dataport    = portOutputRegister(digitalPinToPort(_sid));
        clkpinmask  = digitalPinToBitMask(_sclk);
        datapinmask = digitalPinToBitMask(_sid);
    #endif
   }
 
   // toggle RST low to reset; CS low so it'll listen to us
   CS_LOW();
   if (_rst != -1) {
     pinMode(_rst, OUTPUT);
     digitalWrite(_rst, HIGH);
     delay(500);
     digitalWrite(_rst, LOW);
     delay(500);
     digitalWrite(_rst, HIGH);
     delay(500);
   }
 
   if(cmdList) commandList(cmdList);
 }
 
 #define MADCTL_MY  0x80
 #define MADCTL_MX  0x40
 #define MADCTL_MV  0x20
 #define MADCTL_ML  0x10
 #define MADCTL_RGB 0x00
 #define MADCTL_BGR 0x08
 #define MADCTL_MH  0x04
 
 // Initialization for ST7735R screens (green or red tabs)
 void tzeny_tft::initialize() {
    commonInit(Rcmd1);

    commandList(Rcmd2green144);
    colstart = 2;
    rowstart = 3;

    writecommand(ST7735_MADCTL);
    writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);

    xstart = colstart;
    ystart = rowstart;

    commandList(Rcmd3);
 }

int16_t tzeny_tft::height(){
    return _height;
}

int16_t tzeny_tft::width(){
    return _width;
}

void tzeny_tft::setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}


void tzeny_tft::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1,uint8_t y1) {

 
    writecommand(ST7735_CASET); // Column addr set
    writedata(0x00);
    writedata(x0+xstart);     // XSTART 
    writedata(0x00);
    writedata(x1+xstart);     // XEND

    writecommand(ST7735_RASET); // Row addr set
    writedata(0x00);
    writedata(y0+ystart);     // YSTART
    writedata(0x00);
    writedata(y1+ystart);     // YEND

    writecommand(ST7735_RAMWR); // write to RAM
 }
 
 void tzeny_tft::pushColor(uint16_t color) {
    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)    SPI.beginTransaction(mySPISettings);
    #endif

    DC_HIGH();
    CS_LOW();
    spiwrite(color >> 8);
    spiwrite(color);
    CS_HIGH();

    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)    SPI.endTransaction();
    #endif
 }
 
 void tzeny_tft::drawPixel(int16_t x, int16_t y, uint16_t color) {
 
   if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
 
   setAddrWindow(x,y,x+1,y+1);
 
    #if defined (SPI_HAS_TRANSACTION)
    if (hwSPI)     SPI.beginTransaction(mySPISettings);
    #endif
 
    DC_HIGH();
    CS_LOW();
    spiwrite(color >> 8);
    spiwrite(color);
    CS_HIGH();
 
    #if defined (SPI_HAS_TRANSACTION)
    if (hwSPI)     SPI.endTransaction();
    #endif
 }
 
 void tzeny_tft::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
 
    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((y+h-1) >= _height) h = _height-y;
    setAddrWindow(x, y, x, y+h-1);

    uint8_t hi = color >> 8, lo = color;
        
    #if defined (SPI_HAS_TRANSACTION)
        if (hwSPI)      SPI.beginTransaction(mySPISettings);
    #endif

    DC_HIGH();
    CS_LOW();
    while (h--) {
        spiwrite(hi);
        spiwrite(lo);
    }
    CS_HIGH();

    #if defined (SPI_HAS_TRANSACTION)
     if (hwSPI)      SPI.endTransaction();
    #endif
 }
 
 
 void tzeny_tft::drawFastHLine(int16_t x, int16_t y, int16_t w,
   uint16_t color) {
 
   // Rudimentary clipping
   if((x >= _width) || (y >= _height)) return;
   if((x+w-1) >= _width)  w = _width-x;
   setAddrWindow(x, y, x+w-1, y);
 
   uint8_t hi = color >> 8, lo = color;
 
 #if defined (SPI_HAS_TRANSACTION)
   if (hwSPI)      SPI.beginTransaction(mySPISettings);
 #endif
 
   DC_HIGH();
   CS_LOW();
   while (w--) {
     spiwrite(hi);
     spiwrite(lo);
   }
   CS_HIGH();
 
 #if defined (SPI_HAS_TRANSACTION)
   if (hwSPI)      SPI.endTransaction();
 #endif
 }
 
 void tzeny_tft::fillScreen(uint16_t color) {
   fillRect(0, 0,  _width, _height, color);
 }
 
 // fill a rectangle
 void tzeny_tft::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
   uint16_t color) {
 
   // rudimentary clipping (drawChar w/big text requires this)
   if((x >= _width) || (y >= _height)) return;
   if((x + w - 1) >= _width)  w = _width  - x;
   if((y + h - 1) >= _height) h = _height - y;
 
   setAddrWindow(x, y, x+w-1, y+h-1);
 
   uint8_t hi = color >> 8, lo = color;
     
 #if defined (SPI_HAS_TRANSACTION)
   if (hwSPI)      SPI.beginTransaction(mySPISettings);
 #endif
 
   DC_HIGH();
   CS_LOW();
   for(y=h; y>0; y--) {
     for(x=w; x>0; x--) {
       spiwrite(hi);
       spiwrite(lo);
     }
   }
   CS_HIGH();
 
 #if defined (SPI_HAS_TRANSACTION)
   if (hwSPI)      SPI.endTransaction();
 #endif
 }
 
 
 // Pass 8-bit (each) R,G,B, get back 16-bit packed color
 uint16_t tzeny_tft::Color565(uint8_t r, uint8_t g, uint8_t b) {
   return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
 }
 
 
 void tzeny_tft::invertDisplay(boolean i) {
   writecommand(i ? ST7735_INVON : ST7735_INVOFF);
 }
 
 
 /******** low level bit twiddling **********/
 
 inline void tzeny_tft::CS_HIGH(void) {
 #if defined(USE_FAST_IO)
   *csport |= cspinmask;
 #else
   digitalWrite(_cs, HIGH);
 #endif
 }
 
 inline void tzeny_tft::CS_LOW(void) {
 #if defined(USE_FAST_IO)
   *csport &= ~cspinmask;
 #else
   digitalWrite(_cs, LOW);
 #endif
 }
 
 inline void tzeny_tft::DC_HIGH(void) {
 #if defined(USE_FAST_IO)
   *dcport |= dcpinmask;
 #else
   digitalWrite(_dc, HIGH);
 #endif
 }
 
 inline void tzeny_tft::DC_LOW(void) {
 #if defined(USE_FAST_IO)
   *dcport &= ~dcpinmask;
 #else
   digitalWrite(_dc, LOW);
 #endif
 }

 #if ARDUINO >= 100
 size_t tzeny_tft::write(uint8_t c) {
 #else
 void tzeny_tft::write(uint8_t c) {
 #endif
 
    if(c == '\n') {                        // Newline?
        cursor_x  = 0;                     // Reset x to zero,
        cursor_y += textsize * 8;          // advance y one line
    } else if(c != '\r') {                 // Ignore carriage returns
        if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
            cursor_x  = 0;                 // Reset x to zero,
            cursor_y += textsize * 8;      // advance y one line
        }
        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += textsize * 6;          // Advance x one char
    }

 #if ARDUINO >= 100
     return 1;
 #endif
 }

 void tzeny_tft::setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

void tzeny_tft::drawChar(int16_t x, int16_t y, unsigned char c,
    uint16_t color, uint16_t bg, uint8_t size) {
  
    if((x >= _width)            || // Clip right
        (y >= _height)           || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0))   // Clip top
        return;

    if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

    startWrite();
    for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);
        for(int8_t j=0; j<8; j++, line >>= 1) {
            if(line & 1) {
                if(size == 1)
                    writePixel(x+i, y+j, color);
                else
                    writeFillRect(x+i*size, y+j*size, size, size, color);
            } else if(bg != color) {
                if(size == 1)
                    writePixel(x+i, y+j, bg);
                else
                    writeFillRect(x+i*size, y+j*size, size, size, bg);
            }
        }
    }
    if(bg != color) { // If opaque, draw vertical line for last column
        if(size == 1) writeFastVLine(x+5, y, 8, bg);
        else          writeFillRect(x+5*size, y, size, 8*size, bg);
    }
    endWrite();

  }

void tzeny_tft::startWrite(){
    // Overwrite in subclasses if desired!
}

void tzeny_tft::writePixel(int16_t x, int16_t y, uint16_t color){
    // Overwrite in subclasses if startWrite is defined!
    drawPixel(x, y, color);
}

void tzeny_tft::writeFastVLine(int16_t x, int16_t y,
    int16_t h, uint16_t color) {
// Overwrite in subclasses if startWrite is defined!
// Can be just writeLine(x, y, x, y+h-1, color);
// or writeFillRect(x, y, 1, h, color);
drawFastVLine(x, y, h, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void tzeny_tft::writeFastHLine(int16_t x, int16_t y,
    int16_t w, uint16_t color) {
// Overwrite in subclasses if startWrite is defined!
// Example: writeLine(x, y, x+w-1, y, color);
// or writeFillRect(x, y, w, 1, color);
drawFastHLine(x, y, w, color);
}

void tzeny_tft::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t color) {
// Overwrite in subclasses if desired!
fillRect(x,y,w,h,color);
}

void tzeny_tft::endWrite(){
// Overwrite in subclasses if startWrite is defined!
}

void tzeny_tft::setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void tzeny_tft::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            writePixel(y0, x0, color);
        } else {
            writePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void tzeny_tft::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color) {
  if(x0 == x1){
    if(y0 > y1) _swap_int16_t(y0, y1);
    drawFastVLine(x0, y0, y1 - y0 + 1, color);
  } else if(y0 == y1){
    if(x0 > x1) _swap_int16_t(x0, x1);
    drawFastHLine(x0, y0, x1 - x0 + 1, color);
  } else {
    startWrite();
    writeLine(x0, y0, x1, y1, color);
    endWrite();
  }
}

void tzeny_tft::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color) {
  startWrite();
  writeFastHLine(x, y, w, color);
  writeFastHLine(x, y+h-1, w, color);
  writeFastVLine(x, y, h, color);
  writeFastVLine(x+w-1, y, h, color);
  endWrite();
}

void tzeny_tft::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}