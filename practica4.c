
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

//Metasprite de 3x3 normal
#define DEF_METASPRITE_3x3(name,code,pal)\
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
//Metasprite de 3x3 invertido horizontalmente
#define DEF_METASPRITE_3x3_FLIP(name,code,pal)\
const unsigned char name[]={\
        16,      0,      (code)+0,  (pal)|OAM_FLIP_H, \
        8,      0,      (code)+1,   (pal)|OAM_FLIP_H, \
        0,      0,      (code)+2,   (pal)|OAM_FLIP_H, \
        16,      8,      (code)+3,   (pal)|OAM_FLIP_H, \
        8,      8,      (code)+4,   (pal)|OAM_FLIP_H, \
        0,      8,      (code)+5,   (pal)|OAM_FLIP_H,\
        16,      16,      (code)+6,   (pal)|OAM_FLIP_H,\
        8,      16,      (code)+7,   (pal)|OAM_FLIP_H, \
        0,      16,      (code)+8,   (pal)|OAM_FLIP_H, \
        128};
//Definicion de metasprites 
DEF_METASPRITE_3x3(playerEstatico, 0x80, 0);
DEF_METASPRITE_3x3(playerSalto, 0xad, 0);
DEF_METASPRITE_3x3(playerCamina1, 0x89, 0);
DEF_METASPRITE_3x3(playerCamina2, 0x92, 0);
DEF_METASPRITE_3x3(playerCamina3, 0x9b, 0);
DEF_METASPRITE_3x3(playerCamina4, 0xa4, 0);
//Definicion de metasprites invertidos
DEF_METASPRITE_3x3_FLIP(playerEstaticoL, 0x80, 0);
DEF_METASPRITE_3x3_FLIP(playerSaltoL, 0xad, 0);
DEF_METASPRITE_3x3_FLIP(playerCaminaL1, 0x89, 0);
DEF_METASPRITE_3x3_FLIP(playerCaminaL2, 0x92, 0);
DEF_METASPRITE_3x3_FLIP(playerCaminaL3, 0x9b, 0);
DEF_METASPRITE_3x3_FLIP(playerCaminaL4, 0xa4, 0);
//Secuencia del personaje en movimiento
const unsigned char* const playerSec[16] = {
  playerCaminaL1, playerCaminaL2, playerCaminaL3, 
  playerCaminaL4, playerCaminaL1, playerCaminaL2, 
  playerCaminaL3, playerCaminaL4,
  
  playerCamina1, playerCamina2, playerCamina3, 
  playerCamina4, playerCamina1, playerCamina2, 
  playerCamina3, playerCamina4,
};

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

void setup_graphics() {
  oam_clear();
  pal_all(PALETTE);
}
//Definición del total de personajes a tener
#define PERSONAJES 2 
// Arreglo de posición del personaje en X y Y
byte john_x[PERSONAJES];
byte john_y[PERSONAJES];
// Arreglo de deltas del actor (movimiento)
sbyte john_dx[PERSONAJES];
sbyte john_dy[PERSONAJES];
//Orientación/dirección del personaje
byte dir[PERSONAJES];

