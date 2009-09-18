/*
	SDLcam:  A program to view and apply effects in real-time to video
	Copyright (C) 2002, Raphael Assenat

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// TODO: Improve stripe detection
// TODO: Add support for UPC_E, EAN_8 and CODE_39
// TODO: Add vertical and diagonal scanning

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../SDL_v4l_image.h"
#include "../dyn_misc.h"
#include <stdlib.h>
#include <stdio.h>

/*
#include "../SDL_v4l.h"
#include "../SDL_v4l_filters.h"
#include "../SDL_v4l_filter_upc.h"
*/

/* filter options */
#define BEEP_ON_SUCCESS
#define NUM_HORIZONTAL_SCANLINES 16

/* engine options */
#define DYNAMIC_RECALIBRATION

/* engine debug */
//#define DEBUG_LENGTHS
//#define DEBUG_RELATIVE_LENGTHS
//#define DEBUG_DECODED
//#define DEBUG_RECALIBRATION
//#define DEBUG_ERRORS
//#define DEBUG_EAN13
//#define IGNORE_ERRORS
//#undef IGNORE_ERRORS

#define BARCODE_UNKNOWN 0
#define BARCODE_UPC_A 1
#define BARCODE_UPC_E 2
#define BARCODE_EAN_13 4
#define BARCODE_EAN_8 8
#define BARCODE_CODE_39 16

typedef struct _Barcode_t {
	unsigned char type;
	unsigned char string[50];
	} Barcode_t;

/* This is the odd parity UPC-E Encoding table
The even parity is the invert of this table.
eg: 3,2,1,1 (0) becomes 1,1,2,3.
and the color of the bars are inverted too.
so: 0001101 becomes 0100111 */
static char UPC_E_decoding_table[10][4] = {
			{ 3,2,1,1 }, // 0
			{ 2,2,3,1 }, // 1
			{ 2,1,2,2 }, // 2
			{ 1,4,1,1 }, // 3
			{ 1,1,3,2 }, // 4
			{ 1,2,3,1 }, // 5
			{ 1,1,1,4 }, // 6
			{ 1,3,1,2 }, // 7
			{ 1,2,1,3 }, // 8
			{ 3,1,1,2 }, // 9
};

/* This is the UPC-E Parity Encoding Table */
/* This table represents the number system 0 encoding */
/* The number system 1 encoding is the inverse (XOR 111111) */
/* 0 = even,  1 = odd */
static char UPC_E_Parity_table[10][6] = {
			{ 0,0,0,1,1,1 }, // 0
			{ 0,0,1,0,1,1 }, // 1
			{ 0,0,1,1,0,1 }, // 2
			{ 0,0,1,1,1,0 }, // 3
			{ 0,1,0,0,1,1 }, // 4
			{ 0,1,1,0,0,1 }, // 5
			{ 0,1,1,1,0,0 }, // 6
			{ 0,1,0,1,0,1 }, // 7
			{ 0,1,0,1,0,1 }, // 8
			{ 0,1,0,1,1,0 }, // 9
};

/* This table contains the parities used
 * for the numbers 2-7. The index is the
 * decoded number 0. Number 1 is allways
 * odd.
 * o=even, 1=odd
 */
static char EAN_13_Parity_table[10][5] = {
			{ 1,1,1,1,1 }, // 0
			{ 1,0,1,0,0 }, // 1
			{ 1,0,0,1,0 }, // 2
			{ 1,0,0,0,1 }, // 3
			{ 0,1,1,0,0 }, // 4
			{ 0,0,1,1,0 }, // 5
			{ 0,0,0,1,1 }, // 6
			{ 0,1,0,1,0 }, // 7
			{ 0,1,0,0,1 }, // 8
			{ 0,0,1,0,1 }, // 9
};


/* This is the decoding table for the UPC-A standard*/
static char UPC_12digit_translation[10][4] = {
			{ 3, 2, 1, 1 }, // 0
			{ 2, 2, 2, 1 }, // 1
			{ 2, 1, 2, 2 }, // 2
			{ 1, 4, 1, 1 }, // 3
			{ 1, 1, 3, 2 }, // 4
			{ 1, 2, 3, 1 }, // 5
			{ 1, 1, 1, 4 }, // 6
			{ 1, 3, 1, 2 }, // 7
			{ 1, 2, 1, 3 }, // 8
			{ 3, 1, 1, 2 } // 9
};

