#define ucg_GetWidth(ucg) ((ucg)->dimension.w)
#define ucg_GetHeight(ucg) ((ucg)->dimension.h)

#define USE_PIN_LIST

#define UCG_MSG_DEV_POWER_UP	10
#define UCG_MSG_DEV_POWER_DOWN 11
#define UCG_MSG_SET_CLIP_BOX 12
#define UCG_MSG_GET_DIMENSION 15

/* draw pixel with color from idx 0 */
#define UCG_MSG_DRAW_PIXEL 20
#define UCG_MSG_DRAW_L90FX 21
/* draw  bit pattern, transparent and draw color (idx 0) color */
//#define UCG_MSG_DRAW_L90TC 22		/* can be commented, used by ucg_DrawTransparentBitmapLine */
#define UCG_MSG_DRAW_L90SE 23		/* this part of the extension */
//#define UCG_MSG_DRAW_L90RL 24	/* not yet implemented */
/* draw  bit pattern with foreground (idx 1) and background (idx 0) color */
//#define UCG_MSG_DRAW_L90BF 25	 /* can be commented, used by ucg_DrawBitmapLine */

#include <Print.h>
#include <Arduino.h>

#define UCG_COM_STATUS_MASK_POWER 8
#define UCG_COM_STATUS_MASK_RESET 4
#define UCG_COM_STATUS_MASK_CS 2
#define UCG_COM_STATUS_MASK_CD 1

/*
  arg:	0
  data:	ucg_com_info_t *
  return:	0 for error
  note: 
    - power up is the first command, which is sent
*/
#define UCG_COM_MSG_POWER_UP 10

/*
  note: power down my be followed only by power up command
*/
#define UCG_COM_MSG_POWER_DOWN 11

/*
  arg:	delay in microseconds  (0..4095) 
*/
#define UCG_COM_MSG_DELAY 12

/*
  ucg->com_status	contains previous status of reset line
  arg:			new logic level for reset line
*/
#define UCG_COM_MSG_CHANGE_RESET_LINE 13
/*
  ucg->com_status	contains previous status of cs line
  arg:	new logic level for cs line
*/
#define UCG_COM_MSG_CHANGE_CS_LINE 14

/*
  ucg->com_status	contains previous status of cd line
  arg:	new logic level for cd line
*/
#define UCG_COM_MSG_CHANGE_CD_LINE 15

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			byte for display
*/
#define UCG_COM_MSG_SEND_BYTE 16

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			how often to repeat the 2/3 byte sequence 	
  data:			pointer to two or three bytes
*/
#define UCG_COM_MSG_REPEAT_1_BYTE 17
#define UCG_COM_MSG_REPEAT_2_BYTES 18
#define UCG_COM_MSG_REPEAT_3_BYTES 19

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			length of string 	
  data:			string
*/
#define UCG_COM_MSG_SEND_STR 20

/*
  ucg->com_status	current status of Reset, CS and CD line (ucg->com_status)
  arg:			number of cd_info and data pairs (half value of total byte cnt) 	
  data:			uint8_t with CD and data information
	cd_info data cd_info data cd_info data cd_info data ... cd_info data cd_info data
	cd_info is the level, which is directly applied to the CD line. This means,
	information applied to UCG_CFG_CD is not relevant.
*/
#define UCG_COM_MSG_SEND_CD_DATA_SEQUENCE 21

#define UCG_PIN_RST 0
#define UCG_PIN_CD 1
#define UCG_PIN_CS 2
#define UCG_PIN_SCL 3
#define UCG_PIN_WR 3
#define UCG_PIN_SDA 4

#define UCG_PIN_D0 5
#define UCG_PIN_D1 6
#define UCG_PIN_D2 7
#define UCG_PIN_D3 8
#define UCG_PIN_D4 9
#define UCG_PIN_D5 10
#define UCG_PIN_D6 11
#define UCG_PIN_D7 12

#define UCG_PIN_COUNT 13

#define UCG_PIN_VAL_NONE 255

