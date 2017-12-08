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

//void ucg_com_SendCmdSeq(ucg_t *ucg, const ucg_pgm_uint8_t *data)
void ucg_com_SendCmdSeq(ucg_t *ucg, const ucg_pgm_uint8_t *data)
{
  uint8_t b;
  uint8_t bb;
  uint8_t hi;
  uint8_t lo;

  for(;;)
  {
    b = ucg_pgm_read(data);
    //b = *data;
    hi = (b) >> 4;
    lo = (b) & 0x0f;
    switch( hi )
    {
      case 0:
	return;		/* end marker */
      case 1:
      case 2:
      case 3:
	ucg_com_SendCmdArg(ucg, data+1, hi, lo);
	data+=1+hi+lo;
	break;
      case 6:
	ucg_com_SetCDLineStatus(ucg, (ucg->com_cfg_cd)&1 );
	ucg_com_SendStringP(ucg, lo, data+1);
	data+=1+lo;      
	break;
      case 7:	/* note: 0x070 is used to set data line status */
	ucg_com_SetCDLineStatus(ucg, ((ucg->com_cfg_cd>>1)&1)^1 );
	if ( lo > 0 )
	  ucg_com_SendStringP(ucg, lo, data+1);
	data+=1+lo;      
	break;
      case 8:
	data++;
	b = ucg_pgm_read(data);
	//b = *data;
	ucg_com_DelayMilliseconds(ucg, (((uint16_t)lo)<<8) + b );
	data++;
	break;
      case 9:
	data++;
	b = ucg_pgm_read(data);
	//b = *data;
	ucg_com_DelayMicroseconds(ucg, (((uint16_t)lo)<<8) + b );
	data++;
	break;
      case 10:
	data++;
	b = ucg_pgm_read(data);
	data++;
	bb = ucg_pgm_read(data);
	data++;
	//b = data[0];
	//bb = data[1];
	ucg_com_SetCDLineStatus(ucg, (ucg->com_cfg_cd)&1 );
	ucg_com_SendByte(ucg, (((uint8_t)(((ucg->arg.pixel.pos.x)>>lo)))&b)|bb );
	//data+=2;
	break;
      case 11:
	data++;
	b = ucg_pgm_read(data);
	data++;
	bb = ucg_pgm_read(data);
	data++;
	//b = data[0];
	//bb = data[1];
	ucg_com_SetCDLineStatus(ucg, (ucg->com_cfg_cd)&1 );
	ucg_com_SendByte(ucg, (((uint8_t)(((ucg->arg.pixel.pos.y)>>lo)))&b)|bb );
	//data+=2;
	break;
      case 15:
	hi = lo >> 2;
	lo &= 3;
	switch(hi)
	{
	  case 0:
	    ucg_com_SetResetLineStatus(ucg, lo&1);
	    break;
	  case 1:
	    ucg_com_SetCSLineStatus(ucg, lo&1);
	    break;
	  case 3:
	    ucg->com_cfg_cd = lo;
	    break;
	}
	data++;
	break;
      default:
	return;
    }  
  }
}

uint8_t performOperation(tft_t *tft, uint8_t msg, void *data)
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
