/*------------- HISTORY SLAVE ---------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define DEBUG
#include "debug.h"
#include "mapHistory.h"

/*-----------------------------------------------------------------------------------------*/

int find_free_blocks(unsigned char *bMap, int *x, int *y)
{
/*
Function Searches after zero-bits in bitmap. first instance is returned translated into
corresponding block in History Buffer. Two ints belonging to caller is also set corresponding
to the bit and byte number.
*/
	unsigned char fullMask = 0xFF;
	unsigned char bitBuf = 0x00;
	int byteN;
 	int bit;
	
	for (byteN = 0; byteN < BLOCKSIZE; byteN++){
		bitBuf = bMap[byteN];				/* Copy bitMap into buffer. */
		if ((bitBuf & fullMask) == fullMask)		/* If this buffer is full skip. */
			continue;
		for (bit = 0; bit < 8; bit++){			/* if not shift 1 into byte until 0 is found. */
			if(!(bitBuf & 1<<(bit))){		/* When found return where, corresponding to pattern. */
				debug("byte %d  bit %d",byteN, bit); 
				*x = byteN; 			/* Polpulate ints belonging to caller with bit number */
				*y = bit;			/* bytenumber. Then return calculated byte 0,8,16...504 */

				return (64*byteN + 8*bit); 
			}
		}/* End Inner For*/
	}/*End Outer For */	
	return -1;
}/* End Find Free Blocks */

/*-----------------------------------------------------------------------------------------*/

int count_free_blocks(unsigned char* bM)
{
/*
Sums up all set bits, and returns byte minus set bits. 
Based upon exercise 2-9 p.51 in TCPL(K&R).
*/

	unsigned char bmCpy;
	int nZbits = 0;
	int sumZero = 0;
	int index = 0;

	while (index < BLOCKSIZE){
		bmCpy = bM[index];			/* As long as there are bits in byte: */
		for (nZbits = 0; bmCpy; nZbits++){	/* bit0 = 1? then bit0 of x - 1 = 0   */
			bmCpy &= (bmCpy - 1);		/* Else if right most bits of x is = 0 bits 0-x(1)of*/
		}					/* x - 1 = are inverted. The and operation */
		sumZero += (8-nZbits);			/* will clear out bits not set  until all zero */
		index++;				/* Return bits in a byte minus num found */
	}

	return sumZero;

}/* End Count Free blocks */	

/*-----------------------------------------------------------------------------------------*/

void set_bits_map(int x, int y)
{
/*
Function uses makro defined in header to set (XOR) bits in bit map. 
*/

	BITSWITCH(bitMap[x], y);

}/* End Set Bits in Map */

/*-----------------------------------------------------------------------------------------*/

void clear_blocks(struct history* cmd)
{
/*
With some arithmetics, bit-number in byte-number is found while
unrolling the array elements owned by the history-node.
Bits are flashed in the bitMap and then the corresponding bytes in 
History buffer is sett to zero.
*/
	int i;
	int bit;
	int byte;
	int nBlocks; 
	nBlocks = (cmd->length % BLOCKSIZE)?((cmd->length/BLOCKSIZE)+1):(cmd->length/BLOCKSIZE);

	for (i = 0; i < nBlocks; i++){
		byte =  cmd->occupy[i] / BYTEBLOCKS;
		bit = (cmd->occupy[i] % BYTEBLOCKS) / BLOCKSIZE;
		BITSWITCH(bitMap[byte], bit);
		memset(&historyBuffer[cmd->occupy[i]], 0, BLOCKSIZE);
	}/* End For */


}/* End Clear Blocks */

/*-----------------------------------------------------------------------------------------*/

void copy_into_blocks(unsigned char* histBuffer, char* buff, int *blocks, int len)
{
/*
Function copies string of chars into History Buffer in chunks of eight
bytes at a time.
*/

	int i;
	int j;
	int k;
	int next = 0;

	for (i = 0; i <len; i++){
		for (j = 0; j < 8; j++){
	
			strcpy(&histBuffer[ blocks[i] + j ], &buff[ next++ ]);
	
		}/* End Inner For*/
	}/* End outer For */

}/* End Copy Into Blocks */

/*-----------------------------------------------------------------------------------------*/

int print_bitmap(int blockNum, unsigned char *bm)
{
/*
 Ping-Pong with caller. 
 Caller sends block number from 0 - BYTEBLOCKS-1 (or catch -1 in test)
 and gets bit on/off back. Referencing bitMap. 
 If overflow returns -1.
*/
	int bitNo;
	int byteNo;
	int retVal = 0;
	
	unsigned char bitmask;

	if (blockNum >= BYTEBLOCKS){
	       retVal = -1;
	} else {
 		byteNo = blockNum / 8;
		bitNo = blockNum % 8;
		bitmask	= 1<<bitNo;
		if ((bm[byteNo] & bitmask) != 0)
			return retVal = 1;
	}/* End If Else */
	return retVal;

}/* End Print Bitmap */