#ifdef __GNUC__
#  define UCG_NOINLINE __attribute__((noinline))
#  define UCG_SECTION(name) __attribute__ ((section (name)))
#  if defined(__MSPGCC__)
/* mspgcc does not have .progmem sections. Use -fdata-sections. */
#    define UCG_FONT_SECTION(name)
#  elif defined(__AVR__)
#    define UCG_FONT_SECTION(name) UCG_SECTION(".progmem." name)
#  else
#    define UCG_FONT_SECTION(name)
#  endif
#else
#  define UCG_NOINLINE
#  define UCG_SECTION(name)
#  define UCG_FONT_SECTION(name)
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
/* UCG_PROGMEM is used by the XBM example */
#define UCG_PROGMEM UCG_SECTION(".progmem.data")
typedef uint8_t PROGMEM ucg_pgm_uint8_t;
typedef uint8_t ucg_fntpgm_uint8_t;
#define ucg_pgm_read(adr) pgm_read_byte_near(adr)
#define UCG_PSTR(s) ((ucg_pgm_uint8_t *)PSTR(s))
#else
#define UCG_PROGMEM
#define PROGMEM
typedef uint8_t ucg_pgm_uint8_t;
typedef uint8_t ucg_fntpgm_uint8_t;
#define ucg_pgm_read(adr) (*(const ucg_pgm_uint8_t *)(adr)) 
#define UCG_PSTR(s) ((ucg_pgm_uint8_t *)(s))
#endif

/*================================================*/
/* several type and forward definitions */

typedef int16_t ucg_int_t;
typedef struct _ucg_t ucg_t;
typedef struct _ucg_xy_t ucg_xy_t;
typedef struct _ucg_wh_t ucg_wh_t;
typedef struct _ucg_box_t ucg_box_t;
typedef struct _ucg_color_t ucg_color_t;
typedef struct _ucg_ccs_t ucg_ccs_t;
typedef struct _ucg_pixel_t ucg_pixel_t;
typedef struct _ucg_arg_t ucg_arg_t;
typedef struct _ucg_com_info_t ucg_com_info_t;
typedef struct _ucg_font_info_t ucg_font_info_t;
typedef struct _ucg_font_decode_t ucg_font_decode_t;

typedef ucg_int_t (*ucg_dev_fnptr)(ucg_t *ucg, ucg_int_t msg, void *data); 
typedef int16_t (*ucg_com_fnptr)(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data); 
typedef ucg_int_t (*ucg_font_calc_vref_fnptr)(ucg_t *ucg);

ucg_int_t ucg_ext_none(ucg_t *ucg, ucg_int_t msg, void *data);
ucg_int_t ucg_ext_st7735_18(ucg_t *ucg, ucg_int_t msg, void *data);
//typedef ucg_int_t (*ucg_font_mode_fnptr)(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);

struct _ucg_font_decode_t
{
  const uint8_t *decode_ptr;			/* pointer to the compressed data */
  
  ucg_int_t target_x;
  ucg_int_t target_y;
  
  int8_t x;						/* local coordinates, (0,0) is upper left */
  int8_t y;
  int8_t glyph_width;	
  int8_t glyph_height;

  uint8_t decode_bit_pos;			/* bitpos inside a byte of the compressed data */
  uint8_t is_transparent;
  uint8_t dir;				/* direction */
};

struct _ucg_font_info_t
{
  /* offset 0 */
  uint8_t glyph_cnt;
  uint8_t bbx_mode;
  uint8_t bits_per_0;
  uint8_t bits_per_1;
  
  /* offset 4 */
  uint8_t bits_per_char_width;
  uint8_t bits_per_char_height;		
  uint8_t bits_per_char_x;
  uint8_t bits_per_char_y;
  uint8_t bits_per_delta_x;
  
  /* offset 9 */
  int8_t max_char_width;
  int8_t max_char_height; /* overall height, NOT ascent. Instead ascent = max_char_height + y_offset */
  int8_t x_offset;
  int8_t y_offset;
  
  /* offset 13 */
  int8_t  ascent_A;
  int8_t  descent_g;
  int8_t  ascent_para;
  int8_t  descent_para;
  
  /* offset 17 */
  uint16_t start_pos_upper_A;
  uint16_t start_pos_lower_a;  
};

struct _ucg_ccs_t
{
  uint8_t current;	/* contains the current color component */
  uint8_t start;
  ucg_int_t dir;		/* 1 if start < end or -1 if start > end */
  ucg_int_t num;
  ucg_int_t quot;
  
