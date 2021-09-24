#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

/*-----------------Local Variables to shproc.c --------------------------*/

char* cmdTokens[] = {"quit", "exit", "h", "type"};
typedef enum {QUIT, XIT, HISTORY, TYPE} builtins;

/*--------------------- Function Definitions ----------------------------*/

void disperse_overflow(void)
{
/* 
Function clears out the overflowing stdin. This prevents recoils
in ifish from non processed chars in stdin buffer if user types 
more than set amount of allowed chars.
*/
	char ch;

	while ((ch = getchar()) != '\n')
		;

}/* End Disperse Overlow */

/*-----------------------------------------------------------------------*/

void cmlParser(char *cmltext, char **prmPtr, int *amperFlag)
{
/*
 Function parses up recieved string from user into separate substrings.
 Thorough explanations follows each line.
*/	
	int argCount = 0; 
	int character = 0;
	int whiteSpace = 0;
	char temp[120];

	while (*cmltext != '\0' && argCount < 21){	
		if (isspace(*cmltext) && whiteSpace == 0){ 		/* ignorewhite space, but reset character counter. */
			character = 0;
			
		} else if (isspace(*cmltext) == 0) {			/* isspace returns 0 when char is outside class aka we got a char*/
			if((*amperFlag) = (*cmltext == '&'? 1 : 0))	/* Test if '&'. If so, set flag, remove '&' from buffer while at it */
				debug("AmperFlag=%d\n", (*amperFlag));
			 else {
				temp[character++] = *cmltext;		/* Add to temp buffer */
				whiteSpace++;				/* Incr whitespace to block statment for re-setting char counter */
			}
		} else if(isspace(*cmltext) && character > 0){		/* We got another white space, and char-counter > 0, aka read past a word. */
			temp[character] = '\0';					/* Since character is post-inc it is safe to terminate with null */
			prmPtr[argCount] = malloc((character+1)*sizeof(char));	/* connect pointer to allocated memory to param-vector*/
			if (prmPtr[argCount] == NULL){				/* The std imortant test */
				debug("Failed to allocate parameter");
				exit(EXIT_FAILURE);
			} /* End allocation check */

			strncpy(prmPtr[argCount], temp, (character+1));	/* Copy the word into allocated memory incl termination */

			whiteSpace = 0;					/* Reset whitespace counter and char counter since we are done */
			character= 0;
			argCount++;					/* Inc argument counter */
		  } /* End if */

		cmltext++;						/* Moving pointer one byte up */

	}/* End while */

	prmPtr[argCount] = (char*)NULL; 				/* Shut the door before leaving */

}/* End command line parser */

/*-----------------------------------------------------------------------*/

int checkBuiltins(char *cmdPtr)
{
/*
Iterates over built in commands.
Compares incoming string with built in commands, if found returns corresponding
flag. Else return -1.
*/
	int cmdIter;
	int result;

	for(cmdIter = 0; cmdIter < sizeof(cmdTokens)/sizeof(cmdTokens[0]); cmdIter++){
		if ((result = strcmp(cmdPtr, cmdTokens[cmdIter])) == 0){	
			switch (cmdIter){
				case QUIT: case XIT:
					return 0;
					break;
				case HISTORY:
					return 1;
					break;
				case TYPE:
					return 2;
					break;
				default:
					debug("default");
					return -1;
					break; 				/* No break needed, but kept as good practice. */
			}/* End switch */
		} /* end if */
	} /* end for */
}/* End checkBuitltins*/

/*-----------------------------------------------------------------------*/

void printParams(char **prmPtr)
{
/*
* Prints out all strings in array (until NULL is reached)
*/
	int i = 0;
	while (prmPtr[i] != NULL){
		printf("%s\n", prmPtr[i]);
		i++;
	}/* End while */

}/* end printParams */

/*-----------------------------------------------------------------------*/

void freeParams(char **prmPtr)
{
/*
Iterates over addresses pointed to by prmPtr and frees the pointers associated
with that address.
*/
	int numArgs;
	for(numArgs = 0; prmPtr[numArgs] != (char*)NULL;  numArgs++){
		debug("freeing param: %s", prmPtr[numArgs]);
		free(prmPtr[numArgs]);
	}

}/* End freeParams */

/*-----------------------------------------------------------------------*/

