//INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>

//MACROS
#define HI_BYTE(n)  (((int)n>>8) & 0x00ff)  // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int)n & 0x00ff)       // extracts the low-byte of a word

#define HIGH(n)     ((int)n<<8)             // turn the char into high part of int

//STRUCTURES

typedef struct RGB_color_typ
	{
	unsigned char red;      //Red component if color 0-63
	unsigned char green;    //Green component of color 0-63
	unsigned char blue;     //Blue component of color 0-63
	} RGB_color, *RGB_color_ptr;
	
typedef struct pcx_header_typ
	{
	char manufacturer;
		// Always 10.
	char version;
		// 0-Ver 2.5 Paintbrush, 2-Ver 2.8 with
		// palette, 3-Ver 2.8 use the default palette,
		// 5-Ver 3.0 or higher of Paintbrush
	char encoding;
		// Always 1, meaning RLE encoding.
	char bits_per_pixel;
		// Bits per pixel; in our case, eight
	short x,y;
		// Upper-left corner of the image
	short width, height;
		// Size of the image
	short horv_res;
		// Pixels in the x direction
	short vert_res;
		// Pixels in the y direction
	char ega_palette[48];
		// The EGA palette; we can ignore it
	char reserved;
		// Nothing
	char num_color_planes;
		// The number of planes in the image
	short bytes_per_line;
		// Bytes per one horizontal line
	short palette_type;
		// 1 = Color or B&W   
		// 2 = Grayscale		
	char padding[58];
		// Extra bytes for a rainy day
	} pcx_header, *pcx_header_ptr;
	
typedef struct pcx_picture_typ
	{
	pcx_header header;
		// The header
	RGB_color palette[256];
		// The VGA palette
	unsigned char *buffer;
		// The buffer to hold the image
	} pcx_picture, *pcx_picture_ptr;
	

//// F U N C T I O N S //////////////////////////////////////////////////////////


int PCX_Load(char *filename, pcx_picture_ptr image)
{
	// this function loads a pcx file into a picture structure, the actual image
	// data for the pcx file is decompressed and expanded into a secondary buffer
	// within the picture structure, the separate images can be grabbed from this
	// buffer later.  also the header and palette are loaded

	long num_bytes,index;
	long count;
	long x,y;
	unsigned char data;
	pcx_header *header;
	FILE *fp;

	// open the file
	fp = fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("\nERROR: Can't open file [%s]",filename);
		return 0;
	}

	// load the header
	header = &image->header;
	fread(header, 1, 128, fp);

	header->width++;
	header->height++;

	// check to make sure this is a 256 color PCX
	if( (header->manufacturer != 10) ||
		(header->encoding != 1) ||
		(header->bits_per_pixel != 8) ||
		(header->num_color_planes != 1) ||
		(header->palette_type != 1) )
	{
		printf("\nERROR: File [%s] is not recognized as a 256 color PCX.",filename);
		fclose(fp);
		return 0;
	}
	
	//allocate memory for the picture + 64 empty lines
	image->buffer = malloc( (size_t)(header->height+64)*header->width );
	if(image->buffer == NULL)
	{
		printf("\nERROR: Can't allocate enough memory for the picture.");
		fclose(fp);
		return 0;
	}

	//initally clear the memory (to make those extra lines be blank)
	memset(image->buffer,0,(size_t)(header->height+64)*header->width);	

	// load the data and decompress into buffer
	count=0;
	for(y=0; y < header->height; y++)
	{
		for(x=0; x < header->width; x++)
		{
			// get the first piece of data
			data = getc(fp);

			// is this a rle?
			if (data>=192 && data<=255)
			{
				// how many bytes in run?
				num_bytes = data-192;
				x += (num_bytes-1);
	
				// get the actual data for the run
				data = getc(fp);

				// replicate data in buffer num_bytes times
				while( num_bytes-- > 0)
					image->buffer[count++] = data;

			} // end if rle
			else
		    {
				// actual data, just copy it into buffer at next location
				image->buffer[count++] = data;
			} // end else not rle

		} //end of x loop
	
		//get rid of the padding byte if there is one
		if( x < header->bytes_per_line)
			data = getc(fp); 

	} //end of y loop

	//Get the Palette header (one byte, should equal 12)
	data = getc(fp);
	if(data != 12)
	{
		printf("\nERROR: Couldn't find palette header [%s]",filename);
		free(image->buffer);
		fclose(fp);

		//fp=fopen("fail.pcx","wb");
		//fwrite(image->buffer,(header->width)*(header->height),1,fp);
		//fclose(fp);
		
		return 0;
	}

	//get the pallete data 
	for (index=0; index<256; index++)
    {
	    image->palette[index].red   = (getc(fp) >> 2);
	    image->palette[index].green = (getc(fp) >> 2);
	    image->palette[index].blue  = (getc(fp) >> 2);
    }

	//check to make sure there weren't errors while reading the file
	if(ferror(fp))
	{
		printf("\nERROR: Error reading file [%s]",filename);
		free(image->buffer);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	return -1;

} // end PCX_Load

