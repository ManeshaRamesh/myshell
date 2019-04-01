#ifndef SHELL_H
#define SHELL_H_

#include <stdio.h>
#define CONTINUE 1
#define BEGIN_COUNT 0
#define SUCCESS 1
#define FAILURE 0
#define DIR_PATH 500
//define structs for linked list to store application variables
typedef struct sysPath sysPath;
typedef struct sysPath_list sysPath_list;
//list of all functions
void syspath_linkedlist_destructor();
void sysPath_createlist(char* variable, char* value);
void display_list();
sysPath* search_sysPathList(char* variable);
char* read_input();
char** parse(char* input, FILE* fstream);
int execute(char** parsed, FILE* fstream);
int execute_pipes(char ** pipes_cmds, int pipe_count, FILE* fstream);
int storeInHistory(char* input, FILE* fstream);
struct sysPath* path_variable_create(char* variable, char* value);
void sysPath_addtolist(sysPath* Node);
void update_variable(char* variable, char* value);
char* convert_command(char * input, FILE* fstream);

#endif
