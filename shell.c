#include <stdio.h>
#include "shell.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <wait.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE_ARGS 50
#define READ 0
#define WRITE 1
#define EXIT 0
#define ERROR 0
#define CONTINUE 1
#define ENV_VAR_SIZE 256
#define ENV_VAL_SIZE 2000
#define DIR_PATH 500
#define LINE_SIZE 200
#define BEGIN_COUNT 0
#define FD_PIPE 2
#define FALSE 0
#define TRUE 1
#define SUCCESS 1
#define FAILURE 0
#define NONE 0

//global variables
char* builtin_commands[] = {"cd","pwd", "export", "history", "exit"};
int commands_array_size = sizeof(builtin_commands)/8;

//defining data structure(double linked lists) for system with variables
//defining a node aka a struct with the variable name and value name
struct sysPath{
	char variable[ENV_VAR_SIZE];
	char value[ENV_VAL_SIZE];
	sysPath* next;
	sysPath* prev;
};
//defining a doubly linked list for system path variables
struct sysPath_list{
	sysPath* head; 
	sysPath* tail; 
}list;
//function to remove the first node right next to the head (will be called recursively in teh destructor function
void rmv_first(){
	sysPath * cur=list.head;
	sysPath *Node = cur->next;
	Node->next->prev = cur;
	cur->next = Node->next;
	free(Node);//free the node
}


//the destructor function that frees the linked lists and space allocated for the head node and teh tail node
void syspath_linkedlist_destructor(){
	while(list.head->next!=list.tail){
		rmv_first();//removing the first node recursively
	}
	free(list.head);//free the head node
	free(list.tail);//free the tail node
}

//function to add a Node to the doubly linked list
void sysPath_addtolist(sysPath* Node){
	//joiing links to the the head and teh node that comes after it
	Node->next = list.head->next;
	Node->prev = list.head;
	list.head->next->prev = Node;
	list.head->next = Node;
}

//creates a node with variable and value
struct sysPath* path_variable_create(char* variable, char* value){
	sysPath* object = (sysPath*)malloc(sizeof(sysPath));//allocating memory for object
	strncpy(object->variable, variable, sizeof(object->variable));//copying passed strings into the object
	strncpy(object->value, value, sizeof(object->value));//same as above
	return(object);//returning the object
}

//this is like a constructor - it creates the list 
void sysPath_createlist(char* variable, char* value){
        list.head =(sysPath*)malloc(sizeof(sysPath));//allocating memeory for head and tail(below)
        list.tail =(sysPath*)malloc(sizeof(sysPath));
        list.head->next = list.tail; //the list is empty for now so the head and and tail point to each other
	list.head->prev = NULL;
	list.tail->prev= list.head;
	list.tail->next = NULL;
	sysPath* Node = path_variable_create(variable, value);//create a node
        sysPath_addtolist(Node);//add the node to the list        
}

//displaying the the variables and values in the list of system_path variables
void display_list(){	
	//cursor to iterate through the list
	sysPath* cur = list.tail->prev;
	while(cur != list.head){
		fprintf(stdout,"%s= %s\n", cur->variable, cur->value);//prints the variable and the value
		cur= cur->prev;
	}
}
//searches for a variable in the list of system variables in case the user wants to modify the value of an existing varibale in the system ath
sysPath* search_sysPathList(char* variable){
	sysPath* cur = list.tail->prev;//searches of the passed variable exits in the system variables
	while(cur != list.head){
		if(!strcmp(cur->variable, variable)){
			return(cur);//if it exists it returns the node with the variable
		}
		cur = cur->prev;
	}
return(NULL);//or else it returns a NULL pointer
}
//when the user adds a new path variable it gets adde to the linked list
void add_variable(char* string, sysPath_list* list){
	char* variable = strtok(string, "=");//it takes the user string and divides tit into varibale and value 
	sysPath*  exists = search_sysPathList(variable);//searches if the input variable already exists
	char* value = strtok(NULL,"");
	//is user passes an empty variable
	if(value==NULL){
		value="";
	}
	//if the variable doesn't exist add variable to linked list
	if(exists ==NULL){
		sysPath* OneMore=path_variable_create(variable, value);
		sysPath_addtolist(OneMore);
	}
	//else modify value of the existing variable
	else{
		strcpy(exists->value, value);
	}
}
//function call for export command
void export_path(char**args){
	//if there are no arguments
	if( args[1] == NULL){
		//printf("there are no agruments");
		display_list(&list);		
	}
	//if there are arguments
	else{
		//printf("there are arguments");
		add_variable(args[1], &list);
	}

}