//////////////////////////////////////////////////////////////////////////////

void PutWord(int data, FILE *fp)
{
putc(LOW_BYTE(data),fp);
putc(HI_BYTE(data),fp);
} //end of PutWord

//////////////////////////////////////////////////////////////////////////////

unsigned char *ArrangeBlocks( unsigned char *img, int width, int height,
							 int size, int *xsize, int *ysize, int new_width, int border)
{
	/*
	** img = image buffer
	** width = width (in pixels) of image buffer 
	** height = height (in pixels) of image buffer
	**
	** size = size (in pixels) of image blocks in the image 
	** *xsize = number of image block horizontally in image block grid
	** *ysize = number of image block vertically in image block grid
	**
	** border = how wide of a border surrounds the image blocks
	** new_width = how wide (in pixels) you want the new buffer to be
	**             must be a multiple of size
	**
	**
	** returns:
	**      pointer to new buffer, and updates xsize and ysize
	**
	*/
	unsigned char *buffer;
	int rows, num;
	int i,j, line;
	int x,y;

printf("\nwidth=%d, height=%d, size=%d, *xsize=%d, *ysize=%d, new_width=%d, border=%d",
		width, height, size, *xsize, *ysize, new_width, border);

	//get number of full image block rows in the new buffer
	rows = (*xsize)*(*ysize)/(new_width/size);   // rows = num_blocks / new_xsize
	
	//if it doesn't divide evenly, add another full row
	if ( ((*xsize)*(*ysize))%(new_width/size) != 0 )
		rows++;

printf("\nrows=%d",rows);
	
	//get memory for the new buffer
	buffer = malloc( (size_t) rows*size*new_width );
	if(buffer == NULL)
		return buffer;

	//initially clear the buffer, so if there are empty image blocks
	//or incomplete blocks, the empty parts will be blank
	memset(buffer,0,(size_t) rows*size*new_width );	

	//position in new buffer (x,y) where x and y are in pixel co-ordinates
	x=0;
	y=0;
	
	//go through each image block(i,j) where i and j are in block co-ordinates
	for(j=0; j < *ysize; j++)
	for(i=0; i < *xsize; i++)
	{
		//move each line of the block into the new buffer
		//don't worry about reading past the end of the image here
		//there is an extra 64 lines to read in.
		for(line=0;line<size;line++)
		{
			//find out how much to copy
			//this is needed because the screen mode files may not be 
			//a multiple of 8 pixels wide
			//or no-border files may have the wrong width
			num = width - (i*(size+border) + border);
			if(num>size)
				num=size;

			memcpy( &buffer[ (y+line)*new_width + x ],
		     &img[ (j*(size+border) + line + border)*width + i*(size+border) + border ],
			 num);
		}

		//move to the next location in the new buffer
		x+=size;
		if(x >= new_width)
		{
			x=0;
			y+=size;
		}
	}

	*xsize = new_width/size;
	*ysize = rows;

	return buffer;
} //end of ArrangeBlocks()

//////////////////////////////////////////////////////////////////////////////

