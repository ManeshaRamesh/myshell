#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <errno.h>
#include <unistd.h>

int main(){
	system("clear");
	printf("Hello this is Manesha's shell!! Have fun using it! \n");
	//loop that runs the entire program again and again until user enters exit
    char cwd[DIR_PATH];//space allocation for 5o00 characters
    // getcwd(cwd, sizeof(cwd));
	char *value;
	sysPath_createlist("PATH", "");
    sysPath* Node = path_variable_create("PWD", "");//create a node
    sysPath_addtolist(Node);
    char** piped_cmds;
    char* input; //to store user unparsed input
    char** parsed_input;// to store user input as an array of strings after it has been split
    FILE* fstream;
    int status=CONTINUE;//controls the loop
    char* command;
    int parsed_input_flag = 0;
    if((fstream = fopen("history.txt", "a+") )== NULL){
        fprintf(stderr, "ERROR: %s", strerror(errno));
    }
       

    
        do{
            getcwd(cwd, sizeof(cwd));
            value = cwd;
            parsed_input_flag = 0;
            update_variable("PWD", value);
            int iter =BEGIN_COUNT;
            int pipe_count = BEGIN_COUNT;
            int redin_count = BEGIN_COUNT;
            int redout_count = BEGIN_COUNT;
        	printf(">> ");//directs the user to enter command
        	input = read_input(fstream);//returns  input of the user a string
            //if it is just a newline character, no command has been entered. So, it will just skip to the next loop
        	if(!strcmp(input, "\n")){
                    continue;
            }
            
            //counts the number of '|', '<' and '>' characters
            while (input[iter] != '\n'){
                if (input[iter] == '|') pipe_count++;
                if (input[iter] == '>') redin_count++;
                if (input[iter] == '<') redout_count++;
                iter++;
            }
            
            //if it is a simple command
            if (pipe_count == 0){ 
                //printf("simple command");
                //is there is redirection
                if (redin_count || redout_count){
                    piped_cmds = malloc(sizeof(char*)*(pipe_count+1));
                    storeInHistory(input, fstream);
                    piped_cmds[0] = input;
                    status = execute_pipes(piped_cmds, 0, fstream);
                    //printf("Simple command with redirection: %d", status);
                }
                //if there is no redirection
                else{
                    parsed_input_flag = 1;
                    parsed_input = parse(input, fstream);// takes teh user input splits it and returns it as an array of strings
                    status = execute(parsed_input, fstream);
                    //printf("Simple command with no redirection: %d", status);
                }
                

            }
            else{//piped command
                //printf("piped command");
                
                //allocates space for the piped cmds
                piped_cmds = malloc(sizeof(char*)*(pipe_count+1));
                storeInHistory(input, fstream);
                //separates the commands delimited by pipes
                int pipe_iter = 0;
                command = strtok(input, "|");
                while(command != NULL){      
                    piped_cmds[pipe_iter] = command;    
                    //printf(" %d . %s", pipe_iter , piped_cmds[pipe_iter]);  
                    pipe_iter++;
                    command = strtok(NULL, "|");

                }
                //executes the pipes
                status = execute_pipes(piped_cmds, pipe_count, fstream);
                //printf("Piped Command: %d", status);
                
            }
                    
		      

		       //int iter;
        	//for (iter = 0; iter<size(parsed_input);iter++ ) {
        		//printf("%s\n",parsed_input[iter]);
        	//}
        	
        	//freeing allocated memory

            //free(piped_cmds);
            if (parsed_input_flag ==1){
                free(parsed_input);
            }
            if (parsed_input_flag ==0){
                free(piped_cmds);
            }

	}while(status);//exits loop when status is either zero or one
        fclose(fstream);
        syspath_linkedlist_destructor();
        system("clear");
        //display_list();
}