  ucg_int_t den;
  ucg_int_t rem;  
  ucg_int_t frac;
};

struct _ucg_color_t
{
  uint8_t color[3];		/* 0: Red, 1: Green, 2: Blue */
};

struct _ucg_xy_t
{
  ucg_int_t x;
  ucg_int_t y;
};

struct _ucg_pixel_t
{
  ucg_xy_t pos;
  ucg_color_t rgb;  
};


struct _ucg_arg_t
{
  ucg_pixel_t pixel;
  ucg_int_t len;
  ucg_int_t dir;
  ucg_int_t offset;			/* calculated offset from the inital point to the start of the clip window (ucg_clip_l90fx) */
  ucg_int_t scale;			/* upscale factor, used by UCG_MSG_DRAW_L90BF */
  const unsigned char *bitmap;
  ucg_int_t pixel_skip;		/* within the "bitmap" skip the specified number of pixel with the bit. pixel_skip is always <= 7 */
  ucg_color_t rgb[4];			/* start and end color for L90SE , two more colors for the gradient box */
  ucg_ccs_t ccs_line[3];		/* color component sliders used by L90SE */
};

struct _ucg_wh_t
{
  ucg_int_t w;  
  ucg_int_t h;
};

struct _ucg_box_t
{
  ucg_xy_t ul;
  ucg_wh_t size;
};


//used to hold data about display
typedef struct _ucg_t
{
  unsigned is_power_up:1;
  /* the dimension of the display */
  ucg_wh_t dimension;
  /* display callback procedure to handle display specific code */
  //ucg_dev_fnptr display_cb;
  /* controller and device specific code, high level procedure will call this */
  ucg_dev_fnptr device_cb;
  /* name of the extension cb. will be called by device_cb if required */
  ucg_dev_fnptr ext_cb;
  /* if rotation is applied, than this cb is called after rotation */
  ucg_dev_fnptr rotate_chain_device_cb;
  ucg_wh_t rotate_dimension;

  /* if rotation is applied, than this cb is called by the scale device */
  ucg_dev_fnptr scale_chain_device_cb;
  
  /* communication interface */
  ucg_com_fnptr com_cb;
  
  /* offset, that is additionally added to UCG_VARX/UCG_VARY */
  /* seems to be required for the Nokia display */
  // ucg_xy_t display_offset;
  
  /* data which is passed to the cb procedures */
  /* can be modified by the cb procedures (rotation, clipping, etc) */
  ucg_arg_t arg;
  /* current window to which all drawing is clipped */
  /* should be modified via UCG_MSG_SET_CLIP_BOX by a device callback. */
  /* by default this is done by ucg_dev_default_cb */
  ucg_box_t clip_box;
  

  /* information about the current font */
  const unsigned char *font;             /* current font for all text procedures */
  ucg_font_calc_vref_fnptr font_calc_vref;
  //ucg_font_mode_fnptr font_mode;		/* OBSOLETE?? UCG_FONT_MODE_TRANSPARENT, UCG_FONT_MODE_SOLID, UCG_FONT_MODE_NONE */

  ucg_font_decode_t font_decode;		/* new font decode structure */
  ucg_font_info_t font_info;			/* new font info structure */

  int8_t glyph_dx;			/* OBSOLETE */
  int8_t glyph_x;			/* OBSOLETE */
  int8_t glyph_y;			/* OBSOLETE */
  uint8_t glyph_width;		/* OBSOLETE */
  uint8_t glyph_height;		/* OBSOLETE */
  
  uint8_t font_height_mode;
  int8_t font_ref_ascent;
  int8_t font_ref_descent;

  /* only for Arduino/C++ Interface */
#ifdef USE_PIN_LIST
  uint8_t pin_list[UCG_PIN_COUNT];

#ifdef __AVR__
  volatile uint8_t *data_port[UCG_PIN_COUNT];
  uint8_t data_mask[UCG_PIN_COUNT];
#endif

#endif

  /* 
    Small amount of RAM for the com interface (com_cb).
    Might be unused on unix systems, where the com sub system is 
    not required, but should be usefull for all uC projects.
  */
  uint8_t com_initial_change_sent;	/* Bit 0: CD/A0 Line Status, Bit 1: CS Line Status, Bit 2: Reset Line Status */
  uint8_t com_status;		/* Bit 0: CD/A0 Line Status, Bit 1: CS Line Status, Bit 2: Reset Line Status,  Bit 3: 1 for power up */
  uint8_t com_cfg_cd;		/* Bit 0: Argument Level, Bit 1: Command Level */
  
  
} ugt_t;

