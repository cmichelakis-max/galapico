/* scramble.h */
/* TODO LIST 
1. Graphics issues 
2. Scroll not working
3. Sound not implemented (2nd Z80 CPU)
4. No stars in background
5. DIP switches need addressing
*/ 

//https://github.com/ScottTunstall/Scramble
// some documentation on scramble game

/* Copied from MAME4All documentation: https://github.com/squidrpi/mame4all-pi/blob/master/src/drivers/scramble.cpp 

 
   MAIN BOARD:
0000-3fff ROM
4000-47ff RAM
4800-4fff Character RAM  (Yes, $4fff, mame4all docs are incorrect)
5000-50ff Object RAM
    5000-503f  screen attributes
    5040-505f  sprites
    5060-507f  bullets
    5080-50ff  RAM

read:
7000      Watchdog Reset (Scramble)
8100      IN0
          PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
          PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
          PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
          PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
          PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
          PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
          PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
          PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

8101      IN1
          PORT_DIPNAME( 0x03, 0x00, "Lives") PORT_DIPLOCATION("SW1:2,1")
          PORT_DIPSETTING(    0x00, "3" )
          PORT_DIPSETTING(    0x01, "4" )
          PORT_DIPSETTING(    0x02, "5" )
          PORT_DIPSETTING(    0x03, DEF_STR( Free_Play ) )
          PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
          PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
          PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
          PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
          PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
          PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

8102      IN2 
          PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL

          ; Some examples of Coinage settings and explanation of purpose:
          ; "A 1/1 B 2/1 C 1/1" means "Slot A: ONE coin / ONE credit.  Slot B: TWO coins / ONE credit.  Slot C (SERVICE button) : ONE press / ONE credit."
          ; "A 1/2 B 1/1 C 1/2" means "Slot A: ONE coin / TWO credits.  Slot B: ONE coin / ONE credit.  Slot C (SERVICE button) : ONE press / TWO credits."
          PORT_DIPNAME( 0x06, 0x00, "Coinage" PORT_DIPLOCATION("SW1:5,4")
          PORT_DIPSETTING(    0x00, "Coinage A 1/1 B 2/1 C 1/1")
          PORT_DIPSETTING(    0x02, "Coinage A 1/2 B 1/1 C 1/2")
          PORT_DIPSETTING(    0x04, "Coinage A 1/3 B 3/1 C 1/3" )
          PORT_DIPSETTING(    0x06, "Coinage A 1/4 B 4/1 C 1/4" )
          
          PORT_DIPNAME( 0x08, 0x00, "Cabinet") PORT_DIPLOCATION("SW1:3")
          PORT_DIPSETTING(    0x00, "Upright")
          PORT_DIPSETTING(    0x08, "Cocktail")

          PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
          PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_CUSTOM )     protection bit 
          PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
          PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM )    protection bit 


write:
6801      interrupt enable
6802      coin counter
6803      POUT1 - affects screen background colour (0=black, 1 = blue) - thanks to Phil Murray for telling me about this
6804      stars on
6806      screen vertical flip
6807      screen horizontal flip
8200      To AY-3-8910 port A (commands for the audio CPU)
8201      bit 3 = interrupt trigger on audio CPU  bit 4 = disable sound (see scramble_state::scramble_sh_irqtrigger_w in MAME's scramble audio driver)
8202      protection check bits

SOUND BOARD:
0000-1fff ROM
8000-83ff RAM


I/O ports:
read:
20      8910 #2  read
80      8910 #1  read
write
10      8910 #2  control20      8910 #2  write
40      8910 #1  control
80      8910 #1  write
*/

extern unsigned char scramblk_protection_r(void);

//#define SCROLL_ON

#ifdef CPU_EMULATION
#include "scramble_rom.h"

