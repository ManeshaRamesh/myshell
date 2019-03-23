#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <errno.h>

int main(){
	
	printf("Hello this is Manesha's shell!! Have fun using it! \n");
	//loop that runs the entire program again and again until user enters exit
	
	sysPath_createlist("PATH", "");
        char* input; //to store user unparsed input
	   char** parsed_input;// to store user input as an array of strings after it has been split
        FILE* fstream;
        if((fstream = fopen("history.txt", "a+") )== NULL){
            fprintf(stderr, "ERROR: %s", strerror(errno));
        }
       

        int status=CONTINUE;//controls the loop
        char* command;
        do{
        	printf(">> ");//directs the user to enter command
        	input = read_input(fstream);//returns  input of the user a string
        	if(!strcmp(input, "")){
                    continue;
                }
            int iter =BEGIN_COUNT;
            int pipe_count = BEGIN_COUNT;
            // int redirec_out = 0;
            // int redirec_in = 0;
            //counts the number of '|' characters
            while (input[iter] != '\n'){
                if (input[iter] == '|') pipe_count++;
                iter++;
            }
            
            //if it is a simple command
            if (pipe_count == 0){ 
                //printf("simple command");
                parsed_input = parse(input, fstream);// takes teh user input splits it and returns it as an array of strings
                status = execute(parsed_input, fstream);
            }
            else{//piped command
                //printf("piped command");

                //allocates space for the piped cmds
                char** piped_cmds = malloc(sizeof(char*)*(pipe_count+1));
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
                
            }
            //parsed_input = parse(command, fstream);// takes teh user inputm splits it and returns it as an array of strings
            //status = execute(parsed_input, fstream);

    
             
		      

		       //int iter;
        	//for (iter = 0; iter<size(parsed_input);iter++ ) {
        		//printf("%s\n",parsed_input[iter]);
        	//}
        	
        	//freeing allocated memory
        	free(input);
        	free(parsed_input);
	}while(status);//exits loop when status is either zero or one
        fclose(fstream);
        syspath_linkedlist_destructor();
        //display_list();
}
