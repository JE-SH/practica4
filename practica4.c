
//#resource "Sprites.chr"

#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+0,   pal, \
        8,      0,      (code)+1,   pal, \
        16,      0,      (code)+2,   pal, \
        0,      8,      (code)+3,   pal, \
        8,      8,      (code)+4,   pal, \
        16,      8,      (code)+5,   pal, \
        0,      16,      (code)+6,   pal, \
        8,      16,      (code)+7,   pal, \
        16,      16,      (code)+8,   pal, \
        128};


DEF_METASPRITE_2x2(playerEstatico, 0x80, 0);
DEF_METASPRITE_2x2(playerSalto, 0xad, 0);
DEF_METASPRITE_2x2(playerCamina1, 0x89, 0);
DEF_METASPRITE_2x2(playerCamina2, 0x92, 0);
DEF_METASPRITE_2x2(playerCamina3, 0x9b, 0);
DEF_METASPRITE_2x2(playerCamina4, 0xa4, 0);


/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x3,			// Color de la pantalla

  0x07,0x17,0x37,0x30,	// Paleta de fondo 0
  0x0F,0x16,0x37,0x00,	// Paleta de fondo 1
  0x2D,0x31,0x30,0x00,	// Paleta de fondo 2
  0x0B,0x1B,0x2B,0x00,  // Paleta de fondo 3

  0x0F,0x36,0x2A,0xFF,	// paleta de sprite 0
  0x2B,0x30,0x30,0x00,	// paleta de sprite 1
  0x30,0x30,0x30,0x00,	// paleta de sprite 2
  0x0D,0x27,0x2A	// paleta de sprite 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

void main(void)
{
  char oam_id;
  setup_graphics();
  // draw message  
  vram_adr(NTADR_A(2,2));
  vram_write("HELLO, WORLD!", 12);
  // enable rendering
  ppu_on_all();
  // infinite loop
  
  
  while(1) {
    oam_id=0;
    oam_meta_spr_pal(132, 132, 0,playerSalto);
    oam_id = oam_spr(132, 132, 0xCF,1, oam_id);
    
   ppu_wait_nmi();
  }
}
