#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define DEBUG
#include "debug.h"
#include "mapHistory.h"


struct history *head = (struct history*)NULL;	/* Initial pointer to history list */
struct history *tail = (struct history*)NULL;	/* Initial pointer to history list */

/*-------------------------------------------------------------------------------------*/

struct history* make_history(int len, int *blocks, int numBlocks)
{
/*
Function initialises a node/cmd-history for insertion/enqueuing into
history list. It allocates memory the size of struct and assignes the
proper values for this command.
*/
	int i;
	struct history *command; 
	if ((command = malloc(sizeof(struct history))) == NULL){
		debug("Allocation of command into history failed");
		exit(0);
	}
	
	command->length = len;
	for (i = 0; i < numBlocks && i < MAXBLOCKS; i++){ 
		command->occupy[i] = blocks[i];
		debug("Copied: %d", command->occupy[i]);
	}
	command->next = NULL;

	return command;

}/* End Make History */

/*-------------------------------------------------------------------------------------*/

void insert_to_history(struct history *thisCmd)
{
/*--------- Enqueue ------------------------------------------*
Idea here is to insert new command into the back of the line. This 
ensures that enque and deque has the same constant time. We don't
have to move all the way back O(n) when deque.
*/
	if (head == NULL && tail == NULL){	/* First instance of command from user */
		head = tail = thisCmd;		/* Both ends points to same cmd*/
		return;
	} else {				/* Current rear points to this cmd */
		tail->next = thisCmd;		/* now this cmd is new tail */
		tail = thisCmd;	
	} /* End If Else */

} /* End Insert to History */


/*-------------------------------------------------------------------------------------*/

int count_curr_cmds(void)
{
/* 
*Traverse list while summing up number of nodes encountered.
*/
	int sum = 0;
	struct history* temp = head;

	while (temp != NULL){
		sum++;
		temp = temp->next;
	}

	return sum;

}
/*-------------------------------------------------------------------------------------*/

void delete_first_command()
{
/*-------- Dequeue --------------------------------*
Functions deletes what ever head is pointing to, or
in the (futuristic) event that there is only one 
cmd (head = tail) taking up all the space in the buffer.
*/
	struct history* temp = head;

	if (head == NULL){
		debug("Deleting from empty list!");
		exit(0);
	}/* End If */

	if (head == tail){
		clear_blocks(tail);		/* Send heads cmd-block to be cleaned out */
		head = tail = NULL;
	} else {
		head = head->next;
	}/* End If Else */
	clear_blocks(temp);			/* Send heads cmd-block to be cleaned out */
	free(temp);				/* Free old head node */

}/* End Delete first command */

/*-------------------------------------------------------------------------------------*/

void delete_nth_cmd(int pos)
{
/*
Deletes stored history at the nth(pos) position.
*/
	int i;					
	int first = (count_curr_cmds()-1);		/* In lack of pointer to prev.. */
	struct history* temp1 = head;			/* Temp pointer to head node */
	struct history* temp2; 

	if (pos == first){				/* User want's to delete first entered command from mem */
		clear_blocks(temp1);			/* Send heads cmd-block to be cleaned out */
		head = temp1->next;			/* move head to the next block */
		free(temp1);				/* free this block */
	} else {
		for (i = 0; i < (first - (pos+1)); i++){	/* Move to correct cmd */
			temp1 = temp1->next;
		} /* End For */
		temp2 = temp1->next;			/* make pointer to next */
		temp1->next = temp2->next;		/* temp1 skips the one in middle */
		clear_blocks(temp2);			/* Send doomed cmd-block to be cleaned out */
		free(temp2);				/* Free middle */ 

	} /* End If Else */

}/* End Delete Nth Cmd */

/*------------------------------------------------------------------------------------*/

char* execve_list_elem(char *retStr, int pos)
{
/*
Up on call function populates blocks pointed to by retStr with previous command and returns pointer
to this area back to caller (catchable if needed).
*/
	int i;
	int j;
	int len;
	int nblk;
	int nodeNo = 0;	
	int toBexec = count_curr_cmds() - (pos+1);			/* Current - wanted + offset cause of this insert. */
	struct history* temp = head;
	
	char ch;

	while (temp != NULL && nodeNo != toBexec){			/* Move forward until match */
		nodeNo++;
		temp = temp->next;
	}/* End Traversal While */

	len = temp->length;
	nblk = (len % BLOCKSIZE)?((len / BLOCKSIZE)+1):(len/BLOCKSIZE); /* modulo and test quotient, add one if not zero */

	for (i = 0; i < nblk; i++){

		for(j = 0; j < 8 && (i+j) < len; j++){			/* populate buffer recieved from caller with  */
			ch = (char)historyBuffer[temp->occupy[i]+j];	/* contents in the nodes block-array. */
			*retStr = ch;
			retStr++;
		}/* End Inner For */

	}/* End Outer For */
	*retStr++ = '\n'; 

	return retStr;

}/* End Execve List Elem */

/*------------------------------------------------------------------------------------*/

void write_all_history()
{
/* Print out every byte belonging to each cmd, starting at end of 
queue. Gets length of cmd, and with that calcs num blocks belonging
to cmd.
*/
	int i;
	int j;
	int len;
	int nblk;
	int k = 0;
	int count = count_curr_cmds();

	char ch;
	char str[CMDMAX] = {0};			/* Set buffer to zero */

	struct history* temp = head;

	printf("\nHistory list of the last %d command%s:\n\n", count, count == 1?"":"s");
	while (temp != NULL){
		printf("%3d:  ", count); 
		len = temp->length;
		nblk = (len % BLOCKSIZE)?((len / BLOCKSIZE)+1):(len/BLOCKSIZE); /* Get num allocated blocks based on cmd len */

		for (i = 0; i < nblk; i++){					/* Loop through num byte blocks */

			for (j = 0; j < 8 && k < len; j++){			/* and store them consecutively in buffer */
				ch = (char)historyBuffer[temp->occupy[i]+j];	/* where each ch is the byte stored in histBuf */
				str[k++] = ch;					/* referenced through cmd nodes value in array */
			}/* End Inner For */

		}/* End Outer For */
						/* NOW: */
		str[k] = '\0';			/* Pad with null-termination */
		printf("%s\n", str);		/* Print result */
		memset(&str, 0, CMDMAX);	/* Reset buffer */
		k = 0;				/* Reset pointer */
		temp = temp->next;		/* Go to next cmd */
		count -= 1;			/* decr. counter */
	}/* End while */

}/* End write all history */

/*-------------------------------------------------------------------------------------*/

void free_history(struct history *ptr)
{
/*
Delete and free all commands in the list.
Done at exit of program.
*/
	struct history* temp;

	if (head = NULL){
		debug("Head node lost some how");
		exit(1);
	}

	head = tail = NULL; 			/* Delete link between process and list */

	while (ptr != NULL){			/* Now delete all other nodes recursively until */
		temp = ptr->next;		/* last node is reached. Then we're done. */
		free(ptr);			/* But remember to keep track of next node if any */
		ptr = NULL;			/* Before freeing. */
		ptr = temp;
	}/* End While */

}/* End Free History */

/*-------------------------------------------------------------------------------------*/