#define EAN_13_decoding_table UPC_12digit_translation





int Apply (int mode, const SDL_v4l_image *SRC, SDL_v4l_image *DST) {
  if (mode == 0) return 0;
  SDL_v4l_Filter_UPC (SRC, DST);
}
int Type (void) { return 0; }
const char *Name    (void) { return "barcode"; }
const char *Display (void) { return "Barcode"; }

int SDL_v4l_Filter_UPC(const SDL_v4l_image *SRC, SDL_v4l_image *DST)
{
	int i, x, res;
	//char decoded_digits_temp[30];
	char decoded_digits[30];
	int horiz_offset;
	int total_decoded=0;
	static int serial_number=0;
	Barcode_t barcode;


	for (i=0; i<NUM_HORIZONTAL_SCANLINES; i++)
	{
		horiz_offset=SRC->pitch*((i+1)*(SRC->h/(NUM_HORIZONTAL_SCANLINES+1) )  );

		res = read_and_decode_barcode(&barcode, &SRC->data[horiz_offset], SRC->w,  &DST->data[horiz_offset]);
		if (res!=-1) {
			total_decoded++;
			strcpy(decoded_digits, barcode.string);
		}

		/* Draw a red line to show where we were reading */
/*		if (res==-1) {
			for (x=0; x<SRC->w; x++) {
				((int*)SRC->data)[(horiz_offset>>2)+x]=0x00FF0000;
			}
		} else*/
		if (res!=-1){
			for (x=0; x<SRC->w; x++) {
				((int*)SRC->data)[(horiz_offset>>2)+x]=0x0000FF00;
			}
		}
	}

	/* The dimensions have not changed*/
	DST->len = SRC->len; DST->w = SRC->w;
	DST->h = SRC->h; DST->pitch = SRC->pitch;

	if (total_decoded>0)
	{
		printf("serial_number: %08d: %s\n", serial_number ,decoded_digits);
		serial_number++;
	}


	return 0;
}


/* Read 32 bit RGBr scanline, find, recognize, and decode the bar code.
	note: scanline_width is in pixels, not in bytes!

	The decoded_ascii string will be null terminated :)

	returns the number of decoded characters if successful, otherwise -1.
*/
int read_and_decode_barcode(Barcode_t *barcode, const unsigned char *scanline, int scanline_width, unsigned char *scanline_dst)
{
	int x;
	unsigned char line[1024];
	unsigned char lengths[1000];
	unsigned char relative_lengths[1000];
	unsigned char *p;
	char decoded_digits[30];
	int num_lengths=0;
	int num_relative_lengths=0;
	unsigned char decoded_type=BARCODE_UNKNOWN;
	char has_error=0;
	int output_width;

	/* extract and posterize line data */
	output_width = posterize_32bit_scanline(scanline, line, scanline_width);

	p = line;
	for (x=0; x<scanline_width; x++) {
		unsigned char r,g,b;
		
		if (*p)  {
			r = 0xff;
			g = 0xff;
			b = 0x00;
		} else {
			r = 0x00;
			g = 0xff;
			b = 0xff;
		}
		p++;
		p++; // HACK for double output width


		*scanline_dst = r;
		scanline_dst++;
		*scanline_dst = g;
		scanline_dst++;
		*scanline_dst = b;
		scanline_dst++;
		*scanline_dst = 0;
		scanline_dst++;
	}

	/* convert the posterized line data to lengths */
	num_lengths = scanline_to_lengths(line, lengths, output_width);

	/* convert the raw lengths to relative lengths */
	num_relative_lengths = lengths_to_relative_lengths(lengths, relative_lengths, num_lengths);

	/* decode the barcode */
	/* relative_lengths does not include the first 3 bars=lead */
	decoded_type = decode_relative_lengths(relative_lengths, decoded_digits, num_relative_lengths,&has_error);


	/* some debugging... */
#ifdef DEBUG_LENGTHS
	if (num_lengths>0)printf("DEBUG_LENGTHS=");
	for (x=0; x<num_lengths; x++)
		printf("%d ",lengths[x]); fflush(stdout);
	printf("\n");
#endif
#ifdef DEBUG_RELATIVE_LENGTHS
	if (num_relative_lengths>0)printf("DEBUG_RELATIVE_LENGTHS=");
	for (x=0; x<num_relative_lengths; x++)
		printf("%d ",relative_lengths[x]);
	printf("\n");
#endif

	if (has_error) { return -1; } /* decode error... */

	if (decoded_type==BARCODE_EAN_13) /* EAN_13 */
	{
	 printf("found EAN-13\n");
		barcode->type = BARCODE_EAN_13;

		/* convert the digits to ascii */
		for (x=0; x<13; x++) {
			barcode->string[x]=48+decoded_digits[x];
		}
		barcode->string[x]=0;  // string termination
		return 13;
	}

	if (decoded_type==BARCODE_UPC_A) /* UPC_A */
	{
	 printf("found UPC-A\n");
		barcode->type = BARCODE_UPC_A;

		/* convert the digits to ascii */
		for (x=0; x<12; x++) {
			barcode->string[x]=48+decoded_digits[x];
		}
		barcode->string[x]=0;  // string termination
		return 12;
	}


	if (decoded_type==BARCODE_UPC_E) /* UPC_E */
	{
	 printf("found UPC-E\n");
		barcode->type = BARCODE_UPC_E;

		/* convert the digits to ascii */
		for (x=0; x<12; x++) {
			barcode->string[x]=48+decoded_digits[x];
		}
		barcode->string[x]=0;  // string termination
		return 12;
	}

	/* should not be reached ... */
	barcode->type = BARCODE_UNKNOWN;
	return -1;
}

