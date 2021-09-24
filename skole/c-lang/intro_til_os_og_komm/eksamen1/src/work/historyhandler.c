/*----- HISTORY MASTER --------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapHistory.h"
#define DEBUG
#include "debug.h"

void init_history_mem(void)
{
/*
Done as precaution at initialisation of ifish. Zeroing out all 
bytes in both Bit Map and Histoy Buffer.
*/
	memset(&historyBuffer[BYTEBLOCKS*BLOCKSIZE], 0, BYTEBLOCKS);
	memset(&bitMap[(BYTEBLOCKS+1)/BLOCKSIZE], 0, BLOCKSIZE);
}

/*--------------------------------------------------------------------*/

int history_handler(char *command)
{
/*
 As name implies this function handles most of the over-head work 
 regarding allocating commands into memory. Function calls and var names
 are at least tried to be declared in a way that leaves out the 
 necessity of exessive commenting.
*/
	struct history* newHist;	/* New command needs a node */
	int i = 0;
	int cmdLength;
	int numBlocksNeeded;
	int numBlocksFree;
	int numZbit;
	int bitMcpy;

	int bitNum;
	int byteNum;
	int consecByteNum;

	int allocBlocks[MAXBLOCKS] = {0};	/* Init two buffers, one for the number representing */
	char buffer[CMDMAX] = {0};		/* blocks 0-504 and one for the cmd. Both init to 0. */

	if (strlen(command) >= CMDMAX || command[0] == '\n'){	/* Not likely in this senario. Redundant */
		return 0;
	}
	strncpy(buffer, command, (strlen(command)-1));	/* -1 to remove 0 termination strncpy doesnt pad */

	cmdLength = strlen(buffer);
	debug("length: %d", cmdLength);

	/* Next: get correct number of blocks for str. Add one block if there is a remainder in div */
	numBlocksNeeded = (cmdLength % BLOCKSIZE) ? cmdLength/BLOCKSIZE+1:cmdLength/BLOCKSIZE;
	debug("Number of blocks needed: %d", numBlocksNeeded);

	numBlocksFree = count_free_blocks(bitMap);
	debug("num Blocks Free: %d", numBlocksFree);

	/*---------As long as not free blocks:---------*/

	while (numBlocksNeeded > numBlocksFree){
		delete_first_command();
		numBlocksFree = count_free_blocks(bitMap);
	}/* End while */

	/*----------Now we got free space--------------*/

	for (i = 0; i < numBlocksNeeded; i++){
		consecByteNum = find_free_blocks(bitMap, &byteNum, &bitNum);	
		debug("Found following: %d, %d", byteNum, bitNum);
		set_bits_map(byteNum, bitNum);
		allocBlocks[i] = consecByteNum;
	}/* End For */

	copy_into_blocks(historyBuffer, buffer, allocBlocks, numBlocksNeeded);
	newHist = make_history(cmdLength, allocBlocks, numBlocksNeeded);
	insert_to_history(newHist);	


	/*---------Done inserting, now return ---------*/


	if (DEBUGFLAG){
		/* Writes out a map of both the History buffer and the corresponding Bit Map */
		int i = 0;
		char c;
		int dbgIt;
		printf("\nDEBUG - BITMAP:\n\n");
		for (dbgIt = 0; dbgIt < BYTEBLOCKS; dbgIt++){
			printf("%d%s", print_bitmap(dbgIt, bitMap), (dbgIt%32 == 31)?"\n":"");
		}/* End Bitmap Print */
	
		printf("\nDEBUG - DATABLOCKS:\n\n");
		for (dbgIt = 0; dbgIt < BYTEBLOCKS*BLOCKSIZE; dbgIt++){
			c = (char)historyBuffer[i++];
			printf("%s%c%s",(dbgIt%32 == 0 && dbgIt != 0)?"##\n##":(dbgIt%8 == 0)?"##":"", c =='\0'?' ':c,(i ==512)?"##\n":"");
		}/* End History Buffer Print */

	}/* End Debug */
		

	return 0;

}/* End history handler */

/*-----------------------------------------------------------------------------------------------------------*/

int history_parse(char** args, int present, char *toBexec)
{
/*
Function takes care of handeling processes that are binded to the command 'h'.
It makes sure that the correct amounts of arguments are passed along with 
the 'h' option, if any. Then it passes the execution of arguments to where it belongs.
*/

	char *histOpts[]= {"-d",""};				/* Token holder for current and future options. */

	int i;
	int cmdNo;
	int test;

	if (args[1] != NULL && args[2] == NULL){		/* We have either " h '1234...89' " or "h 'gibberish' */

		for (i = 0; i < strlen(args[1]); i++){		/* Test every byte for traps in the form of non digits */

			if (isdigit(args[1][i]) == 0){ 
				printf("ifish: %s %s: command not found\n", args[0], args[1]);
				return -1;
			}/* End If Else */

		}/* End For */

		cmdNo = atoi(args[1]);				/* We are sure to have only digits, convert. */
		if (cmdNo >= count_curr_cmds() || cmdNo == 0){	/* Time traveller?  */
			printf("Impossible to execute this or any commands not yet typed!\n");
			return -1;
		} else {
			execve_list_elem(toBexec, cmdNo);
			return 0;		/* Zero tells caller that there are exec waiting in buffer */ 
		}

	} else if (args[1] != NULL && args[2] != NULL){

		/* Test to see if user typed in an built in option (next test only for opt -d */
		test = strcmp(args[1], histOpts[0]);		
		
		/* First barrier. Not an option supplied */
		if(test != 0){
			printf("ifish: %s %s %s: command not found\n", args[0], args[1], args[2]);
			return -1;
		}

		/* Second barrier. Only digits supplied? */
		for (i = 0; i < strlen(args[2]); i++){
	
			if (isdigit(args[2][i]) == 0){
				printf("ifish: %s %s %s: command not found\n", args[0], args[1], args[2]);
				return -1;
			}/* End For */
		} /* End For */

		/* Only digits. Now convert from string to integer */
		cmdNo = atoi(args[2]);

		/* Third barrier. Have we got a time traveller on our hands? */
		if (cmdNo >= count_curr_cmds() || cmdNo == 0){
			printf("Impossible to delete this or any commands not yet typed!\n");
			return -1;
		} else {
			delete_nth_cmd(cmdNo);		/* User has been good. Now delete! */
			return 1; 
		}

	} else {
		/* Write out history: */
		write_all_history();
		return 1;
	}

	debug("FAIL: Fell through all tests!");
	return -1;	

}/* End history Parse */


