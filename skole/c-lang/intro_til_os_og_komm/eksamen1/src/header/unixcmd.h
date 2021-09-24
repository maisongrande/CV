#ifndef UNIXCMD_H
#define UNIXCMD_H

#define PATH_MAX 4096 
#define NUM_PATH 100

void init_environ(void);
void init_paths(void);
char *user_name(void);
char *get_path(void);
void split_path(char*);
int try_execve(char*, char **,char**, int*);
void free_unixcmd(void);

#endif /* UNIXCMD_H */
