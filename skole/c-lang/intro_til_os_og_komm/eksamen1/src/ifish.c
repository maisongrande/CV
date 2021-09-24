/*------------------------------ 
  Macro-debug switch for global 
  use in debug.h or localy under:
-------------------------------*/
//#define debug
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unixcmd.h"
#include "mapHistory.h"
#include "shproc.h"
#include "sfork.h"
#include "debug.h"

#define MAXIN 120		/* Maximum 120 chars reads */
#define MAXARGS 21 		/* 20 words pluss NULL */

static char *params[MAXARGS]; 
static char *environ[MAXIN];


/*
*--------------------------------------------------------------*
*	MAIN FUNCTION
*--------------------------------------------------------------*
*/
int main(int argc, char **argv, char **envp)
{

	if (DEBUGFLAG)
		debug("Running in Debug Mode\n");
/*
*--------------------------------------------------------------*
*	INITIALISATIONS
*--------------------------------------------------------------*
*/
	int nRun = 1;
	int input = 0;
	int amperFlag = 0;
	int type;
	int builtIn;
	int cmdPid;
	int hist;
	char input_buf[MAXIN];			
	char histBuff[MAXIN];

	init_paths();						/* Initialise path variable that will follow ifish through out its lifetime*/
	init_history_mem();					/* Memset bitMap and History buffer */
	char *uname = user_name(); 				/* Calling func. user_name() to get current user. */
/*
---------------------------------------------------------------*
*	START SHELLING :
---------------------------------------------------------------*
*/
	printf("%s@ifish %d > ",uname, nRun);			/* Give user a prompt initially */

	for (nRun = 2; fgets(input_buf, MAXIN+1, stdin) != NULL; nRun++){ /* fgets read in at most one less than */
									  /*  MAXIN padding string with '\0' and '\n' */

		if (strchr(input_buf, '\n') == NULL)		/* if not nl-char in input_buf, means that buffer has overflowed */
			disperse_overflow();			/* and strchr returns NULL-pointer (p.249 tcpl K&R) fflush(stdin) has */
								/* undefined behavior as I undestand it and is discuraged */

		cmlParser(input_buf, params, &amperFlag);	/* Parse command line into separate strings */

		if(DEBUGFLAG){
			printParams(params);			/* Echo commands if debug switch is on */
			debug("IFISH READ: %sSize: %d", input_buf, strlen(input_buf));
		}
			
		if (params[0] != NULL){				/* As long as user has typed in a character other than newline: */

			history_handler(input_buf);		/* Add command to history tree */

			builtIn = checkBuiltins(params[0]);	/* Check if it's one of the built in commands. */
			if (builtIn == 0){			/* User typed quit or exit */
				debug("case: quit or exit");
				free_unixcmd();			/* Free cmd vector */
				freeParams(params);		/* Free param vector */
				free_history(head);		/* Free cmd list */
				exit(0);
			} else if (builtIn == 1){		/* User wants history returned */
				debug("history");
				memset(&histBuff, 0, MAXIN);	/* clear out shrapnel stuck in buffer */
				hist = history_parse(params, nRun, histBuff);
				if(hist == 0){
					freeParams(params);		/* Free param vector since a new is in the making */
					cmlParser(histBuff, params, &amperFlag);	/* Parse this new cmd */
					if (DEBUGFLAG) printParams(params);		/* And try to execute proc */
					cmdPid = try_execve(params[0], params, envp,  &amperFlag);
				}/* End If Hist exec */

			} else if (builtIn == 2){				/* User typed 'type' */
				debug("Type");

				if (params[1] != NULL){				/* Test if there is a trailing string to cmd */
					type = checkBuiltins(params[1]);	/* re-run builtins-check  */
					switch(type){
						case 0: case 1: case 2:
							printf("%s is a shell builtin\n", params[1]);
						break;
						default:	
							printf("ifish: type: %s: not found.\n", params[1]);	
						break;
					}/* End Switch Types */
				}/* End If NULL-string */

			} else {						/* User has typed in an extern cmd */

				cmdPid = try_execve(params[0], params, envp,  &amperFlag);

			}/* End if else */		
		}/* end if */

		freeParams(params);				/* Free params-vector */
		printf("%s@ifish %d > ", uname, nRun);		/* Give user prompt back */


	} /* End shelling */
	
	/* In case of fallthrough or CTRL-D by user*/
	free_history(head);
	free_unixcmd();
	return 0;

/*------------------------*/
}/* END  IFI-SHELL */
/*------------------------*/
