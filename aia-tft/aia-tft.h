#ifndef _AIATFT_HH
#define _AIATFT_HH
#endif

#include <Arduino.h>

#define TFT_PIN_COUNT 5
#define TFT_PIN_RST 0
#define TFT_PIN_CD 1
#define TFT_PIN_CS 2
#define TFT_PIN_SCL 3
#define TFT_PIN_SDA 4

#define TFT_MSG_DEV_POWER_UP	10
#define TFT_MSG_DEV_POWER_DOWN 11

typedef struct _tft{
	unsigned is_power_up:1;

	tft_dev_fnptr device_cb;
}tft_t;

/* send command bytes and optional arguments */
#define UCG_C10(c0)				0x010, (c0)
#define UCG_C20(c0,c1)				0x020, (c0),(c1)
#define UCG_C11(c0,a0)				0x011, (c0),(a0)
#define UCG_C21(c0,c1,a0)			0x021, (c0),(c1),(a0)
#define UCG_C12(c0,a0,a1)			0x012, (c0),(a0),(a1)
#define UCG_C22(c0,c1,a0,a1)		0x022, (c0),(c1),(a0),(a1)
#define UCG_C13(c0,a0,a1,a2)		0x013, (c0),(a0),(a1),(a2)
#define UCG_C23(c0,c1,a0,a1,a2)		0x023, (c0),(c1),(a0),(a1),(a2)
#define UCG_C14(c0,a0,a1,a2,a3)		0x014, (c0),(a0),(a1),(a2),(a3)
#define UCG_C24(c0,c1,a0,a1,a2,a3)	0x024, (c0),(c1),(a0),(a1),(a2),(a3)
#define UCG_C15(c0,a0,a1,a2,a3,a4)	0x015, (c0),(a0),(a1),(a2),(a3),(a4)

/* send one or more argument bytes */
#define UCG_A1(d0)					0x061, (d0)
#define UCG_A2(d0,d1)					0x062, (d0),(d1)
#define UCG_A3(d0,d1,d2)				0x063, (d0),(d1),(d2)
#define UCG_A4(d0,d1,d2,d3)				0x064, (d0),(d1),(d2),(d3)
#define UCG_A5(d0,d1,d2,d3,d4)			0x065, (d0),(d1),(d2),(d3),(d4)
#define UCG_A6(d0,d1,d2,d3,d4,d5)		0x066, (d0),(d1),(d2),(d3),(d4),(d5)
#define UCG_A7(d0,d1,d2,d3,d4,d5,d6)		0x067, (d0),(d1),(d2),(d3),(d4),(d5),(d6)
#define UCG_A8(d0,d1,d2,d3,d4,d5,d6,d7)	0x068, (d0),(d1),(d2),(d3),(d4),(d5),(d6),(d7)

/* force data mode on CD line */
#define UCG_DATA()					0x070
/* send one or more data bytes */
#define UCG_D1(d0)				0x071, (d0)
#define UCG_D2(d0,d1)				0x072, (d0),(d1)
#define UCG_D3(d0,d1,d2)			0x073, (d0),(d1),(d2)
#define UCG_D4(d0,d1,d2,d3)			0x074, (d0),(d1),(d2),(d3)
#define UCG_D5(d0,d1,d2,d3,d4)		0x075, (d0),(d1),(d2),(d3),(d4)
#define UCG_D6(d0,d1,d2,d3,d4,d5)	0x076, (d0),(d1),(d2),(d3),(d4),(d5)

/* delay by specified value. t = [0..4095] */
#define UCG_DLY_MS(t)				0x080 | (((t)>>8)&15), (t)&255
#define UCG_DLY_US(t)				0x090 | (((t)>>8)&15), (t)&255

/* access procedures to ucg->arg.pixel.pos.x und ucg->arg.pixel.pos.y */
#define UCG_VARX(s,a,o)				0x0a0 | ((s)&15), (a), (o)
#define UCG_VARY(s,a,o)				0x0b0 | ((s)&15), (a), (o)

/* force specific level on RST und CS */
#define UCG_RST(level)				0x0f0 | ((level)&1)
#define UCG_CS(level)				0x0f4 | ((level)&1)