static inline unsigned char scramble_RdZ80(unsigned short Addr) {
		
	// This causes white squares to work
	// taking out causes issues
	#if 0
	Addr &= 0x7fff;   // a15 is unused
	#endif
	if (Addr < 16384)
		return scramble_rom[Addr];


    // easier ranges
    if ((Addr >= 0x4000)  && (Addr <= 0x47ff))
	{
		return memory[Addr - 0x4000];
	}
	
	
    if ((Addr >= 0x5000)  && (Addr <= 0x50ff))
	{
		return memory[Addr - 0x4000];
	}
	
	if (Addr == 0x8100)
	{
			// get a mask of currently pressed keys
		    unsigned char keymask = buttons_get();
			unsigned char retval = 0xff;
		
			if (keymask & BUTTON_LEFT) 
			{
			   retval &= ~0x20;
			}   
			
			if (keymask & BUTTON_RIGHT)
			{
			   retval &= ~0x10;
			}  
		
		    if (keymask & BUTTON_FIRE)
		    {  
			   
		       retval &= ~0x08;
		    }   
		    if (keymask & BUTTON_EXTRA) // This drops the bombs
		    {
			    	 
		        retval &= ~0x02;
		    }
			
			if (keymask & BUTTON_COIN) 
			{
			   retval &= ~0x80;
			}  
			
			return retval;
	}


	if (Addr == 0x8101) 
	{
			// get a mask of currently pressed keys
		    unsigned char keymask = buttons_get();
			unsigned char retval = 0xfd; // 0xef for service --- 3 lives xxxxxx00
			if (keymask & BUTTON_START)  retval &= ~0x80;
			return retval;
	}
					
	if (Addr == 0x8102) 
	{
			// get a mask of currently pressed keys
		    unsigned char keymask = buttons_get();
			unsigned char retval = 0xff; // 0xef for service
			if (keymask & BUTTON_DOWN)  retval &= ~0x40;
			if (keymask & BUTTON_UP)    retval &= ~0x10;
			return retval;
	}
		
    // Protection port	
	if ((Addr >= 0x8200) && (Addr <= 0x8203))
	{
			
			unsigned char retval = 0xff; // 0xef for service
			retval =scramblk_protection_r(); 
			return retval;
		}
	
   

	return 0xff;
}

static inline void scramble_WrZ80(unsigned short Addr, unsigned char Value) {
	Addr &= 0x7fff;   // a15 is unused

    
	if ((Addr & 0xf000) == 0x4000) 
	{	
	    
		memory[Addr - 0x4000] = Value; 
		return;
	}
	
	if ((Addr & 0xf000) == 0x4800)
	{   
		memory[Addr - 0x4000] = Value; 
		return;		
	}	

    
	if ((Addr & 0xff00) == 0x5000) 
	{
		if ((Addr & 0xfff0) == 0x5000)
		{			 
			memory[Addr - 0x4000] = Value; 
		}
		
		if ((Addr & 0xfff0) == 0x5010)
		{			 
			memory[Addr - 0x4000] = Value; 
		} 
		
		if ((Addr & 0xfff0) == 0x5020)
		{			 
			memory[Addr - 0x4000] = Value; 
		}  	
		
		if ((Addr & 0xfff0) == 0x5030)
		{			 
			memory[Addr - 0x4000] = Value; 
		} 
		
		if ((Addr & 0xfff0) == 0x5040)
		{			 
			memory[Addr - 0x4000] = Value; 
		} 	
		
		if ((Addr & 0xfff0) == 0x5050)
		{
			 
			memory[Addr - 0x4000] = Value; 
		}
			
		if ((Addr & 0xfff0) == 0x5060)
			memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x5070)
			memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x5080)
			memory[Addr - 0x4000] = Value; 	
		if ((Addr & 0xfff0) == 0x5090)
			memory[Addr - 0x4000] = Value; 		
		if ((Addr & 0xfff0) == 0x50A0)
		    memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x50B0)
		    memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x50C0)
		    memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x50D0)
		    memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x50E0)
		    memory[Addr - 0x4000] = Value; 
		if ((Addr & 0xfff0) == 0x50F0)
		    memory[Addr - 0x4000] = Value;      
		               
		return;    	
    }    
        
   
	if (Addr == 0x6801)
	{		
		irq_enable[0] = Value & 1;
	}

	return;	

	// initial stack is at 0xf000 (0x6000 with a15 ignored), so catch that
	//if((Addr & 0xfffe) == 0x6ffe)
	//  return;
}