int *MakeMap(unsigned char *img, int *num_tiles, 
			 int xsize, int ysize, int tile_x, int tile_y, int colors, int rearrange)
{
	int *map;
	unsigned char blank[64];
	int tiles = *num_tiles;
	int newtiles;
	int blank_absent;
	int current;	//the current tile we're looking at
	int i,t, palette;
	int x,y;
	int x_offset, y_offset;
	
	//find x_offset,y_offset 
	//don't center
	x_offset=0;
	y_offset=0;
	
	//allocate map
	map=malloc((size_t)tile_x*tile_y*sizeof(int));
	if(map==NULL)
		return map;

	//clear map
	memset(map,0,tile_x*tile_y*sizeof(int));

	//if the palette has been rearranged... save the palette number
	if(rearrange)
	{
		current=0;
		for(y=0;y<ysize;y++)
		for(x=0;x<xsize;x++)
		{
			//get the palette number
			palette = (img[current*64] >> 4) & 0x07;
			t = (palette << 10);

			//put tile number in map
			if(tile_x==64 && tile_y==32) // 64x32 screen
			{
				if(x<32)
					map[y*32 + x]=t;
				else
					map[(y+32)*32+x-32]=t;
			}
			else if(tile_x==32 && tile_y==64) // 32x64 screen
				map[y*32+x]=t;
			else if(tile_x==64 && tile_y==64) // 64x64 screen
			{
				if(y<32)
					if(x<32)
						map[y*32+x]=t;
					else
						map[(y+32)*32+x-32]=t;
				else
					if(x<32)
						map[(y+64-32)*32+x]=t;
					else
						map[(y+96-32)*32+x-32]=t;
			}
			else //32x32 or 128x128 screen
				map[y*tile_x+x]=t;

			//goto the next tile
			current++;
		}
	
	} //end of if(rearrange)
	
	
	//truncate the colors if necessary
	if(colors != 256)
	{
		t = colors - 1;	//color truncation mask
		
		for(i=0;i<xsize*ysize*64;i++)
			img[i] = img[i] & t; 
	}

	//make a blank tile
	memset(blank,0,64);

	//I want tile #0 to be blank..
	//is it?
	if( memcmp(blank,img,64) == 0 )
	{
		blank_absent=0;
		current=1;
		t=0;
		newtiles=1;
	}
	else
	{
		blank_absent=1;
		current=1;
		t=1;
		newtiles=1;
	}

	//save the first tilemap piece
	map[0] += t;

	for(y=0;y<ysize;y++)
	for(x=0;x<xsize;x++)
	{
		//if we already processed this, move on
		if(x==0 && y==0)
			continue;

		//is the current tile blank?
		if( memcmp(blank,&img[current*64],64) == 0 )
			t=0;
		else 	
		{
			//check for matches with previous tiles
			for(i=0;i<newtiles;i++)
				if( memcmp(&img[i*64],&img[current*64],64) == 0 )
					break;

			//is it a new tile?
			if(i==newtiles) 
			{
				// yes -> add it
				memcpy(&img[newtiles*64],&img[current*64],64);
				t=newtiles+blank_absent;
				newtiles++;
			}
			else 
			{	// no -> find what tile number it is
				t=i+blank_absent;
			}
		}

	
		//put tile number in map
		if(tile_x==64 && tile_y==32) // 64x32 screen
		{
			if(x<32)
				map[y*32 + x] += t;
			else
				map[(y+32)*32+x-32] += t;
		}
		else if(tile_x==32 && tile_y==64) // 32x64 screen
			map[y*32+x] += t;
		else if(tile_x==64 && tile_y==64) // 64x64 screen
		{
			if(y<32)
				if(x<32)
					map[y*32+x] += t;
				else
					map[(y+32)*32+x-32] += t;
			else
				if(x<32)
					map[(y+64-32)*32+x] += t;
				else
					map[(y+96-32)*32+x-32] += t;
		}
		else //32x32 or 128x128 screen
			map[y*tile_x+x] += t;

		//goto the next tile
		current++;
	}

	//also return the number of new tiles
	//make it negative if we need to add the blank tile
	if (blank_absent)
		*num_tiles = -newtiles;
	else
		*num_tiles = newtiles;

	return map;

}//end of MakeMap
		
//////////////////////////////////////////////////////////////////////////////