/* Configure CD line for command, arguments and data */
/* Configure CMD/DATA line: "c" logic level CMD, "a" logic level CMD Args */
#define UCG_CFG_CD(c,a)			0x0fc | (((c)&1)<<1) | ((a)&1)

/* Termination byte */
#define UCG_END()					0x00

static const ucg_pgm_uint8_t ucg_tft_128x128_st7735_init_seq[] = {
  UCG_CFG_CD(0,1),				/* DC=0 for command mode, DC=1 for data and args */
  UCG_RST(1),					
  UCG_CS(1),					/* disable chip */
  UCG_DLY_MS(5),
  UCG_RST(0),					
  UCG_DLY_MS(5),
  UCG_RST(1),
  UCG_DLY_MS(50),
  UCG_CS(0),					/* enable chip */
  
  UCG_C10(0x011),				/* sleep out */
  UCG_DLY_MS(10),
  //UCG_C10(0x038),				/* idle mode off */
  UCG_C10(0x013),				/* normal display on */

  

  UCG_C10(0x20), 				/* not inverted */
  //UCG_C10(0x21), 				/* inverted */

  UCG_C11(0x03a, 0x006), 		/* set pixel format to 18 bit */
  //UCG_C11(0x03a, 0x005), 		/* set pixel format to 16 bit */

  //UCG_C12(0x0b1, 0x000, 0x01b), 	/* frame rate control (POR values) */
  //UCG_C10(0x28), 				/* display off */
  //UCG_C11(0x0bf, 0x003), 		/* backlight control 8 */
  UCG_C10(0x029), 				/* display on */
  //UCG_C11(0x051, 0x07f), 		/* brightness */
  //UCG_C11(0x053, 0x02c), 		/* control brightness */
  //UCG_C10(0x028), 				/* display off */


  UCG_C11( 0x036, 0x000),		/* memory control */
  
  UCG_C14(  0x02a, 0x000, 0x000, 0x000, 0x07f),              /* Horizontal GRAM Address Set */
  UCG_C14(  0x02b, 0x000, 0x000, 0x000, 0x09f),              /* Vertical GRAM Address Set */
  UCG_C10(  0x02c),               /* Write Data to GRAM */

  
  UCG_CS(1),					/* disable chip */
  UCG_END(),					/* end of sequence */
};


typedef tft_int_t (*tft_dev_fnptr)(tft_t *tft, tft_int_t msg, void *data); 
typedef tft_int_t (*tft_font_calc_vref_fnptr)(tft_t *tft);

static void tft_com_arduino_init_shift_out(uint8_t dataPin, uint8_t clockPin);

class aiaTft
{
	private:
		//these will hold the control pins, ports and masks 
		uint8_t pin_list[TFT_PIN_COUNT];
		volatile uint8_t *data_port[TFT_PIN_COUNT];
		uint8_t data_mask[TFT_PIN_COUNT];
	
	public:
		aiaTft(uint8_t scl, uint8_t sda, uint8_t cd, uint8_t cs, uint8_t reset)
		{
			this->pin_list[TFT_PIN_SCL] = scl; 	
			this->pin_list[TFT_PIN_SDA] = sda; 
			this->pin_list[TFT_PIN_RST] = reset; 
			this->pin_list[TFT_PIN_CD] = cd;
			this->pin_list[TFT_PIN_CS] = cs;

			this->data_port[TFT_PIN_RST] = portOutputRegister(digitalPinToPort(reset));
			this->data_mask[TFT_PIN_RST] = digitalPinToBitMask(reset);
			this->data_port[TFT_PIN_CD] = portOutputRegister(digitalPinToPort(cd));
			this->data_mask[TFT_PIN_CD] = digitalPinToBitMask(cd);
			this->data_port[TFT_PIN_CS] = portOutputRegister(digitalPinToPort(cs));
			this->data_mask[TFT_PIN_CS] = digitalPinToBitMask(cs);
		}
		int performOperation();

		void power_down(tft_t *tft);
		void power_up(tft_t *tft);
};