#define THIRD_VERSION
/* Take a 32 bit RGBr scanline of data, and output it as 
 * thresholded(0, 255) 8 bit data.
 * source_width is in pixels. Outputs is two times the
 * width of the input. Make suze 'posterized' is big enough'
 **/
int posterize_32bit_scanline(const unsigned char *source, unsigned char *posterized, int source_width)
{
	int x;

#ifdef FIRST_VERSION
	unsigned char a;

	for (x=0; x<source_width; x++) {
		a = (source[x*4]+source[x*4+1]+source[x*4+2])/3;
		if (a<127) { posterized[x]=0; } else { posterized[x]=255; }
	}
	return source_width;
#endif

#ifdef SECOND_VERSION
	char a;
	char bg_red   = (source[0*4  ] + source[1*4  ]) / 2;
	char bg_green = (source[0*4+1] + source[1*4]+1) / 2;
	char bg_blue  = (source[0*4+2] + source[1*4+2]) / 2;
//debug	printf("DEBUG: posterize() bg-color=%d %d %d\n", bg_red, bg_green, bg_blue);
	for (x=0; x<source_width; x++) {
		a = ((source[x*4]-bg_red)+(source[x*4+1]-bg_green)+(source[x*4+2]-bg_blue))/3;
		if (a<0) { posterized[x]=0; } else { posterized[x]=255; }
	}

	return source_width;
#endif

#ifdef THIRD_VERSION
	unsigned char grey;
	unsigned char previousGreys[32];
	unsigned char min,max;
	int range, offset;
	int gi=0, i, xx;

	/* Initialize the round-robin with REAL values */
	for (xx=0; xx < sizeof(previousGreys); xx++) {
		grey = (source[xx*4]+source[xx*4+1]+source[xx*4+2]) / 3;
		previousGreys[xx] = grey;
	}

	for (x=0; x<source_width; x++) 
	{
		/* calculate the grey value by average. */
		grey = (source[x*4]+source[x*4+1]+source[x*4+2]) / 3;

		/* Manage the round-robin previous values array */
		previousGreys[gi] = grey;
		gi++;
		if (gi >= sizeof(previousGreys)) {
			gi = 0;
		}

		/* Find the minimum and maximum luminosity pixels in
		 * previous samples */
		min = 255; max = 0;
		for (i=0; i<sizeof(previousGreys); i++) {
			if (previousGreys[i] < min)
				min = previousGreys[i];

			if (previousGreys[i] > max)
				max = previousGreys[i];
		}

		range = max - min;
		if (range < 25)
			range = 25;	// dont let it become too low. It generates extreme noise
	
		/* Now stretch the range to cover 0 to 255 */
		grey = (grey - min) * 255 / range;

		/* Store results at two bytes intervals for later interpolation */
		posterized[x*2] = grey;
	}

	for (x=0; x<source_width; x++) {
		unsigned char prev, next;
		prev = posterized[x*2];
		if (x != source_width-1)
			next = posterized[(x+1) * 2];
		else
			next = prev;

		posterized[x*2 + 1] = (prev + next) >> 1;
	}

	for (x=0; x<source_width * 2; x++) {
		
		if (posterized[x]<127) { 
			posterized[x]=0; 
		} else { 
			posterized[x]=255; 
		}
	}

	return source_width * 2;
#endif

	return 0;
}

