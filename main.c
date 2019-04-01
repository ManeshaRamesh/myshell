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
    int input_flag = 0;
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
        	input = read_input();//returns  input of the user a string
            
            
            if(!strcmp(input, "\n")){
                    continue;
            }
            //printf("Send for conversion\n");
            if(strchr(input, '!') !=NULL){
                //convert_flag = 1;
                //=printf("here");
                input_flag = 1;
                input = convert_command(input, fstream);
                //printf("%s\n", input);
            }

            //printf("converted = %s", input);
            if (input == NULL){
                continue;
            }
            // printf("%s", input);
            storeInHistory(input, fstream);
            //fprintf(fstream,"end\n");
           // printf("before checking for pipes\n");
            //if it is just a newline character, no command has been entered. So, it will just skip to the next loop
        	//fprintf(fstream,"evaluate\n");
            //printf("here\n");
            //counts the number of '|', '<' and '>' characters
            while (input[iter] != '\n'){
                if (input[iter] == '|') pipe_count++;
                if (input[iter] == '>') redin_count++;
                if (input[iter] == '<') redout_count++;
                iter++;
            }
            
            //if it is a simple command
            //printf("execution\n");
            if (pipe_count == 0){ 
                //printf("simple command");
                //is there is redirection
                if (redin_count || redout_count){
                    //storeInHistory(input, fstream);
                    piped_cmds = malloc(sizeof(char*)*(pipe_count+1));
                    
                    piped_cmds[0] = input;
                    status = execute_pipes(piped_cmds, 0, fstream);
                    //printf("Simple command with redirection: %d", status);
                }
                //if there is no redirection
                else{
                    parsed_input_flag = 1;
                    //==printf( "writing\n");
                    //storeInHistory(input, fstream);
                    
                    parsed_input = parse(input, fstream);// takes teh user input splits it and returns it as an array of strings
                    //printf( "done\n");
                    status = execute(parsed_input, fstream);
                    //printf( "done\n");
                    //printf("Simple command with no redirection: %d", status);
                }
                

            }
            else{//piped command
                //printf("piped command");
                
                //allocates space for the piped cmds
                piped_cmds = malloc(sizeof(char*)*(pipe_count+1));
                //storeInHistory(input, fstream);
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
            //fprintf(fstream,"evaluate end\n");    
		      //printf("finishing up \n");

		       //int iter;
        	//for (iter = 0; iter<size(parsed_input);iter++ ) {
        		//printf("%s\n",parsed_input[iter]);
        	//}
        	
        	//freeing allocated memory

            //free(piped_cmds);
            // if (input_flag ==1){
            //     free(input);
            // }
            if (parsed_input_flag ==1){
                //printf("free1 \n");
                free(parsed_input);
                //printf("freed1 \n");
            }
            if (parsed_input_flag ==0){
                //printf("free2 \n");
                free(piped_cmds);
            }
            // if (input_flag == 1){
            //     free(input);
            // }
           // printf("freeed all \n");
            //fprintf(fstream,"loop\n");
	}while(status);//exits loop when status is either zero or one
        //printf("here \n");
        fclose(fstream);
        //printf("here \n");
        syspath_linkedlist_destructor();
        //printf("return \n");
        //display_list();
}