int RearrangePalette(unsigned char *buffer, int *palette, 
					 int num_tiles, int colors)
{
	int final[8];
	int num_final;
	int *combos;//holds sorted list of colors in combo of each tile
	int *num;	//holds number of colors in each combo
	int *list;	//for sorting combos
	int n;
	
	int new_palette[256];
	int color_table[256];

	int index, last_index;
	int test, test2;
	int num_miss;
	int data;
	int i,ii;
	
	//get memory
	num=malloc(num_tiles*sizeof(int));
	if(num==NULL)
	{
		printf("\nERROR: Not enough memory to do rearrangement calculations.\n");
		return 0;
	}

	combos=malloc(num_tiles*16*sizeof(int));
	if(combos==NULL)
	{
		printf("\nERROR: Not enough memory to do rearrangement calculations.\n");
		free(num);
		return 0;
	}

	list=malloc(num_tiles*sizeof(int));
	if(list==NULL)
	{
		printf("\nERROR: Not enough memory to do rearrangement calculations.\n");
		free(combos);
		free(num);
		return 0;
	}

	//clear 'color combo' lists
	memset(combos,0,num_tiles*16*sizeof(int));

	//start each list having one color... color zero
	for(i=0;i<num_tiles;i++)
		num[i]=1;

	//if two colors have the same RGB values... 
	//replace all instances of the redundant color with the first color
	for(i=0;i<256;i++)
	for(ii=i+1;ii<256;ii++)
		if(palette[i]==palette[ii])
			for(index=0; index < num_tiles*8*8; index++)
				if(buffer[index]==ii)
					buffer[index]=i;

	//now, build up the 'color combo' list...
	for(index=0;index<num_tiles;index++)
	for(i=0;i<64;i++)
	{
		data=buffer[index*64+i];

		//is this color already in the list?
		for(ii=0;ii<num[index];ii++)
			if(combos[index*16+ii]==data)
				break;

		//if not add it to the list
		if(ii==num[index])
		{
			if(num[index]==colors) //combo is full
			{
				printf("\nERROR: Detected more colors in one 8x8 tile than is allowed.\n");
				free(list);
				free(combos);
				free(num);
				return 0;
			}

			combos[index*16+ii]=data;
			num[index]++;
		}
	}

	//now sort combos in order of number of colors (greatest to least)
	//
	//here's some more horrid code... I know this is all messy and
	//slow, but hey... I just don't care right now.
	n=0;
	for(ii=colors;ii>0;ii--)
	for(i=0;i<num_tiles;i++)
		if(num[i]==ii)
			list[n++]=i;

	//ok, now try to combine the combos
	last_index=-1;
	for(num_final=0;num_final<9;num_final++)
	{
		//start looking for next 'non-combined' combo in the list
		for(index=last_index+1; index<num_tiles; index++)
				if(num[list[index]]>0)
					break;

		//if none... we're done
		if(index==num_tiles)
			break;
		
		// test = combo # of new 'final combo'
		test=list[index];
		last_index=index;

		//check if we've failed
		if(num_final==8)
		{
			//we already have 8 palettes... can't add more
			printf("\nERROR: not enough colors/palettes to represent the picture.\n");
			free(list);
			free(combos);
			free(num);
			return 0;
		}

		//if one exists, then add to final and start combining
		final[num_final]=test;
		for(n=index+1; n<num_tiles; n++)
		{
			//n = index into sorted list of combos

			//test  = combo # of new 'final combo'
			//test2 = combo we're going to try to combine with the 'final combo'
			test2 = list[n];

			//if already combined to someone... move on
			if(num[test2]<0)
				continue;
		
			//can it be combined?
			num_miss = 0;
			for(ii=test2*16;ii<test2*16+num[test2];ii++)
			{  
				//ii = index into the 'attempting to combine' combo
				// i = index into the 'final combo'

				//check for non-matched colors
				for(i=test*16;i<test*16+num[test]+num_miss;i++) 
					if(combos[ii]==combos[i])
						break;

				//is there a miss?
				if(i == test*16+num[test]+num_miss)
				{
					if(num[test]+num_miss == colors)
					{
						//we can't add anymore colors
						//this combine has failed
						num_miss=-1;
						break;
					}
				
					//temporarily add the missed color to the 'final combo'
					combos[test*16 + num[test] + num_miss] = combos[ii];
					num_miss++;
				}
			}//loop - try to combine an individual combo 
	
			//did we succeed?
			if(num_miss>=0)
			{
				//permanently add in the new colors;
				num[test] = num[test] + num_miss;

				//save the final_num here, and make this negative to show it 
				//has been combined
				num[test2] = num_final - 100;	
			}

		}//loop - sweep through combos, finding potential ones to combine

	}//build up 8 palettes...

	
	//Yeah! ... if we made it here it worked! 
	//(assuming my code is right)
	printf("\nRearrangement possible!! Accomplished in %d palettes...", num_final);
	
	//convert the image
	for(i=0;i<num_tiles;i++)
	{
		//reset conversion table
		memset(color_table, 0, 256*sizeof(int));

		//find which 'final combo' this block uses
		if(num[i]>0)
		{
			//this block's combo became a final

			//find which final it is
			for(n=0;n<num_final;n++)
				if(final[n]==i)
					break;
		}
		else
			n = num[i] + 100;

		//make the conversion table
		for(ii=0;ii<num[final[n]];ii++)
		{
			index = combos[ final[n]*16 + ii ];
			color_table[index] = n*16 + ii;
		}
				
		//convert the block
		for(ii=64*i; ii<64*(i+1); ii++)
			buffer[ii] = (unsigned char) color_table[ buffer[ii] ];
	}


	//clear conversion table, and default palette entries to the original palette
	memcpy(new_palette, palette, 256*sizeof(int));

	//make the palette conversion
	for(i=0;i<num_final;i++)
	for(ii=0;ii<num[final[i]];ii++)
	{
		index = combos[ final[i]*16 + ii ];
		new_palette[ i*16 + ii ] = palette[index];
	}

	//save back the palette
	memcpy(palette, new_palette, 256*sizeof(int));

	
	//free up mem from the combo lists
	free(list);
	free(combos);
	free(num);

	return -1;
}//end of RearrangePalette()
		