static inline void scramble_OutZ80(unsigned short Port, unsigned char Value) {
	irq_ptr = Value;
}


static inline void scramble_run_frame(void) {
		
	
	
	for (int i = 0; i < INST_PER_FRAME_SCRAMBLE; i++) {
		StepZ80(cpu); StepZ80(cpu); StepZ80(cpu); StepZ80(cpu);
	}
    
	if (irq_enable[0])
	{

	    IntZ80(cpu,INT_NMI );
	}  
		
}
#endif // CPU_EMULATION

#ifdef IO_EMULATION

#ifndef SINGLE_MACHINE
#include "scramble_logo.h"
#endif
#include "scramble_tilemap.h"
#include "scramble_spritemap.h"
#include "scramble_cmap.h"
#ifdef SOUND // TODO 
#include "scramble_wavetable.h"
#endif

#define USE_NAMCO_WAVETABLE

#define CHUNKSIZE (8)

// In code
/* i
Structure of sprite
struct sprite_S {
    unsigned char code, color, flags;
    short x, y;
};
*/

/* Actually from docs
; represents a hardware sprite
struct SPRITE
{
   BYTE Y;                          ; Y coordinate
   BYTE Code;                       ; Determines animation frame                
   BYTE Colour;                     ; Core (individual) colour of sprite - other colours shared between all sprites 
   BYTE X;                          ; X coordinate
} - sizeof(SPRITE) is 4 bytes
*/ 

static inline void scramble_prepare_frame(void) {
	// Do all the preparations to render a screen.

	/* preprocess sprites */
	active_sprites = 0;
	
	for (int idx = 0; idx < 8 && active_sprites < 92; idx++) 
	
	   {
	     
		 unsigned char* sprite_base_ptr = memory+0x1040 + 4 * idx; // video RAM PTR (0x5040-0x4000) - see memory map
       
        // donkey kong code
        // adjust sprite position on screen for upright screen
		struct sprite_S spr;
		spr.x = sprite_base_ptr[0] - 23;
		spr.y = sprite_base_ptr[3] + 8;   
		
			
		spr.code = sprite_base_ptr[1] & 0x7f;
	#if 0	
		spr.color = sprite_base_ptr[2] & 0x0f;
	#endif
	// Try this 
	    spr.color = sprite_base_ptr[2] & 0x07;	
		spr.flags = ((sprite_base_ptr[2] & 0x80) ? 1 : 0) |
			((sprite_base_ptr[1] & 0x80) ? 2 : 0);

		// save sprite in list of active sprites
		if ((spr.y > -16) && (spr.y < 288) &&
			(spr.x > -16) && (spr.x < 224))
			sprite[active_sprites++] = spr;
    		
	}
	
}