/* convert a 8bit scanline to a list a lengths. */
int scanline_to_lengths(const unsigned char *scanline, unsigned char *lengths, int scanline_length)
{
	int num_lengths=0, count=0, x;
	unsigned char a;

	a = 255;
	count = 0;
	for (x=1; x<scanline_length; x++) {
		count++;
		if (a!=scanline[x]) // change of state
		{
			a=scanline[x];
			lengths[num_lengths]=count;
			num_lengths++;
			count=0;
		}
	}

	return num_lengths;
}


/* scan the relative lengths until 3 lines of the same size are found. */
int get_start_of_code_index(const unsigned char *relative_lengths, int num_relative_lengths)
{
	int i=0;

	if (num_relative_lengths<3) { return -1; }

	printf("relative_lengthes: ");
	for (i=0; i<num_relative_lengths; i++)
	{
		printf("%d ", relative_lengths[i]);
	}
	printf("\n");

	for (i=0; i<num_relative_lengths-3; i++)
	{
		if ( relative_lengths[i] == relative_lengths[i+1] &&
				relative_lengths[i+1] == relative_lengths[i+2] )
		{
			//printf("offset: %d\n", i);
			return i;
		}
	}

	return -1;
}


/*
	Convert lengths to relative lengths. That is, use the 3 first lengths as reference units,
	and output lengths relative to the reference unit.
*/
int lengths_to_relative_lengths(const unsigned char *lengths, unsigned char *relative_lengths, int num_lengths)
{
	int num_relative_lengths=0, x;
	char ref_unit=0;
	char last_3_units[3];

// Convert to lengths to relative lengths
	ref_unit = (lengths[1]+lengths[2]+lengths[3])/3;	/* Use the start of code to get the smaller bar length */
	if (ref_unit == 0) return 0;
	last_3_units[0] = lengths[1]; last_3_units[1] = lengths[2]; last_3_units[2] = lengths[3];
	//printf("recal unit %d\n",ref_unit);
	for (x=4; x<num_lengths; x++) {	/* we start at 4 because we skip the first length, and the start of code*/
		relative_lengths[x-4]=lengths[x]/ref_unit;

		if (relative_lengths[x-4]<1) { relative_lengths[x-4]=1; }
		if (relative_lengths[x-4]>4) { relative_lengths[x-4]=4; }

#ifdef DYNAMIC_RECALIBRATION
		if ((relative_lengths[x-4]==1&&!x%2))
		{
			last_3_units[0]=last_3_units[1];
			last_3_units[1]=last_3_units[2];
			last_3_units[2]=lengths[x];
			//last_3_units[2]=lengths[x]/relative_lengths[x-4];

			ref_unit = (last_3_units[0]+last_3_units[1]+last_3_units[2])/3;
#ifdef DEBUG_RECALIBRATION
			printf("recal unit %d\n",ref_unit);
#endif
		}
#endif
		num_relative_lengths++;
	}

	return num_relative_lengths;
}

/*
 * Will return the type of code
 * relative_lengths contains 1,2,3 or 4 and is the width of the bars and spaces
 * relative to the first 3.
 * The first 3=lead are not included. These first 3 have the length of 1.
 */