//////////////
void main(void)
{
  char oam_id;	// id del objeto en oam
  char pad;	// Bandera del control
  char i;	// Identificador 
  byte runseq=0;// Atributo para realizar secuencia de movimiento
  
  setup_graphics();
  vram_adr(NTADR_A(2,2));
  vram_write("HELLO, WORLD!", 12);
  ppu_on_all();
  
  for(i=0;i<PERSONAJES;++i){
    john_x[i]=10*i + 150;
    john_y[i]=30*i + 100;
    
    john_dx[i]=0;
    john_dy[i]=0;
    
  }
  
  while(1) {
    oam_id=0;
    //Ciclo para identificar los dos controles
    for(i=0;i<2;i++){
      pad = pad_poll(i); // i=0 para personaje 1; i=1 para personaje 2
      //Va a la izquierda antes de llegar al límite?
      if (pad&PAD_LEFT && john_x[i]>6)
      {
        john_dx[i]=-2;
        dir[i] = 1;
      }
      //Va a la derecha antes de llegar al límite?
      else if (pad&PAD_RIGHT && john_x[i]<225)
      {
        john_dx[i]=2;
        dir[i]=0;
      }
      //no se mueve en el eje x
      else 
      {
        john_dx[i]=0;
      }
      //Va hacia arriba antes de llegar al límite?
      if (pad&PAD_UP && john_y[i]>7)
      {
        john_dy[i]=-2;
      }
      //Va hacia abajo antes de llegar al límite?
      else if (pad&PAD_DOWN && john_y[i]<200)
      {
        john_dy[i]=2;
      }
      //no se mueve en el eje y
     else 
      {
        john_dy[i]=0;
      }
    }
    
    // Imprimir personajes
    for(i=0;i<PERSONAJES;i++){
      char sec; 
      char caraX = 0;
      char caraY = 0;
      ++runseq;
      
      if(runseq>=80) 
        runseq = 0;
      if(dir[i]==0) //Frames en direccion a la derecha
  	sec = (runseq/10)+8;
      else  //Frames en dirección a la izquierda
        sec = (runseq/10);
      // Si el personaje no tiene movimiento
      if(john_dx[i]==0&&john_dy[i]==0){
        // Si mira a la derecha
        if(dir[i]==0){
          // Imprime la cara del personaje para la posición estática
          caraX=9;
          caraY=4;
          oam_id=oam_meta_spr(john_x[i], john_y[i] , oam_id,playerEstatico);
          oam_id = oam_spr(john_x[i]+caraX,  john_y[i] +caraY, 0xCF,1, oam_id);
        }
        // Si mira a la izquierda
        else{
          caraX=7;
          caraY=4;
          oam_id=oam_meta_spr(john_x[i],  john_y[i] , oam_id,playerEstaticoL);
          oam_id = oam_spr(john_x[i]+caraX,  john_y[i] +caraY, 0xCF,1|OAM_FLIP_H, oam_id);
        }
      }
      //Si hay movimiento
      else {
      oam_id=oam_meta_spr(john_x[i],  john_y[i] , oam_id,playerSec[sec]);
        
      //PARA LA CARA DEL PERSONAJE  
      if(dir[i]==0){ //Lado derecho
        switch(sec){
                                        
          case 8: caraX=12; caraY=6;
            break;
          case 9: caraX=11; caraY=5;
            break;
          case 10: caraX=12; caraY=5;
            break;
          case 11: caraX=11; caraY=5;
            break;
          case 12: caraX=12; caraY=6;
            break;
          case 13: caraX=11; caraY=5;
            break;
          case 14: caraX=12; caraY=5;
            break;
          case 15: caraX=11; caraY=5;
            break;
        }
      }
      else{ //Lado izquierdo
        switch(sec){
          case 0: caraX=4; caraY=6;
            break;
          case 1: caraX=5; caraY=5;
            break;
          case 2: caraX=4; caraY=5;
            break;
          case 3: caraX=5; caraY=5;
            break;
          case 4: caraX=4; caraY=6;
            break;
          case 5: caraX=5; caraY=5;
            break;
          case 6: caraX=4; caraY=5;
            break;
          case 7: caraX=5; caraY=5;
            break;
        }
      }
        // Imprime cara del personaje
        if(dir[i]==0)
          oam_id = oam_spr(john_x[i]+caraX, john_y[i] +caraY, 0xCF,1, oam_id);
        else
          oam_id = oam_spr(john_x[i]+caraX,  john_y[i] +caraY, 0xCF,1|OAM_FLIP_H, oam_id);
          
      }
      //
      john_x[i] += john_dx[i];
      john_y[i] += john_dy[i];
      
    }
    
    ppu_wait_frame();
    //ocultar todos los sprites restantes del desplazamiento
    if (oam_id!=0)oam_hide_rest(oam_off); 
  }
}