#ifndef SCROLL_ON
void scramble_render_tile_raster(int chunk)
{

	int column = chunk;	
	
	// A row is actually a column for a horizontal raster beam
	//for (int row = 0; row < (TV_WIDTH / 8) - 4; row++)
	for (int row = 0; row < 36; row++)
	{       
		 unsigned short addr = tileaddr[row][(TV_HEIGHT / CHUNKSIZE) - column - 1];
		
		//  this must be memory[addr+0x800] OFFSET -- VERY VERY IMPORTANT AS THIS IS VIDEO RAM 
	 	
		const unsigned short* tile = c2_5f[memory[addr+0x800]]; // from scramble_tilemap.h 		
	
	   	
    
	    // TODO work out scroll
	    // Addres is 0x5000-0x4000 SEE MEMORY MAP
	    int c = memory[(0x1000) + 2 * (addr & 31) + 1] & 7;	    
	    
		const unsigned short* colors = scramble_colormap[c];    // from scramble_cmap.h
		
		unsigned short* ptr = frame_buffer + CRT_ROW_OFFSET + 8 * (row + (TV_WIDTH * column)); 

		// 8 pixel rows per tile
		for (char r = 0; r < 8; r++, ptr += (TV_WIDTH - 8))
		{
	        		
			unsigned short pix = *tile++;		 
		    	
			// 8 pixel columns per tile
			for (char c = 0; c < 8; c++, pix >>= 2)
			{
				if (pix & 3)
				{				
					*ptr = colors[pix & 3];
				}	
				  	
				ptr++;
			}
		}		
	}
	
}
#else
static inline void scramble_render_tile_raster(unsigned short chunk)
{

    int col = chunk;    
    
    // don't render lines 0, 1, 34 and 35
    //if (row <= 1 || row >= 34) return;

    // A row is actually a column for a horizontal raster beam 
    for (int row = 0; row < GAME_WIDTH/8 -2 ; row++)
    {        
       
		//get scroll info for this row
        // Addres is 0x5000-0x4000 SEE MEMORY MAP		  
        unsigned char scroll = memory[0x1000 + 2 * (row - 2)];        
        scroll = ((scroll << 4) & 0xf0) | ((scroll >> 4) & 0x0f);        
                
        if (scroll == 0) // no scroll in this line?
        {			 
            
            if ((row > 2) || (row < 34))
            {
                unsigned short addr = tileaddr[row][(TV_HEIGHT / CHUNKSIZE) - col - 1];

              
                const unsigned short* tile = c2_5f[memory[0x0800 + addr]]; // from scramble_tilemap.h
                          
                
				// Addres is 0x5000-0x4000 SEE MEMORY MAP                
                int c = memory[0x1000 + 2 * (addr & 31) + 1] & 7;
                const unsigned short* colors = scramble_colormap[c];
                
                unsigned short* ptr = frame_buffer+CRT_ROW_OFFSET+ 8 * (row + (TV_WIDTH * col)); // was column

                // 8 pixel rows per tile
                for (char r = 0; r < 8; r++, ptr += (TV_WIDTH - 8)) 
                {
                    unsigned short pix = *tile++;
                    // 8 pixel columns per tile
                    for (char c = 0; c < 8; c++, pix >>= 2) 
                    {
                        if (pix & 3) *ptr = colors[pix & 3];
                        ptr++;
                    }
                }
            }

        }
        else
        {        
            if ((row > 2) || (row < 34))
            {

                unsigned short addr = 0; // Shut up, compiler
                unsigned short mask = 0xffff;
                int sub = scroll & 0x07;             
               
             
                if (col < (TV_HEIGHT / CHUNKSIZE) -1 ) 
                {
                    addr = tileaddr[row][(TV_HEIGHT / CHUNKSIZE) - col -1];                 
                    

                    //JB one tile (8 pixels) further is an address offset of 32
                    addr = (addr + ((scroll & ~7) << 2)) & 1023;
                   
                }
                else 
                {
                    // negative column is a special case for the leftmost
                    // tile when it's only partly visible
                    addr = tileaddr[row][(TV_HEIGHT / CHUNKSIZE) - col -1];
                    addr = (addr + 32 + ((scroll & ~7) << 2)) & 1023; 
                   
                }

				// Addres is 0x4800-0x4000 SEE MEMORY MAP  (Video RAM)
                const unsigned char chr = memory[0x0800 + addr];
                
                const unsigned short* tile = c2_5f[chr]; // from scramble_tilemap.h
                

                // Addres is 0x5000-0x4000 SEE MEMORY MAP                
                int c = memory[0x1000 + 2 * (addr & 31) + 1] & 7;
                const unsigned short* colors = scramble_colormap[c];            
              
                unsigned short* ptr = frame_buffer +(-TV_WIDTH*(sub))+8 * (row+(TV_WIDTH * col)); // this ooes not go past  white squares (from frogger.h) 
				
				 
                // 8 pixel rows per tile
                for (char r = 0; r < 8; r++, ptr += (TV_WIDTH - 8)) 
                {
                    unsigned short pix = *tile++ ;//&mask;
                    // 8 pixel columns per tile
                    for (char c = 0; c < 8; c++, pix >>= 2) 
                    {
                        if (pix & 3) *ptr = colors[pix & 3];
                        ptr++;
                    }
                }
            }
        }
    }
}
#endif


