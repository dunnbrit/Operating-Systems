//This is the header file for functions used in the smallsh program
#ifndef SMALLSH_H
#define SMALLSH_H
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*Function parses the input and divides it into the command and arguments
 * Returns the number of arguments
 */
 int getArgs(char* input, char** arguments){     
     //Define the delimiter
     const char delim[2] = " ";
     //Token to hold the command then each argument
     char* token;
     //Counter for arguments
     int totalArg = 0;
     
     //Get the first token which is the command
     token = strtok(input,delim);
     
     //Get the rest of the tokens which are the arguments
     while(token != NULL){
	 //Copy the token into arguments
	 strncpy(arguments[totalArg],token,strlen(token));
	 
	 //Increment totalArg
	 totalArg++;
	 
	 //Get next token
	 token = strtok(NULL,delim);
     }
     
     return totalArg;
 }

 
 

 


#endif