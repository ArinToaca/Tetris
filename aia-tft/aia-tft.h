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