//ucg_int_t ucg_draw_transparent_glyph(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);
//ucg_int_t ucg_draw_solid_glyph(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);

// old font procedures
//#define UCG_FONT_MODE_TRANSPARENT ucg_draw_transparent_glyph
//#define UCG_FONT_MODE_SOLID ucg_draw_solid_glyph
//#define UCG_FONT_MODE_NONE ((ucg_font_mode_fnptr)0)

// new font procedures
#define UCG_FONT_MODE_TRANSPARENT 1
#define UCG_FONT_MODE_SOLID 0
#define UCG_FONT_MODE_NONE 1


/* Information on a specific given font */
uint8_t ucg_font_GetFontStartEncoding(const void *font);
uint8_t ucg_font_GetFontEndEncoding(const void *font);

uint8_t ucg_font_GetCapitalAHeight(const void *font);

int8_t ucg_font_GetFontAscent(const void *font);
int8_t ucg_font_GetFontDescent(const void *font);

int8_t ucg_font_GetFontXAscent(const void *font);
int8_t ucg_font_GetFontXDescent(const void *font);

size_t ucg_font_GetSize(const void *font);

/* Information on the current font */

uint8_t ucg_GetFontBBXWidth(ucg_t *ucg);
uint8_t ucg_GetFontBBXHeight(ucg_t *ucg);
uint8_t ucg_GetFontCapitalAHeight(ucg_t *ucg) UCG_NOINLINE; 
uint8_t ucg_IsGlyph(ucg_t *ucg, uint8_t requested_encoding);
int8_t ucg_GetGlyphWidth(ucg_t *ucg, uint8_t requested_encoding);

#define ucg_GetFontAscent(ucg)	((ucg)->font_ref_ascent)
#define ucg_GetFontDescent(ucg)	((ucg)->font_ref_descent)

/* Drawing procedures */

ucg_int_t ucg_DrawGlyph(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding);
ucg_int_t ucg_DrawString(ucg_t *ucg, ucg_int_t x, ucg_int_t y, uint8_t dir, const char *str);

/* Mode selection/Font assignment */

void ucg_SetFontRefHeightText(ucg_t *ucg);
void ucg_SetFontRefHeightExtendedText(ucg_t *ucg);
void ucg_SetFontRefHeightAll(ucg_t *ucg);

void ucg_SetFontPosBaseline(ucg_t *ucg) UCG_NOINLINE;
void ucg_SetFontPosBottom(ucg_t *ucg);
void ucg_SetFontPosTop(ucg_t *ucg);
void ucg_SetFontPosCenter(ucg_t *ucg);

void ucg_SetFont(ucg_t *ucg, const ucg_fntpgm_uint8_t  *font);
//void ucg_SetFontMode(ucg_t *ucg, ucg_font_mode_fnptr font_mode);
void ucg_SetFontMode(ucg_t *ucg, uint8_t is_transparent);

ucg_int_t ucg_GetStrWidth(ucg_t *ucg, const char *s);

/* ucg_pixel.c */
void ucg_SetColor(ucg_t *ucg, uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
void ucg_DrawPixel(ucg_t *ucg, ucg_int_t x, ucg_int_t y);

/* ucg_rotate.c */
void ucg_UndoRotate(ucg_t *ucg);
void ucg_SetRotate90(ucg_t *ucg);
void ucg_SetRotate180(ucg_t *ucg);
void ucg_SetRotate270(ucg_t *ucg);

/* ucg_scale.c */
void ucg_UndoScale(ucg_t *ucg);
void ucg_SetScale2x2(ucg_t *ucg);

/* ucg_dev_msg_api.c */
void ucg_PowerDown(ucg_t *ucg);
ucg_int_t ucg_PowerUp(ucg_t *ucg);
void ucg_SetClipBox(ucg_t *ucg, ucg_box_t *clip_box);
void ucg_SetClipRange(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_SetMaxClipRange(ucg_t *ucg);
void ucg_GetDimension(ucg_t *ucg);
void ucg_DrawPixelWithArg(ucg_t *ucg);
void ucg_DrawL90FXWithArg(ucg_t *ucg);
void ucg_DrawL90TCWithArg(ucg_t *ucg);
void ucg_DrawL90BFWithArg(ucg_t *ucg);
void ucg_DrawL90SEWithArg(ucg_t *ucg);
/* void ucg_DrawL90RLWithArg(ucg_t *ucg); */

/* ucg_line.c */
void ucg_Draw90Line(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len, ucg_int_t dir, ucg_int_t col_idx);
void ucg_DrawHLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len);
void ucg_DrawVLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len);
void ucg_DrawHRLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len);
void ucg_DrawLine(ucg_t *ucg, ucg_int_t x1, ucg_int_t y1, ucg_int_t x2, ucg_int_t y2);
/* the following procedure is only available with the extended callback */
void ucg_DrawGradientLine(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t len, ucg_int_t dir);