//////////////////////////////////////////////////////////////////////////////

int Convert2Pic(char *filebase, unsigned char *buffer, 
				int num_tiles, int blank_absent, int colors, int packed)
{
	char filename[80];
	int x,y,t,b;
	int i;
	int bitplanes;
	int mask;
	unsigned char data;
	FILE *fp;

	if(packed)
		sprintf(filename,"%s.pc7",filebase);
	else
		sprintf(filename,"%s.pic",filebase);

	printf("\nSaving graphics file: [%s]",filename);
	fp = fopen(filename,"wb");
	
	if(fp==NULL)
	{
		printf("\nERROR: Can't open file [%s] for writing\n",filename);
		return 0;
	}
	
	if(packed)
	{
		//remember to add the blank if its needed....
		if(blank_absent)
			for(i=0;i<64;i++)
				fputc(0,fp);

		fwrite(buffer,64*num_tiles,1,fp);

		fclose(fp);
		return -1;
	}

	//find the number of bitplanes
	if(colors==4)
		bitplanes=2;
	else if(colors==16)
		bitplanes=4;
	else if(colors==128)
		bitplanes=8;
	else if (colors==256)
		bitplanes=8;

	//remember to add the blank if its needed....
	if(blank_absent)
		for(i=0;i<8*bitplanes;i++)
			fputc(0,fp);

	for(t=0;t<num_tiles;t++) //loop through tiles
	for(b=0;b<bitplanes;b+=2) //loop through bitplane pairs
	for(y=0;y<8;y++)
	{
		//get bit-mask
		mask = 1 << b;

		//get row of bit-plane
		for(x=0;x<8;x++)
		{
			data = data << 1;
			if(buffer[t*64 + y*8 + x] & mask)
				data = data+1;
		}

		//save row
		fputc(data,fp);

		//adjust bit-mask
		mask = mask << 1;

		//get row of next bit-plane
		for(x=0;x<8;x++)
		{
			data = data << 1;
			if(buffer[t*64 + y*8 + x] & mask)
				data = data+1;
		}

		//save row
		fputc(data,fp);
	}

	fclose(fp);

	return -1;
} //end of Convert2Pic
		
//////////////////////////////////////////////////////////////////////////////

void ConvertPalette(RGB_color *palette, int *new_palette)
{
	int i,data;
	int rounded;
	int temp;

	//Convert the colors and put them in the new array
	//
	//alternate rounding down and rounding up
	//this is an attempt to preserve the brightness of a color
	//which the human eye notices easier than a change in color
	rounded=0;
	for(i=0;i<256;i++)
	{
		data=0;

		//get blue portion and round it off
		temp = (palette[i].blue & 0x01);	//see if this needs rounding
		if(palette[i].blue == 63)			//if value == 63, then we can't round up
		{
			temp = 0; 
			rounded = 1;
		}
		data = (data<<5) + (palette[i].blue >> 1) 
			+ (temp & rounded);				//round up if necessary
		rounded = (temp ^ rounded);			//reset rounded down flag after rounding up

		//get green portion and round it
		temp = (palette[i].green & 0x01);	//see if this needs rounding
		if(palette[i].green == 63)			//if value == 63, then we can't round up
		{
			temp = 0; 
			rounded = 1;
		}
		data = (data<<5) + (palette[i].green >> 1) 
			+ (temp & rounded);				//round up if necessary
		rounded = (temp ^ rounded);			//reset rounded down flag after rounding up

		//get red portion and round it
		temp = (palette[i].red & 0x01);	//see if this needs rounding
		if(palette[i].red == 63)			//if value == 63, then we can't round up
		{
			temp = 0; 
			rounded = 1;
		}
		data = (data<<5) + (palette[i].red >> 1) 
			+ (temp & rounded);				//round up if necessary
		rounded = (temp ^ rounded);			//reset rounded down flag after rounding up
	
		//store converted color
		new_palette[i] = data;

	}//loop through all colors

} //end of ConvertPalette