int decode_relative_lengths(const unsigned char *relative_lengths, char *decoded_digits, int num_relative_lengths, char *has_error)
{
	int res;
	res = try_decode_UPC_A(relative_lengths, decoded_digits, num_relative_lengths, has_error);
	if (! (*has_error)) { return BARCODE_UPC_A; }

	res = try_decode_EAN_13(relative_lengths, decoded_digits, num_relative_lengths, has_error);
	if (! (*has_error)) { return BARCODE_EAN_13; }


	res = try_decode_UPC_E(relative_lengths, decoded_digits, num_relative_lengths, has_error);
	if (! (*has_error)) { return BARCODE_UPC_E; }

	*has_error=1;
	return BARCODE_UNKNOWN;
}

int try_decode_EAN_13(const unsigned char *relative_lengths, char *decoded_digits, int num_relative_lengths, char *has_error)
{
        int num_decoded_digits = 0;
	int x = 0;
	char parities[13];
	int tmp_decoded_digits[12];
        char check_digit;

        *has_error=0;

	if (num_relative_lengths < 4*12) {
#ifndef IGNORE_ERRORS
                *has_error=1;
	       return -1;
#endif
	}


#ifdef DEBUG_EAN13
printf("EAN13: ");
#endif
        // Decode the relative data
        for (x=0; x<num_relative_lengths; x+=4)
        {
		tmp_decoded_digits[num_decoded_digits] = decode_EAN_13_digit(&relative_lengths[x], &parities[num_decoded_digits]);
#ifdef DEBUG_EAN13
	 printf("%d%d%d%d=%i(%d)%i\t", relative_lengths[x], relative_lengths[x+1], relative_lengths[x+2], relative_lengths[x+3], tmp_decoded_digits[num_decoded_digits], parities[num_decoded_digits], num_decoded_digits);
#endif

                if (tmp_decoded_digits[num_decoded_digits]==-1) { /* unknow symbol... marked as -1 */
#ifndef IGNORE_ERRORS
                        *has_error=1;
#ifdef DEBUG_ERRORS
                        printf("Decode error (unknown symbol)\n");
#endif
#endif
#ifdef DEBUG_EAN13
   		       printf("\n"); 
#endif
		       return -1;
                }

                if (x==0 && parities[num_decoded_digits] == 0) {
#ifndef IGNORE_ERRORS
                        *has_error=1;
#ifdef DEBUG_ERRORS
                        printf("Decode error (parity of EAN country-code digit 2 must be odd) num_decoded_digits=%d\n", num_decoded_digits);
#endif
#endif
#ifdef DEBUG_EAN13
		printf("\n"); 
#endif
		       return -1;
		}

                num_decoded_digits++;

                /* we have reached the middle bars, skip them... */
                if (num_decoded_digits==6) { x+=5; }

                /* we have completely decoded this 13 digit EAN code */
                if (num_decoded_digits==12) {break; }
        }
#ifdef DEBUG_EAN13
printf("\n");
#endif

// decode byte 0 by using the parities found according to http://www.codeproject.com/csharp/ean_13_barcodes.asp
// byte 1 (decoded_num_0) is allways odd, the other bytes give us byte 0 by using EAN_13_Parity_table
	char number_0;
	int i;
        decode_EAN_13_parity(parities, &number_0);

	decoded_digits[0]=number_0;
	for (i=0; i<12; i++)
		decoded_digits[i+1]=tmp_decoded_digits[i];
	num_decoded_digits=13;


//TODO: check checksum
//TODO: check trailer to be 1,1,1


        return num_decoded_digits;
}

