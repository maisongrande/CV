
#ifndef MAPHISTORY_H
#define MAPHISTORY_H

#define BITSWITCH(a,b) ((a) ^= (1<<(b))) 
#define MAXHIST 10
#define MAXBLOCKS 15
#define BYTEBLOCKS 64
#define BLOCKSIZE 8
#define CMDMAX 120

unsigned char historyBuffer[BYTEBLOCKS * BLOCKSIZE];  
unsigned char bitMap[(BYTEBLOCKS + 1) / BLOCKSIZE];	/* +1 Added in case div ends in zero */


/*-------------------------------------------*
------------------ LINKHIST -----------------
------ Definitions & Function prototypes ----
*--------------------------------------------*/

struct history{
/* This struct represent one command. */   
	int length;				/* Place holder for the length of the string contained in on cmd. */
	int occupy[MAXBLOCKS];			/* Each element in structs array corresponds to eight 8byte in hist buf. */
	struct history* next;			/* Needs a pointer to next element i cmd-history list */
};

struct history *head;				/* Init and declaring a struct pointer to front of history*/
struct history *tail;				/* Init and declaring a struct pointer to end of history */

struct history* make_history(int, int*, int);
void insert_to_history(struct history *thisCmd);
int count_elems(void);
void delete_first_command();
void delete_nth_cmd(int cmd);
char* execve_list_elem(char*, int);
void write_all_history();
void free_history(struct history *ptr);

/*-------------------------------------------*
*----------- HISTORY HANDLER ----------------
---------- Function prototypes --------------
*-------------------------------------------*/

void init_history_mem(void);
int history_handler(char *);
int history_parse(char**, int, char*);

/*-------------------------------------------*
------------- HISTORY SLAVE -----------------
--------- Function prototypes ---------------
*-------------------------------------------*/

int find_free_blocks(unsigned char *bMap, int *x, int *y);
int count_free_blocks(unsigned char* bMap);
void set_bits_map(int, int);
void clear_blocks(struct history*);
void copy_into_blocks(unsigned char*, char*, int*, int);
int print_bitmap(int, unsigned char*);

#endif /*-- MAPHISTORY --*/ 