static inline void scramble_render_sprite_raster(unsigned short chunk)
{

	// boundaries for sprites to be on this chunk
	int upper_sprite_bound = CHUNKSIZE * (chunk + 1);
	int lower_sprite_bound = CHUNKSIZE * chunk;
	int chunk_vert_start = CHUNKSIZE * chunk;
 
	for (unsigned char s = 0; s < active_sprites; s++)
	{
		int x_vert = 224 - sprite[s].x - 16;

		// check if sprite is visible on this row
		if ((x_vert < upper_sprite_bound) && ((x_vert + 16) > lower_sprite_bound))
		{
          
			const unsigned long* spr = scramble_sprites[sprite[s].flags & 3][sprite[s].code]; // from scramble_spritemap.h - flags derived from sprite_base_ptr[2] and sprite_base_ptr[1]		
									
			const unsigned short *colors = scramble_colormap[sprite[s].color]; // from scramble_cmap.h 
			
			short sprite_chunk_offset = x_vert - chunk_vert_start;

			int lines2draw = CHUNKSIZE - sprite_chunk_offset;
			if (lines2draw > 16)
				lines2draw = 16;

			// If we're onto the next line for the sprite...
			if (sprite_chunk_offset < 0)
			{
				lines2draw = 16 + sprite_chunk_offset;
			}

			// check which sprite line to begin with
			unsigned short startline = 0;
			if (sprite_chunk_offset < 0)
			{
				startline = -sprite_chunk_offset;
			}

			spr += startline;
			// calculate pixel lines to paint
			int scanline_start = chunk_vert_start;
			if (sprite_chunk_offset >= 0)
				scanline_start = (chunk * CHUNKSIZE) + sprite_chunk_offset; // i.e. sprite[].x

			unsigned short* ptr = frame_buffer + CRT_ROW_OFFSET + ((scanline_start)*TV_WIDTH) + sprite[s].y;

			for (char r = 0; r < lines2draw; r++, ptr += (TV_WIDTH - 16))
			{
				unsigned long pix = *spr++;
				// 16 pixel columns per tile
				for (char c = 0; c < 16; c++, pix >>= 2)
				{
					unsigned short col = colors[pix & 3];
					if (pix & 3)
						*ptr = col;
					ptr++;
				}
			}
		}
	}		
	
}

struct scramble_bullet { unsigned char nu1, x,nu2, y; };

static void scramble_render_bullets_raster()
{
	unsigned short x ;
	unsigned short y ;	
	 
    for (int row = 0; row < GAME_WIDTH / 8; row++)
	{		
		for (char bullet_cntr = 0; bullet_cntr < 4*8; bullet_cntr+=4) // 8 bullets
		{
			struct scramble_bullet *b;
			
			unsigned char* bullet_base_ptr = memory+0x1060 + bullet_cntr; // video RAM PTR (0x5060-0x4000) - see memory map
		    b = (scramble_bullet *)bullet_base_ptr; 

			x =  224- b->x; //try 224 subtract
			y =  224 - b->y +16 - row * 8;             
        
			if (y < 8 && x < 224)
			{				
				frame_buffer[y + 8 * row + (x * TV_WIDTH)+(CRT_ROW_OFFSET*2)-8] = 5; //*colours;
			}	
			
		}	
		
	}
	
}

static inline void scramble_render_frame_raster()
{

	// Raster beam scan, divided into sprite-sized chunks (x axis)
	// Means I can ditch the framebuffer(s) if RAM gets low
	// X and Y are in vertical monitor-world, so remember y is horizontal beam and x is vertical (aargh)
	// Real hardware does this line by line, but checking dozens of sprites on each scanline isn't efficient or necessary

	frame_buffer = &nextframeptr[0];
	//Sprites
	// dkong has its own sprite drawing routine since unlike the other
	// games, in dkong black is not always transparent. Black pixels
	// are instead used for masking

	for (int chunk = 0; chunk < TV_HEIGHT / CHUNKSIZE; chunk++)
	{
		unsigned short* store = &nextframeptr[chunk * CHUNKSIZE * TV_WIDTH];

		for (int x = 0; x < TV_WIDTH * CHUNKSIZE; x++)
		{
			store[x] = 0;
		}		
	
		scramble_render_tile_raster(chunk);		
		scramble_render_sprite_raster(chunk);		
		scramble_render_bullets_raster();		
		
	}	
	
}

#endif // IO_EMULATION