int try_decode_UPC_A(const unsigned char *relative_lengths, char *decoded_digits, int num_relative_lengths, char *has_error)
{
	int num_decoded_digits=0, x;
	char check_digit;

	*has_error=0;

	// Decode the relative data
	for (x=0; x<num_relative_lengths; x+=4)
	{
		decoded_digits[num_decoded_digits] = decode_upc_A_digit(&relative_lengths[x]);

		if (decoded_digits[num_decoded_digits]==-1) { /* unknow symbol... marked as -1 */
#ifndef IGNORE_ERRORS
			*has_error=1;
#ifdef DEBUG_ERRORS
			printf("Decode error\n");
#endif
#endif
		}
		num_decoded_digits++;

		/* we have reached the middle bars, skip them... */
		if (num_decoded_digits==6) { x+=5; }

		/* we have completely decoded this 12 digit UPC code */
		if (num_decoded_digits==12) { break; }
	}



	/* Calculate the checksum digit */
#ifndef IGNORE_ERRORS
	check_digit = compute_checksum(decoded_digits);
	if (check_digit!=decoded_digits[11])
	{
#ifdef DEBUG_ERRORS
		printf("Bad checksum\n");
#endif
		*has_error=1;
	}
#endif

	return num_decoded_digits;
}

int try_decode_UPC_E(const unsigned char *relative_lengths, char *decoded_digits, int num_relative_lengths, char *has_error)
{
	int num_decoded_digits=0, x, i;
	char parities[10];
	int tmp_decoded_digits[6];
	char check_digit;
	char number_system;  // 0 or 1 for UPC_E

	*has_error=0;

	// Decode the relative data
	for (x=0; x<num_relative_lengths; x+=4)
	{
		tmp_decoded_digits[num_decoded_digits] = decode_upc_E_digit(&relative_lengths[x], &parities[num_decoded_digits]);

		if (tmp_decoded_digits[num_decoded_digits]==-1) { /* unknow symbol... marked as -1 */
#ifndef IGNORE_ERRORS
			*has_error=1;
#ifdef DEBUG_ERRORS
			printf("Decode error\n");
#endif
#endif
		}
		num_decoded_digits++;

		/* Finished decoding the code :) */
		if (num_decoded_digits==6) { break; }
	}

	if (num_decoded_digits!=6) { *has_error=1; return -1; }

	/* The check digit and the number system is encoded in the
	parity of the 6 encoded digits. */
	decode_upc_E_parity(parities, &number_system, &check_digit);

	decoded_digits[0]=number_system;
	for (i=1; i<7; i++)
		decoded_digits[i]=tmp_decoded_digits[i-1];
	decoded_digits[7]=check_digit;
	num_decoded_digits=8;

	/* Expand the data to UPC-A format */
	num_decoded_digits = expand_UPC_E(decoded_digits, decoded_digits);



	/* Calculate the checksum digit */
#ifndef IGNORE_ERRORS
	check_digit = compute_checksum(decoded_digits);
	if (check_digit!=decoded_digits[11])
	{
#ifdef DEBUG_ERRORS
		printf("Bad checksum\n");
#endif
		*has_error=1;
	}
#endif

	return num_decoded_digits;
}


/* Expand the UPC_E data to UPC_A */
/* digits and decoded_digits may point at the same location */
int expand_UPC_E(const char *digits, char *decoded_digits)
{
	char compressed[8];

	memcpy(compressed, digits, 8);
	memset(decoded_digits, 0, 12);

	if (compressed[6]==0||compressed[6]==1||compressed[6]==2)
	{
		memcpy(decoded_digits, compressed, 3);
		decoded_digits[3]=compressed[6];
		memcpy(&decoded_digits[8], &compressed[3], 3);
		decoded_digits[11]=compressed[7];
	}

	if (compressed[6]==3)
	{
		memcpy(decoded_digits, compressed, 3);
		memcpy(&decoded_digits[8], &compressed[3], 3);
		decoded_digits[11]=compressed[7];
	}

	return 12;
}

int decode_EAN_13_parity(const char *parities, char *countryCodeByte0) {
	int i;
	for (i=0; i<10; i++)
	{
		if (    EAN_13_Parity_table[i][0]==parities[1]&&
			EAN_13_Parity_table[i][1]==parities[2]&&
			EAN_13_Parity_table[i][2]==parities[3]&&
			EAN_13_Parity_table[i][3]==parities[4]&&
			EAN_13_Parity_table[i][4]==parities[5])
		{
			*countryCodeByte0=i;
			return 0;
		}
	}
	return -1;
} 

