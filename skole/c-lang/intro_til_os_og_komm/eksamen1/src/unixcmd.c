#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "debug.h"
#include "sfork.h"
#include "unixcmd.h"


static char *envPath[NUM_PATH];

void init_paths(void)
{
/*
Initialises environment. Populates envPath vector with the
users different paths.
*/
	char *p = get_path();
	split_path(p);

	if(DEBUGFLAG){	
		int i;
		for(i=0; envPath[i] != NULL; i++)
			printf("%s\n", envPath[i]);
	}/* End if */

	free(p); 					/* Done with OsPath */

}/* END Init_Environ */

/*-----------------------------------------------------------------------*/

char *user_name(void)
{
/*	
Upon call gets user name from environment
checks if success and then returns username 
If not: Then there is not much else to do, so it exits process.
*/
	char *uname;
	if((uname = getenv("USER")) == NULL){
		debug("Failed to retrieve user name");
		exit(0);
	} /*End if */
	return uname;

}/* End User_Name */

/*-----------------------------------------------------------------------*/

char *get_path(void)
{
/*
Upon call gets PATH variable from environment
checks if success and then returns pointer to string of paths.
If not: Then there is not much else to do, so it exits process.
*/
	char *OsPath;
	char *path;
	if ((path = getenv("PATH")) == NULL){
		debug("failed to retrieve PATH-Variable");
		exit(0);
	}/* End if */
	if(DEBUGFLAG){
		debug("%s", path);
		printf("len of \"PATH\": %d\n", strlen(path));
	}/* End if */
	if ((OsPath = malloc(sizeof(char)*PATH_MAX)) == NULL){
		debug("Malloc Failed!");
		exit(0);
	}/* End if */
	strcpy(OsPath, path);

	return OsPath;

} /* End Get_Path */

/*-----------------------------------------------------------------------*/

void split_path(char *p)
{
/*
Splits PATH into paths and connects every path to
environment vector. Paths are sep. by ':' as one can see under (My os PATH):
/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl
Identical algorithm to cmdParse() except for catching the trailing path.
*/	
	int i = 0;
	int colon = 0;
	int numChar = 0;
	int numPath = 0;
	char pNum[PATH_MAX];
	
	while (*p != '\0' && numChar < PATH_MAX && numPath < NUM_PATH){
		if (*p == ':' && colon == 0){
			numChar = 0;
		} else if (*p != ':'){
			pNum[numChar++] = *p;	
			colon++;
		} else if (*p == ':' && numChar > 0){
			pNum[numChar++] = '/';
			pNum[numChar] = '\0';
			if ((envPath[numPath] = calloc(sizeof(char), numChar+1)) == NULL){
				debug("Failed to alloc memory for path");
				exit(0);
			}/* End if */
			strcpy(envPath[numPath], pNum);
			colon = 0;	
			numChar = 0;
			numPath++;
		}/* End if else */
		
		p++;
	}/* End while */
	
	if (*p == '\0' && numChar > 0){		/* If p points to a null char: */
		pNum[numChar++] = '/';
		pNum[numChar] = '\0';
		if ((envPath[numPath] = calloc(sizeof(char), numChar+1)) == NULL){
			debug("Failed to alloc memory for path");
			exit(0);
		}/* End if */
		strcpy(envPath[numPath], pNum);
	} else {
		printf("FATAL: Missing PATH environment variable!");
	}/* End allocation of unary chars */


	envPath[++numPath] = (char*)NULL;	/* Close the door */

}/* End Split_path */

/*-----------------------------------------------------------------------*/

int try_execve(char* cmd, char **prmVec,char** environ, int *amperFlag)
{
/*
Function iterates through evry path in env-"PATH" to see if there is a match on an executable bin file
in that specific directory. This is done with the help of stat(). If found; process is forked
and the executable is executed. Parent process either waits or leaves child behind depending on wether
an ampersand was combined with command.
*/
	int numPath;
	int ukcmd;
	int status;
	pid_t cProc;	
	int statReturn;
	int numTries = 0;
	char tryPath[PATH_MAX]; 
	struct stat filestat;

	debug("Flag '&' set to: %d", (*amperFlag));
	debug("motherprocess is %d", (int)getpid());

	for (numPath = 0; envPath[numPath] != NULL; numPath++){
		strcpy(tryPath, envPath[numPath]); 			/* Copy numPath into trypath temporarily */
		strncat(tryPath, cmd, strlen(cmd));					/* Append cmd to path */
		statReturn = stat(tryPath, &filestat);			/* check if file exists */

		if (statReturn == 0){ 					/* There was a match between wanted cmd and existing cmds */
			cProc = safefork(); 				/* Fork out this process */
			debug("PID: %d", (int)cProc);
									/* Childs life: */
			if (cProc == 0) {
				ukcmd = execve(tryPath, prmVec, environ); 	/* tryPath: path/cmd prmVec: arg to cmd envPath: ptr to char *arr */
				debug("Execve returned unexpectedly: %d", ukcmd);
				exit(0); 					/* Dummy in case something went wrong in exec. Normally never happens. */
			 						/* Parents life: */
			} else {				
				debug("Mother process: '%s' started in process %d", tryPath, (int)cProc);	
				debug("Mother process -> %d", (int)getpid());
				if(*amperFlag){				/* Do not wait for childs return */
					waitpid(cProc, &status, WNOHANG);
					printf("[PID] %d\n", (int)cProc);
					return 0;
				} else
				waitpid(cProc, &status, 0);		/* Waiting for child to return  */	
			} /* End If Else */

		} else {
			numTries++;
		}/* End If Else */
	}/* End For */
	if (numTries == numPath)
		printf("ifish: %s: command not found\n", cmd);
	return cProc;

}/* End try_execve */

/*-----------------------------------------------------------------------*/

void free_unixcmd(void)
{
/*
* Frees all external paths plugged into pointer array.
*/
	int paths;
	for (paths = 0; envPath[paths] != (char*)NULL; paths++){
		debug("freeing envPath: %s", envPath[paths]);
		free(envPath[paths]);
	}/* End For */

}/* End Free Unixcmd */