//function call for reading user input in the shell
char* read_input(FILE* fstream){
	char* input;//where the input is stored
	size_t size_input = 0; //a vraible to store the lenght of the input as required by the getline function
	getline(&input, &size_input, stdin);//reads input from the user
	//if the user enters !2 or !9 to get a command from history
	if (input[0] == '!'){
		char* num_string = strtok(input, "!\n");//get the nuumber
		int j;
		//printf("%d", strlen(num_string));
		for(j =BEGIN_COUNT;j<strlen(num_string);j++){ //in case ! is follwed by some random letters instead of digits
			if(!isdigit((unsigned char)num_string[j]))
			{
				printf("Error: Event not found\n"); 
				return("");
			}//check is each character after the ! is a digit
		} 
			int num = atoi(num_string);//change the number into a integer
			int i =0;
			//fptrFind = fopen("history.txt", "r");//ope the history file
			rewind(fstream);
			while(i<num){//loop to iterate through the ist of commands in the file
				if(!fgets(input, 200, fstream)){//if file stream cannot read from file, print that even is not found in teh history file
					printf("Error: Event not found\n");
					return("");
				}
				i++;			
			}

			//fclose(fptrFind);
	
	}	
	return(input);//returns the input to the main function
}
char** parse(char* input, FILE* fptr){
	//FILE *fptr = fopen("history.txt", "a");
	if(fptr !=NULL){
		if (!feof(fptr)){
			fseek(fptr, 0, SEEK_END);
		}
		fprintf(fptr,"%s",input);
	}
	if (input == NULL){
		return NULL;
	}
	//fclose(fptr);
	char * arg;// to store each argument of teh prrsed string
	char ** args;// this stores all teh srguments obtained in the string
	int i = 0;// the counter for the dynamica array
	args = malloc(sizeof(char*)*SIZE_ARGS);	// allocated memory space for the array
	arg = strtok(input, " \n");//using teh standard function to split a string stores teh first work in arg
	while(arg !=NULL){//the function loops until it reches teh last word
		args[i] = arg;//stores each word in the string in the iterating array
		arg = strtok(NULL," \n");//iterates through each word in the string and replaces teh whitespaces with a \n
		i++;//counter
	}
	args[i] = NULL;//the last pointer in the array points tp NULL to mark teh end of teh array
	return args;//returns the array
}

//utility function that returns the size of the user input array
int size(char **array){
	int i = 0;
	while( array[i] != NULL){
		i++;	
	}
	return i;
}

//checks if command is built in
int builtin_check(char ** parsed){
	int i;
	for(i = BEGIN_COUNT; i<commands_array_size; i++){	//for loop to iterate through the global array of caommand caribales
		if (strcmp(builtin_commands[i], parsed[0])==0){
			return i;
		}
	}		
	return(-1);//return -1 if the the user input is not a builtin command
}
//function for changing directory
int change_directory(char**parsed){
	//if no argument is given
	if (parsed[1] == NULL){
		if(chdir(getenv("HOME"))==-1){
			printf("Error: Cannot go to given path\n");
			return(ERROR);
		}
	}
	//if there aree arguments
	else{
		if(chdir(parsed[1])== -1){//system call
			printf("Error: Cannot change directory to given path\n");
			return(ERROR);
		}		
	}
	//if there is an argument
	//if there is an error
}

//function call for printing working directory
void print_working_directory(){
	char cwd[DIR_PATH];//space allocation for 5o00 characters
	getcwd(cwd, sizeof(cwd));//system call to print current working directory
	fprintf(stdout,"%s\n", cwd);
}
//function call to history command
void print_history(FILE* fstream){
	
	char line[LINE_SIZE];
	int i = 1;
	//opens history file
	//FILE *fstream = fopen("history.txt","r");
	//read from file and print
	rewind(fstream);
	while(fgets(line, LINE_SIZE, fstream) !=NULL){	
	//	printf("printing");
		fprintf(stdout,"\t%d %s", i, line);
		i++;
	}
	//printf("^");
	//close file stream
	//fclose(fstream);
}
//search if command is an external command
void search_sysPath(char** args){
	char* file_paths =malloc(sizeof(char*)*300); 
	strcpy(file_paths, list.tail->prev->value);
	char* path = strtok(file_paths, ":\n");//the file paths are stored with ':' as a delimiter	
	char file_path[DIR_PATH];
	int i_check;

	while(path!=NULL){
		strcpy(file_path, path);
        	strcat(file_path, "/");//concatenat the command executive file
        	strcat(file_path,args[0]);
		if(access(file_path, X_OK)==0){
			//if the executive fle for the command exists in the file path, then print the following message
			//if(fork() == 0){
				execvp(file_path, args);
				exit(EXIT);
			//}
			//else wait(NULL);
			//return;
		}
		path =strtok(NULL, ":\n");
		
	}
	//if nothing works out, command not found
	printf("Command not found\n");
}