/* ucg_box.c */
void ucg_DrawBox(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_ClearScreen(ucg_t *ucg);
void ucg_DrawRBox(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h, ucg_int_t r);
void ucg_DrawGradientBox(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_DrawFrame(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h);
void ucg_DrawRFrame(ucg_t *ucg, ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h, ucg_int_t r);

/* ucg_ccs.c */
void ucg_ccs_init(ucg_ccs_t *ccs, uint8_t start, uint8_t end, ucg_int_t steps);
void ucg_ccs_step(ucg_ccs_t *ccs);
void ucg_ccs_seek(ucg_ccs_t *ccs, ucg_int_t pos);

/* ucg_circle.c */
#define UCG_DRAW_UPPER_RIGHT 0x01
#define UCG_DRAW_UPPER_LEFT  0x02
#define UCG_DRAW_LOWER_LEFT 0x04
#define UCG_DRAW_LOWER_RIGHT  0x08
#define UCG_DRAW_ALL (UCG_DRAW_UPPER_RIGHT|UCG_DRAW_UPPER_LEFT|UCG_DRAW_LOWER_RIGHT|UCG_DRAW_LOWER_LEFT)
void ucg_DrawDisc(ucg_t *ucg, ucg_int_t x0, ucg_int_t y0, ucg_int_t rad, uint8_t option);
void ucg_DrawCircle(ucg_t *ucg, ucg_int_t x0, ucg_int_t y0, ucg_int_t rad, uint8_t option);

/* ucg_polygon.c */

typedef int16_t pg_word_t;

#define PG_NOINLINE UCG_NOINLINE

struct pg_point_struct
{
  pg_word_t x;
  pg_word_t y;
};

typedef struct _pg_struct pg_struct;	/* forward declaration */

struct pg_edge_struct
{
  pg_word_t x_direction;	/* 1, if x2 is greater than x1, -1 otherwise */
  pg_word_t height;
  pg_word_t current_x_offset;
  pg_word_t error_offset;
  
  /* --- line loop --- */
  pg_word_t current_y;
  pg_word_t max_y;
  pg_word_t current_x;
  pg_word_t error;

  /* --- outer loop --- */
  uint8_t (*next_idx_fn)(pg_struct *pg, uint8_t i);
  uint8_t curr_idx;
};

/* maximum number of points in the polygon */
/* can be redefined, but highest possible value is 254 */
#define PG_MAX_POINTS 4

/* index numbers for the pge structures below */
#define PG_LEFT 0
#define PG_RIGHT 1


struct _pg_struct
{
  struct pg_point_struct list[PG_MAX_POINTS];
  uint8_t cnt;
  uint8_t is_min_y_not_flat;
  pg_word_t total_scan_line_cnt;
  struct pg_edge_struct pge[2];	/* left and right line draw structures */
};

void pg_ClearPolygonXY(pg_struct *pg);
void pg_AddPolygonXY(pg_struct *pg, ucg_t *ucg, int16_t x, int16_t y);
void pg_DrawPolygon(pg_struct *pg, ucg_t *ucg);
void ucg_ClearPolygonXY(void);
void ucg_AddPolygonXY(ucg_t *ucg, int16_t x, int16_t y);
void ucg_DrawPolygon(ucg_t *ucg);
void ucg_DrawTriangle(ucg_t *ucg, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

void ucg_DrawTetragon(ucg_t *ucg, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3);

// Do not use Ucglib class directly, use Ucglib8Bit or Ucglib4WireSPI instead
class Ucglib : public Print
{
  protected:
    ucg_t ucg;
    ucg_dev_fnptr dev_cb;
    ucg_dev_fnptr ext_cb;  
  private:
    ucg_int_t tx, ty;          // current position for the Print base class procedures
    uint8_t tdir;
  protected:
    uint8_t& get_tdir(void) { return tdir; };
    ucg_int_t& get_tx(void) { return tx; };
    ucg_int_t& get_ty(void) { return ty; };
    ucg_t *get_ucg(void) { return &ucg; };
    void init(void);
  public:
    Ucglib(void) { init(); }
    Ucglib(ucg_dev_fnptr dev, ucg_dev_fnptr ext = ucg_ext_none) { init(); dev_cb = dev; ext_cb = ext; }

    void setPrintPos(ucg_int_t x, ucg_int_t y) { tx = x; ty = y; }
    void setPrintDir(uint8_t dir) { tdir = dir; }
    size_t write(uint8_t c);
    ucg_t *getUcg(void) { return &ucg; }
    
    ucg_int_t getWidth(void) { return ucg_GetWidth(&ucg); }
    ucg_int_t getHeight(void) { return ucg_GetHeight(&ucg); }
    
    
    void setFontRefHeightText(void) 	{ ucg_SetFontRefHeightText(&ucg); }
    void setFontRefHeightExtendedText(void) { ucg_SetFontRefHeightExtendedText(&ucg); }
    void setFontRefHeightAll(void) 	{ ucg_SetFontRefHeightAll(&ucg); }

    void setFontPosBaseline(void) 	{ ucg_SetFontPosBaseline(&ucg); }
    void setFontPosBottom(void) 	{ ucg_SetFontPosBottom(&ucg); }
    void setFontPosTop(void) 		{ ucg_SetFontPosTop(&ucg); }
    void setFontPosCenter(void) 	{ ucg_SetFontPosCenter(&ucg); }
    
    void setFont(const ucg_fntpgm_uint8_t  *font)
      { ucg_SetFont(&ucg, font); }
    void setFontMode(uint8_t is_transparent) 
      { ucg_SetFontMode(&ucg, is_transparent); }
    ucg_int_t getFontAscent(void)
      { return ucg_GetFontAscent(&ucg); }
    ucg_int_t getFontDescent(void)
      { return ucg_GetFontDescent(&ucg); }
    ucg_int_t getStrWidth(const char *s)
      { return ucg_GetStrWidth(&ucg, s); }
    
    void setColor(uint8_t idx, uint8_t r, uint8_t g, uint8_t b)
      { ucg_SetColor(&ucg, idx, r, g, b); }
    void setColor(uint8_t r, uint8_t g, uint8_t b)
      { ucg_SetColor(&ucg, 0, r, g, b); }

      
    void undoRotate(void) { ucg_UndoRotate(&ucg); }
    void setRotate90(void) { ucg_SetRotate90(&ucg); }
    void setRotate180(void) { ucg_SetRotate180(&ucg); }
    void setRotate270(void) { ucg_SetRotate270(&ucg); }

    void undoScale(void) { ucg_UndoScale(&ucg); }
    void setScale2x2(void) { ucg_SetScale2x2(&ucg); }
    
    void powerDown(void) { ucg_PowerDown(&ucg); }
    void powerUp(void) { ucg_PowerUp(&ucg); }
    
    
    
    // Procedures, which are always available as part of the BASIC drawing procedure set
      
    void setClipRange(ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h) { ucg_SetClipRange(&ucg, x,y,w,h); }
    void setMaxClipRange(void) { ucg_SetMaxClipRange(&ucg); }
    void undoClipRange(void) { ucg_SetMaxClipRange(&ucg); }

    void drawPixel(ucg_int_t x, ucg_int_t y) { ucg_DrawPixel(&ucg, x, y); }    
    void drawHLine(ucg_int_t x, ucg_int_t y, ucg_int_t len) { ucg_DrawHLine(&ucg, x, y, len); }
    void drawVLine(ucg_int_t x, ucg_int_t y, ucg_int_t len) { ucg_DrawVLine(&ucg, x, y, len); }
    void drawLine(ucg_int_t x1, ucg_int_t y1, ucg_int_t x2, ucg_int_t y2) { ucg_DrawLine(&ucg, x1, y1, x2, y2); }

    ucg_int_t drawGlyph(ucg_int_t x, ucg_int_t y, uint8_t dir, uint8_t encoding) { return ucg_DrawGlyph(&ucg, x, y, dir, encoding); }
    ucg_int_t drawString(ucg_int_t x, ucg_int_t y, uint8_t dir, const char *str) { return ucg_DrawString(&ucg, x, y, dir, str); }
    
    void drawBox(ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h) { ucg_DrawBox(&ucg, x, y, w, h); }
    void clearScreen(void) { ucg_ClearScreen(&ucg); }
    void drawRBox(ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h, ucg_int_t r) { ucg_DrawRBox(&ucg, x, y, w, h, r); }
    
    void drawFrame(ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h) { ucg_DrawFrame(&ucg, x, y, w, h); }
    void drawRFrame(ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h, ucg_int_t r) { ucg_DrawRFrame(&ucg, x, y, w, h, r); }
 
    void drawDisc(ucg_int_t x0, ucg_int_t y0, ucg_int_t rad, uint8_t option) { ucg_DrawDisc(&ucg, x0, y0, rad, option); }
    void drawCircle(ucg_int_t x0, ucg_int_t y0, ucg_int_t rad, uint8_t option) { ucg_DrawCircle(&ucg, x0, y0, rad, option); }

    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) { ucg_DrawTriangle(&ucg, x0, y0, x1, y1, x2, y2); }
    /* the polygon procedure only works for convex tetragons (http://en.wikipedia.org/wiki/Convex_polygon) */
    void drawTetragon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3) { ucg_DrawTetragon(&ucg, x0, y0, x1, y1, x2, y2, x3, y3); }
    
    // Procedures, which are only available with the EXTENDED option
    
    void drawGradientLine(ucg_int_t x, ucg_int_t y, ucg_int_t len, ucg_int_t dir) {ucg_DrawGradientLine(&ucg, x, y, len, dir); }
    void drawGradientBox(ucg_int_t x, ucg_int_t y, ucg_int_t w, ucg_int_t h) { ucg_DrawGradientBox(&ucg, x, y, w, h); }
};


class Ucglib4WireHWSPI : public Ucglib
{
  public:
    Ucglib4WireHWSPI(ucg_dev_fnptr dev, ucg_dev_fnptr ext, uint8_t cd, uint8_t cs = UCG_PIN_VAL_NONE, uint8_t reset = UCG_PIN_VAL_NONE)
      { init(); dev_cb = dev; ext_cb = ext; 

	ucg.pin_list[UCG_PIN_RST] = reset; 
	ucg.pin_list[UCG_PIN_CD] = cd;
	ucg.pin_list[UCG_PIN_CS] = cs; 

#ifdef __AVR__	
	  ucg.data_port[UCG_PIN_RST] =  portOutputRegister(digitalPinToPort(reset));
	  ucg.data_mask[UCG_PIN_RST] =  digitalPinToBitMask(reset);
	  ucg.data_port[UCG_PIN_CD] =  portOutputRegister(digitalPinToPort(cd));
	  ucg.data_mask[UCG_PIN_CD] =  digitalPinToBitMask(cd);
	  ucg.data_port[UCG_PIN_CS] =  portOutputRegister(digitalPinToPort(cs));
	  ucg.data_mask[UCG_PIN_CS] =  digitalPinToBitMask(cs);
#endif
    }
    void begin(uint8_t is_transparent);
};

ucg_int_t ucg_dev_st7735_18x128x128(ucg_t *ucg, ucg_int_t msg, void *data);

class Ucglib_ST7735_18x128x128_HWSPI : public Ucglib4WireHWSPI
{
  public:
  Ucglib_ST7735_18x128x128_HWSPI( uint8_t cd, uint8_t cs = UCG_PIN_VAL_NONE, uint8_t reset = UCG_PIN_VAL_NONE) : 
    Ucglib4WireHWSPI(ucg_dev_st7735_18x128x128, ucg_ext_st7735_18, /*cd=*/ cd , /*cs=*/ cs, /*reset=*/ reset)
    { }
};