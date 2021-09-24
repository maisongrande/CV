#ifndef SHPROC_H
#define SHELPROC_H

void disperse_overflow(void);
void cmlParser(char*, char**, int*);
int checkBuiltins(char*);
int test_cmds(char**);
void printParams(char **);
void freeParams(char**);

#endif /* SHPROC_H */
