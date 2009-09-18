#ifndef _BCD_CODER_
#define _BCD_CODER_

extern unsigned char intToBCD_table[];
extern unsigned char BCDToInt_table[];

#define intToBCD(c) intToBCD_table[c]
#define BCDToInt(c) BCDToInt_table[c]

#define btoi(b)		BCDToInt_table[b]		/* BCD to u_char */
#define itob(i)		intToBCD_table[i]		/* u_char to BCD */

#endif