/* Extract the check digit and the number system from the
parity of the 6 digits encoded in a UPC-E barcode. */
int decode_upc_E_parity(const char *parities, char *number_system, char *check_digit)
{
	int i;
	for (i=0; i<10; i++)
	{
		/* try number system 0 encoding */
		if (UPC_E_Parity_table[i][0]==parities[0]&&
			UPC_E_Parity_table[i][1]==parities[1]&&
			UPC_E_Parity_table[i][2]==parities[2]&&
			UPC_E_Parity_table[i][3]==parities[3]&&
			UPC_E_Parity_table[i][4]==parities[4]&&
			UPC_E_Parity_table[i][5]==parities[5])
		{
			*number_system=0;
			*check_digit=i;
			return 0;
		}

		if (UPC_E_Parity_table[i][0]!=parities[0]&&
			UPC_E_Parity_table[i][1]!=parities[1]&&
			UPC_E_Parity_table[i][2]!=parities[2]&&
			UPC_E_Parity_table[i][3]!=parities[3]&&
			UPC_E_Parity_table[i][4]!=parities[4]&&
			UPC_E_Parity_table[i][5]!=parities[5])
		{
			*number_system=0;
			*check_digit=i;
			return 0;
		}
	}
	return -1;
}


/* Translate the EAN-13 bar pattern to a standard integer number.
 * if parity==0 even, parity==1 odd */
int decode_EAN_13_digit(const char *lengths, char *parity)
{
        int i = 0;

        /* scan the table of known codes */
        for (i=0; i<10; i++) {
                        /* Try odd parity */
                        if (EAN_13_decoding_table[i][0]==lengths[0]&&
                                EAN_13_decoding_table[i][1]==lengths[1]&&
                                EAN_13_decoding_table[i][2]==lengths[2]&&
                                EAN_13_decoding_table[i][3]==lengths[3])
                        {
                                *parity=1;
                                return i;
                        }

                        /* Try even parity */
                        if (EAN_13_decoding_table[i][0]==lengths[3]&&
                                EAN_13_decoding_table[i][1]==lengths[2]&&
                                EAN_13_decoding_table[i][2]==lengths[1]&&
                                EAN_13_decoding_table[i][3]==lengths[0])
                        {
                                *parity=0;
                                return i;
                        }
        }

        return -1;
}

/* Translate the UPC-E bar pattern to a standard integer number.
if parity==0 even, parity==1 odd */
int decode_upc_E_digit(const char *lengths, char *parity)
{
	int i;

	/* scan the table of known codes */
	for (i=0; i<10; i++) {
			/* Try odd parity */
			if (UPC_E_decoding_table[i][0]==lengths[0]&&
				UPC_E_decoding_table[i][1]==lengths[1]&&
				UPC_E_decoding_table[i][2]==lengths[2]&&
				UPC_E_decoding_table[i][3]==lengths[3])
			{
				*parity=1;
				return i;
			}

			/* Try even parity */
			if (UPC_E_decoding_table[i][0]==lengths[3]&&
				UPC_E_decoding_table[i][1]==lengths[2]&&
				UPC_E_decoding_table[i][2]==lengths[1]&&
				UPC_E_decoding_table[i][3]==lengths[0])
			{
				*parity=0;
				return i;
			}
	}

	return -1;
}

/* Translate the encoded bar code value to a standard integer number. */
int decode_upc_A_digit(const char *digits)
{
	int i;

	/* scan the table of known codes */
	for (i=0; i<10; i++) {

			if (UPC_12digit_translation[i][0]==digits[0]&&
				UPC_12digit_translation[i][1]==digits[1]&&
				UPC_12digit_translation[i][2]==digits[2]&&
				UPC_12digit_translation[i][3]==digits[3])
			{
				return i;
			}
	}

	return -1; // unknown
}

/* compute the check digit for standard 12 number UPC codes
 * The digits given must not be in ascii!!! If the check
 * digit is included in the array, it is ignored */
int compute_checksum(const char *digits)
{
        return 10-(  (digits[0]+digits[2]+digits[4]+digits[6]+digits[8]+digits[10])*3+
                        digits[1]+digits[3]+digits[5]+digits[7]+digits[9] )%10;
}