//the funtion that determines which function to acll for any user command that the user enters
int getCommand(char** parsed, int command_num, FILE* fstream, int piped){
	int status_change = CONTINUE;
	switch (command_num){
	case 0://cd
		//printf("cd code will be executed \n");
		status_change = change_directory(parsed);
		break;
	case 1://pwd
		//printf("pwd code will be executed \n");	
		print_working_directory();
		break;
	case 2://export
		//printf("export code will be executed \n");
		export_path(parsed);
		break;
	case 3: //history
		//printf("history code will be executed \n");
		print_history(fstream);
		break;
	case 4: //exit
		//printf("exit code will be executed \n");
		status_change = EXIT;
		break;
	default: //if it is not a built in command
		//printf("it is an external command \n");
		if(piped ==FALSE){
			if(fork()==0){
			search_sysPath(parsed);
			}
			 else{
				wait(NULL);
			}
		}
		else{search_sysPath(parsed);}
		break;


	}

return(status_change);
}
//takes in the parsed funtion, checks if it is builtin and reads status change is user enters exit
int execute(char** parsed, FILE* fstream){
	if (size(parsed )==0 ){
		return(1);
	}	
	int command_num = builtin_check(parsed);
	int status = getCommand(parsed, command_num, fstream, FALSE);	
	return(status);
}

