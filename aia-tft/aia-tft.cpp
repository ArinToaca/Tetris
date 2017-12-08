#include "aia-tft.h" 

static void ucg_com_arduino_init_shift_out(uint8_t dataPin, uint8_t clockPin)
{
  u8g_outData = portOutputRegister(digitalPinToPort(dataPin));
  u8g_outClock = portOutputRegister(digitalPinToPort(clockPin));
  u8g_bitData = digitalPinToBitMask(dataPin);
  u8g_bitClock = digitalPinToBitMask(clockPin);

  u8g_bitNotClock = u8g_bitClock;
  u8g_bitNotClock ^= 0x0ff;

  u8g_bitNotData = u8g_bitData;
  u8g_bitNotData ^= 0x0ff;
}

void aiaTft::powerUp(tft_t *tft)
{
	this->performOperation(tft, TFT_MSG_DEV_POWER_UP, NULL);
}

void aiaTft::powerDown(tft_t *tft)
{
	if ( tft->is_power_up != 0 )
	{
		tft->device_cb(ucg, UCG_MSG_DEV_POWER_DOWN, NULL);
		tft->is_power_up = 0;
	}
}

uint8_t aiaTft::performOperation(tft_t *tft, uint8_t msg, void *data)
{
  switch(msg)
  {
    case TFT_MSG_DEV_POWER_UP:
	  /* 1. Call to the controller procedures to setup the com interface */
		#ifdef __AVR__
			ucg_com_arduino_init_shift_out(ucg->pin_list[UCG_PIN_SDA], ucg->pin_list[UCG_PIN_SCL]);
		#endif

		pinMode(ucg->pin_list[UCG_PIN_CD], OUTPUT);
		pinMode(ucg->pin_list[UCG_PIN_SDA], OUTPUT);
		pinMode(ucg->pin_list[UCG_PIN_SCL], OUTPUT);
		pinMode(ucg->pin_list[UCG_PIN_CS], OUTPUT);
		pinMode(ucg->pin_list[UCG_PIN_RST], OUTPUT);

		digitalWrite(ucg->pin_list[UCG_PIN_CD], 1);
		digitalWrite(ucg->pin_list[UCG_PIN_SDA], 1);
		digitalWrite(ucg->pin_list[UCG_PIN_SCL], 0);
		digitalWrite(ucg->pin_list[UCG_PIN_CS], 1);
		digitalWrite(ucg->pin_list[UCG_PIN_RST], 1);
	return 0;

      /* 2. Send specific init sequence for this display module */
      ucg_com_SendCmdSeq(ucg, ucg_tft_128x128_st7735_init_seq);
      
      return 1;
      
    case UCG_MSG_DEV_POWER_DOWN:
      /* let do power down by the conroller procedures */
      return ucg_dev_ic_st7735_18(ucg, msg, data);  
    
    case UCG_MSG_GET_DIMENSION:
      ((ucg_wh_t *)data)->w = 128;
      ((ucg_wh_t *)data)->h = 128;
      return 1;
  }
  
  /* all other messages are handled by the controller procedures */
  return ucg_dev_ic_st7735_18(ucg, msg, data);  
}
