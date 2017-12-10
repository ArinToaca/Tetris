#ifndef _TZENY_TFT_
#define _TZENY_TFT_

#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>

#if defined(__AVR__) || defined(CORE_TEENSY)
  #include <avr/pgmspace.h>
  #define USE_FAST_IO
  typedef volatile uint8_t RwReg;
#elif defined(ARDUINO_STM32_FEATHER)
  typedef volatile uint32 RwReg;
  #define USE_FAST_IO
#elif defined(ARDUINO_FEATHER52)
  typedef volatile uint32_t RwReg;
  #define USE_FAST_IO
#elif defined(__SAM3X8E__)
  #undef __FlashStringHelper::F(string_literal)
  #define F(string_literal) string_literal
  #include <include/pio.h>
  #define PROGMEM
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
  typedef unsigned char prog_uchar;
#endif

//uC ops
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

class tzeny_tft : public Print {
    
    public:
        tzeny_tft(int8_t CS, int8_t RS, int8_t RST = -1);
        int16_t height(),
                width();

        void    initialize(),
                setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1),
                pushColor(uint16_t color),
                fillScreen(uint16_t color),
                drawPixel(int16_t x, int16_t y, uint16_t color),
                drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
                drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
                fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                    uint16_t color),
                setCursor(int16_t x, int16_t y),
                setTextSize(uint8_t s) ,
                drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                    uint16_t bg, uint8_t size),
                invertDisplay(boolean i);
        uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
        
        void setTextColor(uint16_t c);

        #if ARDUINO >= 100
            virtual size_t write(uint8_t);
        #else
            virtual void   write(uint8_t);
        #endif

        virtual void startWrite(void);
        virtual void writePixel(int16_t x, int16_t y, uint16_t color);
        virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
        virtual void endWrite(void);

        virtual void // Optional and probably not necessary to change
            drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
            drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
            drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

    private:
        int16_t
        _width, _height, // Display w/h as modified by current rotation
        cursor_x, cursor_y;    
        uint8_t textsize;
        boolean wrap;
        uint16_t textcolor, textbgcolor;
    
    
    void    spiwrite(uint8_t),
            writecommand(uint8_t c),
            writedata(uint8_t d),
            commandList(const uint8_t *addr),
            commonInit(const uint8_t *cmdList);
    //uint8_t  spiread(void);
    
    
    inline void CS_HIGH(void);
    inline void CS_LOW(void);
    inline void DC_HIGH(void);
    inline void DC_LOW(void);
    
    boolean  hwSPI, _cp437;

    int8_t  _cs, _dc, _rst, _sid, _sclk;
    uint8_t colstart, rowstart, xstart, ystart; // some displays need this changed
    
    #if defined(USE_FAST_IO)
        volatile RwReg  *dataport, *clkport, *csport, *dcport;
    
        #if defined(__AVR__) || defined(CORE_TEENSY)  // 8 bit!
            uint8_t  datapinmask, clkpinmask, cspinmask, dcpinmask;
        #else    // 32 bit!
            uint32_t  datapinmask, clkpinmask, cspinmask, dcpinmask;
        #endif
    #endif
    
};


#endif