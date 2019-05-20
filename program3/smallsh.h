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
 int getArgs(char* input, char** arguments, int* inputIndex, int* outputIndex){     
     //Define the delimiter
     const char delim[2] = " ";
     //Token to hold the command then each argument
     char* token;
     //Counter for arguments
     int totalArg = -1;
     
     //Get the first token which is the command
     token = strtok(input,delim);
     
     //Get the rest of the tokens which are the arguments
     do{
	 //Copy the token into arguments
	 strncpy(arguments[totalArg+1],token,strlen(token));
	 
	 //Increment totalArg
	 totalArg++;
	 
	 //If the token is < then its followed by an input_file
	 if(strcmp("<",token) == 0){
	     //Save the index of the input file argument
	     *inputIndex =  totalArg + 1;
	 }
	 
	 //If the token is > then its followed by an output_file
	 if(strcmp(">",token) == 0){
	     //Save the index of the input file argument
	     *outputIndex = totalArg + 1;
	 }
	 
	 //Get next token
	 token = strtok(NULL,delim);
	 
     }while(token != NULL);
     
     arguments[totalArg][strlen(arguments[totalArg])-1] = 0;
     
     return totalArg+1;
 }

 
 

 


#endif