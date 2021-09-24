/* Macro for debugging as described in the assignement */
#ifndef DEBUG_H_
#define DEBUG_H_

//#define DEBUG		/* DEBUG SWITCH. UNABLE TO MAKE GNU MAKE TALK TO MACRO :( */


/* If compiled with DEBUG the debug macro is an fprintf with args*/
#ifdef DEBUG
#define debug(msg, ...) fprintf(stderr, "DEBUG: In: %s Line: %d Func: %s(): " msg "\n", __FILE__, __LINE__,__func__, ##__VA_ARGS__)
#else 
#define debug(msg, ...)
#endif /* End macro def */

#ifdef DEBUG
#define DEBUGFLAG 1
#else
#define DEBUGFLAG 0
#endif /* End variables*/

#endif /* DEBUG_H */
