#include "aia-tft.h"
#include <string.h> 	/* memset */

//initialize the ucg structure with default values
void ucg_init_struct(ucg_t *ucg)
{
	//memset(ucg, 0, sizeof(ucg_t));
	ucg->is_power_up = 0;
	ucg->rotate_chain_device_cb = 0;
	ucg->arg.scale = 1;
	//ucg->display_offset.x = 0;
	//ucg->display_offset.y = 0;
	ucg->font = 0;
	//ucg->font_mode = UCG_FONT_MODE_NONE;   Old font procedures
	ucg->font_decode.is_transparent = 1;  // new font procedures

	ucg->com_initial_change_sent = 0;
	ucg->com_status = 0;
	ucg->com_cfg_cd = 0;
}

void ucg_SetFontPosBaseline(ucg_t *ucg)
{
  ucg->font_calc_vref = 0;
}

//power up display
ucg_int_t ucg_PowerUp(ucg_t *ucg)
{
  ucg_int_t r;
  /* power down first. will do nothing if power is already down */
  ucg_PowerDown(ucg);
  /* now try to power up the display */
  r = ucg->device_cb(ucg, UCG_MSG_DEV_POWER_UP, NULL);
  if ( r != 0 )
  {
    ucg->is_power_up = 1;
  }
  return r;
}

//power down display
void ucg_PowerDown(ucg_t *ucg)
{
  if ( ucg->is_power_up != 0 )
  {
    ucg->device_cb(ucg, UCG_MSG_DEV_POWER_DOWN, NULL);
    ucg->is_power_up = 0;
  }
}


void ucg_GetDimension(ucg_t *ucg)
{
  ucg->device_cb(ucg, UCG_MSG_GET_DIMENSION, &(ucg->dimension));
  ucg_SetMaxClipRange(ucg);
}

void ucg_SetMaxClipRange(ucg_t *ucg)
{
  ucg_box_t new_clip_box;
  new_clip_box.size = ucg->dimension;
  new_clip_box.ul.x = 0;
  new_clip_box.ul.y = 0;
  ucg_SetClipBox(ucg, &new_clip_box);
}

void ucg_SetClipBox(ucg_t *ucg, ucg_box_t *clip_box)
{
  ucg->device_cb(ucg, UCG_MSG_SET_CLIP_BOX, (void *)(clip_box));
}

ucg_int_t ucg_Init(ucg_t *ucg, ucg_dev_fnptr device_cb, ucg_dev_fnptr ext_cb, ucg_com_fnptr com_cb)
{
	ucg_int_t r;
	ucg_init_struct(ucg);

	if ( ext_cb == (ucg_dev_fnptr)0 )
		ucg->ext_cb = ucg_ext_none;
	else 
		ucg->ext_cb = ext_cb;

	ucg->device_cb = device_cb;
	ucg->com_cb = com_cb;
	ucg_SetFontPosBaseline(ucg);
	r = ucg_PowerUp(ucg);
	ucg_GetDimension(ucg);
	return r;
}