int execute_pipes(char ** piped_cmds, int pipe_count, FILE* fstream){
	//create the file descriptors for the pipes
	int pipe_i;
	int cmd;
	int fd;
	int flag = FALSE;
	char ** parsed_command;
	char ** parsed_command2;
	char* pipedcmds;
	char* piped_cmd_store =  malloc(sizeof(char)*SIZE_ARGS);
	char* redirection;
	int redirecfd_in;
	int redirecfd_out;
	int redirection_in;
	int redirection_out;
	int i;
	int j;
	int count;
	char ** parsed_redirection = NULL;
	int redirection_count;
	char *redirin_filename;
	char *redirout_filename;
	//allocate space for the file descriptors
	int (*pipes)[2] = calloc(pipe_count*(sizeof(int[2])), 1);
	if (pipes== NULL){fprintf(stderr, "ERROR: %s\n", strerror(errno));}
	//initialize all teh pipes
	for(pipe_i = BEGIN_COUNT;pipe_i< pipe_count; pipe_i++){
		pipe(pipes[pipe_i]);
	}

	//check is any of teh commands have '>' or '<'
	//the loop that forks to create a child process for each command
	for( int cmd = 0; cmd<=pipe_count; cmd++){
		//parse the command and get the command in a NULL terminated array
		parsed_command2 = parse(piped_cmds[cmd], NULL);
		//check if the command has any redirections
		i =0;
		while (parsed_command2[i] != NULL){
				//printf("%s\n",parsed_command2[i]);
				if(strstr(parsed_command2[i], ">") || strstr(parsed_command2[i], "<")){
					//printf("yess%s\n",parsed_command2[i]);
					break;
				}
				i++;
		}
		//is there are redirections
		if (parsed_command2[i] != NULL){ //if i reaches the end of th array without meeing an '<' or '>'
				parsed_command = malloc(sizeof(char*)*i); //allocate memory for parsed_command. 
				parsed_redirection = malloc(sizeof(char*)*10); //allocate memory for redirection
				//copy everything before the first '<' or '>' into parsed_command
				for (count = 0; count< i; count++){
					parsed_command[count] = malloc(strlen(parsed_command2[count]));
					strcpy(parsed_command[count], parsed_command2[count]);
				}
				//add a NULL terminator
				parsed_command[i] = NULL;
				//store the redirection part of the array in an another array called parsed_redirection
				j =  i;
				while(parsed_command2[j] != NULL){
					parsed_redirection[j-i] = malloc(strlen(parsed_command2[j]));
					strcpy(parsed_redirection[j-i], parsed_command2[j]);
					j++;
				}
				parsed_redirection[j] = NULL;
				redirection_count = j-i; //get the size of the parsed_redirection
				
		}
		//if there are no redirections
		else{
			parsed_command = parsed_command2;
			parsed_redirection = NULL;
		}
		


		//child process for every command
		int pid;
		if ((pid=fork())==0){
			setvbuf (stdout, NULL, _IONBF, BUFSIZ);
			//printf("Child[%d]: ", cmd);
			if(cmd==0){
				printf("Start Process - %s\n",parsed_command[0]);
				
				//makes the STDOUT file descriptor point to the write end of the pipe.  
				if (pipe_count != 0){
					dup2(pipes[cmd][WRITE], STDOUT_FILENO);
					//close all the pipes in this child process
					for(pipe_i = 0; pipe_i< pipe_count; pipe_i++){
						for(fd =0; fd< FD_PIPE; fd++){
							close(pipes[pipe_i][fd]);
						}
					}
				}
				if (parsed_redirection ==NULL){
					printf("End Process: no redirection\n");
					continue;
				}
				else{
						for(redirection_out = redirection_count-1; redirection_out >= 0 ;redirection_out--){
							// fflush(stdout);
							//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
							//fflush(stdout);
							//printf("%s", parsed_redirection[redirection_out						//fflush(stdout);
							if (strstr(parsed_redirection[redirection_out], ">") != NULL) {
								//printf("%p\n", strstr(parsed_redirection[redirection_out], ">"));
								//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
								redirout_filename = parsed_redirection[++redirection_out];
								//printf("%s",redirout_filename);
								redirecfd_out = open(redirout_filename, O_CREAT|O_WRONLY|O_TRUNC, 0777);
								if(!dup2(redirecfd_out,STDOUT_FILENO)){
									printf("ERROR: %s\n",strerror(errno));
								}
								break;
							}

						}
						

						for(redirection_in = redirection_count-1; redirection_in >= 0 ;redirection_in--){
							// fflush(stdout);
							//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
							//fflush(stdout);
							//printf("%s", parsed_redirection[redirection_out						//fflush(stdout);
							if (strstr(parsed_redirection[redirection_in], "<") != NULL) {
								//printf("%p\n", strstr(parsed_redirection[redirection_out], ">"));
								redirin_filename = parsed_redirection[++redirection_in];
								//printf("%s",redirout_filename);
								redirecfd_in = open(redirin_filename, O_RDONLY, 0777);
								if(!dup2(redirecfd_in,STDIN_FILENO)){
									printf("ERROR: %s\n",strerror(errno));
								}
								break;
							}

						}
					flag = TRUE;

				}
				//checkif it is builtin
				int command_num = builtin_check(parsed_command);
				int status = getCommand(parsed_command, command_num, fstream, TRUE);
				close(redirecfd_in);
				close(redirecfd_out);
				if (status == EXIT){break;}		
				
			}
			else if(cmd== pipe_count){
				printf("End Process - %s\n", parsed_command[0]);
				//if (pipe_count != 0){	
				// //makes the STDIN file descriptor point to the read end of the pipe. 
					dup2(pipes[cmd-1][READ], STDIN_FILENO);
					//close all the pipes in this child process
					for(pipe_i = BEGIN_COUNT; pipe_i< pipe_count; pipe_i++){
						for(fd =BEGIN_COUNT; fd< FD_PIPE; fd++){
							close(pipes[pipe_i][fd]);
						}
					}
				//}
				
				if (parsed_redirection ==NULL){
					printf("End Process: no redirection\n");
					continue;
				}
				else{
						for(redirection_out = redirection_count-1; redirection_out >= 0 ;redirection_out--){
							// fflush(stdout);
							//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
							//fflush(stdout);
							//printf("%s", parsed_redirection[redirection_out						//fflush(stdout);
							if (strstr(parsed_redirection[redirection_out], ">") != NULL) {
								//printf("%p\n", strstr(parsed_redirection[redirection_out], ">"));
								//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
								redirout_filename = parsed_redirection[++redirection_out];
								//printf("%s",redirout_filename);
								redirecfd_out = open(redirout_filename, O_CREAT|O_WRONLY|O_TRUNC, 0777);
								if(!dup2(redirecfd_out,STDOUT_FILENO)){
									printf("ERROR: %s\n",strerror(errno));
								}
								break;
							}

						}
						

						for(redirection_in = redirection_count-1; redirection_in >= 0 ;redirection_in--){
							// fflush(stdout);
							//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
							//fflush(stdout);
							//printf("%s", parsed_redirection[redirection_out						//fflush(stdout);
							if (strstr(parsed_redirection[redirection_in], "<") != NULL) {
								//printf("%p\n", strstr(parsed_redirection[redirection_out], ">"));
								redirin_filename = parsed_redirection[++redirection_in];
								//printf("%s",redirout_filename);
								redirecfd_in = open(redirin_filename, O_RDONLY,777);
								if(!dup2(redirecfd_in,STDIN_FILENO)){
									printf("ERROR: %s\n",strerror(errno));
								}
								break;
							}

						}
					flag = TRUE;

				}
				
				int command_num = builtin_check(parsed_command);
				int status = getCommand(parsed_command, command_num, fstream, TRUE);
				close(redirecfd_in);
				close(redirecfd_out);
				if (status == EXIT){break;}			
			}
			else{
				printf("Middle Process - %s\n", parsed_command[0]);
				
				dup2(pipes[cmd-1][READ], STDIN_FILENO);
				dup2(pipes[cmd][WRITE], STDOUT_FILENO);
				//close all the pipes in this child process
				for(pipe_i = BEGIN_COUNT; pipe_i< pipe_count; pipe_i++){
					for(fd =BEGIN_COUNT; fd< FD_PIPE; fd++){
						close(pipes[pipe_i][fd]);
					}
				}
				

				if (parsed_redirection ==NULL){
					printf("End Process: no redirection\n");
					continue;
				}
				else{
						for(redirection_out = redirection_count-1; redirection_out >= 0 ;redirection_out--){
							// fflush(stdout);
							//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
							//fflush(stdout);
							//printf("%s", parsed_redirection[redirection_out						//fflush(stdout);
							if (strstr(parsed_redirection[redirection_out], ">") != NULL) {
								//printf("%p\n", strstr(parsed_redirection[redirection_out], ">"));
								//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
								redirout_filename = parsed_redirection[++redirection_out];
								//printf("%s",redirout_filename);
								redirecfd_out = open(redirout_filename, O_CREAT|O_WRONLY|O_TRUNC, 0777);
								if(!dup2(redirecfd_out,STDOUT_FILENO)){
									printf("ERROR: %s\n",strerror(errno));
								}
								break;
							}

						}
						

						for(redirection_in = redirection_count-1; redirection_in >= 0 ;redirection_in--){
							// fflush(stdout);
							//printf("parsed_redirection[%d] = %s\n", redirection_out, parsed_redirection[redirection_out]);
							//fflush(stdout);
							//printf("%s", parsed_redirection[redirection_out						//fflush(stdout);
							if (strstr(parsed_redirection[redirection_in], "<") != NULL) {
								//printf("%p\n", strstr(parsed_redirection[redirection_out], ">"));
								redirin_filename = parsed_redirection[++redirection_in];
								//printf("%s",redirout_filename);
								redirecfd_in = open(redirin_filename, O_RDONLY, 0777);
								if(!dup2(redirecfd_in,STDIN_FILENO)){
									printf("ERROR: %s\n",strerror(errno));
								}
								break;
							}

						}
					flag = TRUE;

				}


				int command_num = builtin_check(parsed_command);
				int status = getCommand(parsed_command, command_num, fstream, TRUE);
				close(redirecfd_in);
				close(redirecfd_out);
				if (status == EXIT){break;}	
				
			}
			
			exit(0);
		}
		if (flag == TRUE){
			break;
		}
	}
	//closes all the pipes
	for(pipe_i = 0; pipe_i< pipe_count; pipe_i++){
		for(fd =BEGIN_COUNT; fd< 2; fd++){
			close(pipes[pipe_i][fd]);
		}
	}
	//waits for all the processes to end
	for( int cmd = BEGIN_COUNT; cmd<=pipe_count; cmd++){
		wait(NULL);
	}
	//free(pipes);
	return(SUCCESS);
}

int storeInHistory(char* input, FILE* fptr){
	//printf("here");
	if(fptr !=NULL){
		if (!feof(fptr)){
			fseek(fptr, 0, SEEK_END);
		}
		fprintf(fptr,"%s",input);
		return(SUCCESS);
	}
	else{
		fprintf(fptr,"ERROR: %s",strerror(errno));
		return(FAILURE);
	}

}