//////////////////////////////////////////////////////////////////////////////

void PrintOptions(char *str)
{
	if(str[0]!=0)
		printf("\nThe [%s] parameter is not recognized.",str);
	
	printf("\n");
	printf("\nCall with:  Pcx2Snes BASENAME");
	printf("\n    where BASENAME.pcx is a 256 color PCX file");
	printf("\nValid options: ");
	printf("\n    -p   output in packed pixel format");
	printf("\n    -n   no border");
	printf("\n    -s8   |");
	printf("\n    -s16  +-->  size of image blocks");
	printf("\n    -s32  |");
	printf("\n    -s64  |");
	printf("\n    -screen     convert the whole picture");
	printf("\n    -screen7    convert the whole picture for mode 7 format");
	printf("\n    -c4   |");
	printf("\n    -c16  +-->  the number of colors to use");
	printf("\n    -c128 |");
	printf("\n    -c256 |");
	printf("\n    -o#  where # = 0 to 256, the number of colors to output");
	printf("\n                  to the BASENAME.clr (SNES color file)");
	printf("\n    -r   rearrange palette, and preserve palette numbers");
	printf("\n         in the tilemap. this only affects screen mode.");
	printf("\n");
} //end of PrintOptions()

/// M A I N ////////////////////////////////////////////////////////////

int main(int argc, char **arg)
{
	int	border=1;			//options and their defaults
	int	packed=0;			//
	int size=0;				//
	int screen=0;			//	
	int colors=0;			//
	int output_palette=-1;  //
	int rearrange=0;		//

	int palette[256];
	int tile_x, tile_y;

	unsigned char clr;
	int height, width;
	int xsize, ysize;
	int num_tiles;
	int blank_absent=0;
	pcx_picture image;

	unsigned char *buffer;
	unsigned char *temp;
	int *tilemap;
	FILE *fp;

	char filebase[80]="";
	char filename[80];
	int i, j;

	//parse the arguments
	for(i=1;i<argc;i++)
	{
		if(arg[i][0]=='-')
		{
			if(arg[i][1]=='p')	//packed pixels
				packed=1;
			else if(arg[i][1]=='n') //no border
				border=0;
			else if(arg[i][1]=='s') //size specification
			{
				if(arg[i][2]=='c') //full screen
				{
					if( strcmp(&arg[i][1],"screen") == 0)
					{
						screen=1;					
						border=0;
					}
					else if( strcmp(&arg[i][1],"screen7") == 0)
					{
						screen=7;					
						border=0;
						packed=1;
					}
					else
					{
						PrintOptions(arg[i]);
						return 1;					
					}
				}
				else //block size
				{
					size=atoi(&arg[i][2]);
					if( (size != 8) && (size !=16) && 
						(size != 32) && (size != 64) )
					{
						PrintOptions(arg[i]);
						return 1;
					}
				}
			}
			else if(arg[i][1]=='c') //color number specification
			{
				colors=atoi(&arg[i][2]);
				if( (colors != 4) && (colors !=16) && 
					(colors != 128) && (colors != 256) )
				{
					PrintOptions(arg[i]);
					return 1;
				}
			}
			else if(arg[i][1]=='o') //color number specification
			{
				output_palette=atoi(&arg[i][2]);
				if( (output_palette < 0) || (output_palette > 256) )
				{
					PrintOptions(arg[i]);
					return 1;
				}
			}
			else if(arg[i][1]=='r') //re-arrange palette option
				rearrange=1;
			else //invalid option
			{
				PrintOptions(arg[i]);
				return 1;
			}
		}
		else
		{
			//its not an option flag, so it must be the filebase
			if(filebase[0]!=0) // if already defined... there's a problem
			{
				PrintOptions(arg[i]);
				return 1;
			}
			else
				strcpy(filebase,arg[i]);
		}
	}

	//make sure options are valid
	if((colors==0) && screen)
		colors=256;
	
	if( filebase[0] == 0 )
	{
		printf("\nYou must specify a base filename.");
		PrintOptions("");
		return 1;
	}
	
	if( colors == 0 )
	{
		printf("\nThe Number of Colors parameter must be specified.");
		PrintOptions("");
		return 1;
	}

	if((size == 0) && (border == 0) && (screen == 0)) 
	{
		printf("\nThe Size parameter must be specified when the border");
		printf("\nis turned off.");
		PrintOptions("");
		return 1;
	}

	if(rearrange)
	{
		//let i = the number of colors in all 8 palettes
		i = colors*8;
		if(i>256)
			i=256;

		if((colors == 256) || (colors == 128))
		{
			printf("\nWARNING: -c128 and -c256 override the -r option.");
			printf("\n    The palette will not be rearranged.");
			printf("\n");
			rearrange=0;
		}
		if(screen == 0)
		{
			printf("\nWARNING: the -r option means nothing in image block mode.");
			printf("\n    The palette will not be rearranged.");
			printf("\n");
			rearrange=0;
		}
		else if((output_palette != i) && (output_palette != -1))
		{
			printf("\nWARNING: -o# is not over-riden, but because -r was");
			printf("\nselected, anything other than the 8 palettes won't ");
			printf("\nmean much	after all the colors are re-arranged.");
			printf("\n");
		}
		else
			output_palette=i;
	}
	
	if(output_palette==-1)
		output_palette=256;

	//Load picture
	sprintf(filename,"%s.pcx",filebase);
	printf("\nOpenning graphics file: [%s]",filename);
	if(!PCX_Load(filename,(pcx_picture_ptr) &image))
		return 1;

	//convert the palette
	ConvertPalette(image.palette, palette);

	height = image.header.height;
	width = image.header.width;

	//autodetect size if necessary
	if((size == 0) && (screen == 0))
	{
		printf("\nAuto-detecting size of image blocks...");
	
		clr = image.buffer[0]; //get the border color
		for(i=1; i<width; i++)
			if(image.buffer[i]!=clr)
				break;

		xsize = i-1; //if size = n, then xsize = multiple of (n+1)

		for(i=1; i<height; i++)
			if(image.buffer[i*width]!=clr)
				break;

		ysize = i-1; //if size = n, then xsize = multiple of (n+1)
	
		//look along line 1 until we hit the border color
		//this should be the end of the first image block
		for(i=1; i<width; i++)
			if(image.buffer[i + width]==clr)
				break;
		
		size = i-1;

		if( (xsize%(size+1) != 0 ) || (ysize%(size+1) != 0 ) )
		{
			printf("\nBorder format is incorrect... autodetect size failed.\n");
			return 1;
		}
	
		xsize = xsize/(size+1); //the number of blocks across
		ysize = ysize/(size+1); //the number of blocks down
	}
	else
	{
		//determine the constants if in screen mode
		//or image block mode with no borders
		
		if(screen)
			size=8;

		xsize = width/size;
		if(width%size)
			xsize++;

		ysize = height/size;
		if(height%size)
			ysize++;
	}

	//if its a full screen, determine the number of tiles
	if(screen)
	{
		if(width>256)
			tile_x=64;
		else
			tile_x=32;

		if(height>256)
			tile_y=64;
		else
			tile_y=32;

		if(screen==7)
		{
			tile_x=128;
			tile_y=128;
		}
	}


	//Print what the user has selected
	printf("\n****** O P T I O N S ***************");
	if(border)
		printf("\nborder=ON");
	else
		printf("\nborder=OFF");

	if(packed)
		printf("\npixel format = packed-bit");
	else
		printf("\npixel format = bit-plane");

	printf("\nPCX file: %dx%d pixels",width,height);
	
	if(screen)
	{
		printf("\nScreen mode selected: %dx%d tile map",tile_x,tile_y);
		if(rearrange)
			printf("\nPalette rearrange: ON");
		else
			printf("\nPalette rearrange: OFF");
	}
	else
	{
		printf("\nImage mode selected: %dx%d pixel blocks",size,size);
		printf("\n          image has: %dx%d blocks",xsize,ysize);
	}
	printf("\nTotal colors = %d", colors);
	
	if(output_palette)
		printf("\nPalette section to convert: Color #0 to %d", output_palette-1);
	printf("\n************************************");

	//truncate the colors if necessary
	if(colors == 128)
	{
		j = colors - 1;	//color truncation mask
		temp = image.buffer;
		
		for(i=0;i<width*height;i++)
			temp[i] = temp[i] & j; 
	}

	//arrange the blocks according to how we would like them represented.
	if(screen)
	{
		j=xsize;
		num_tiles=ysize;

		//first arrange into a list of 8x8 blocks
		buffer=ArrangeBlocks(image.buffer, width, height,
							 size, &j, &num_tiles, 8, 0);
		free(image.buffer);
	
		if(buffer==NULL)
		{
			printf("\nERROR:Not enough memory to do image operations...\n");
			return 1;
		}
	
		//if re-arranging is wanted, attempt it now
		if(rearrange)
		{
			if(!RearrangePalette(buffer, palette, num_tiles, colors))
			{
				free(buffer);
				return 1;
			}
		}

		//make the tile map now
		tilemap=MakeMap(buffer, &num_tiles, xsize, ysize, 
					tile_x, tile_y, colors, rearrange);
		if(tilemap==NULL)
		{
			free(buffer);
			printf("\nERROR:Not enough memory to do tile map optimizations..\n");
			return 1;
		}
	
		if(num_tiles<0)
		{
			num_tiles = -num_tiles;
			blank_absent = 1;
		}

		if((screen == 7) && (num_tiles+blank_absent)>256)
		{
			printf("\nERROR: Need %d tiles to represent the screen in mode7"
					,num_tiles+blank_absent);
			printf("\n       the SNES only has room for 256 tiles");
			free(buffer);
			free(tilemap);
			return 1;
		}

		printf("\nReduced screen to %d tiles.",num_tiles+blank_absent);
	}
	else //image block mode
	{
		//first arrange in SNES image block format
		buffer=ArrangeBlocks(image.buffer, width, height,
							 size, &xsize, &ysize, 16*8, border);
		free(image.buffer);
		
		if(buffer==NULL)
		{
			printf("\nERROR:Not enough memory to do image operations...\n");
			return 1;
		}

		//now re-arrange into a list of 8x8 blocks for easy conversion
		xsize *= size/8;
		ysize *= size/8;
		
		temp=ArrangeBlocks(buffer, xsize*8, ysize*8,
							 8, &xsize, &ysize, 8, 0);
		free(buffer);
		
		if(temp==NULL)
		{
			printf("\nERROR:Not enough memory to do image operations...\n");
			return 1;
		}
		
		buffer=temp;
		num_tiles=xsize*ysize;
	}

	//convert pictures and save to file	
	if(!Convert2Pic(filebase, buffer, num_tiles, blank_absent, colors, packed))
	{
		if(screen)
			free(tilemap);
		free(buffer);
		return 1;
	}

	//free up image memory
	free(buffer);

	//save the map
	if(screen)
	{
		if(screen==7)
			sprintf(filename,"%s.mp7",filebase);
		else
			sprintf(filename,"%s.map",filebase);
		
		printf("\nSaving tile map file: [%s]",filename);
		fp = fopen(filename,"wb");
		if(fp==NULL)
		{	
			printf("\nERROR: Can't open file [%s] for writing\n",filename);
			free(tilemap);
			return 1;
		}
	
		for(i=0;i<tile_x*tile_y;i++)
		{
			if(screen==7)
				fputc(tilemap[i],fp);
			else
				PutWord(tilemap[i],fp);
		}

		fclose(fp);

		//free up tilemap memory
		free(tilemap);
	}

	//convert and save the palette if necessary
	if(output_palette)
	{
		sprintf(filename,"%s.clr",filebase);
		printf("\nSaving palette file: [%s]",filename);
		fp = fopen(filename,"wb");
	
		if(fp==NULL)
		{	
			printf("\nERROR: Can't open file [%s] for writing\n",filename);
			return 0;
		}

		for(i=0;i<output_palette;i++)
			PutWord(palette[i],fp);
		
		fclose(fp);
	}

	printf("\nDone!\n\n");

	return 0;
}
	
