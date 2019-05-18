//This is the header file for functions used in the smallsh program
#ifndef SMALLSH_H
#define SMALLSH_H

#include <stdio.h>
#include <string.h>


 int getCommand(char* input, char* command, char** arguments){
     //Clear the previous arguments
     int i;
     for(i=0; i < 512; i++){
	 memset(arguments[i],0,sizeof(arguments[i]));
     }
     //Clear the previous command
     memset(command,0,sizeof(command));
     
     //Define the delimiter
     const char delim[2] = " ";
     //Token to hold the command then each argument
     char* token;
     //Counter for arguments
     int totalArg = -1;
     
     //Get the first token which is the command
     token = strtok(input,delim);
     //Copy the token into command
     strncpy(command,token,strlen(token)-1);
     
     //Get the rest of the tokens which are the arguments
     while(token != NULL){
	 //If total arguments is -1 then it is the command
	 if(totalArg == -1){
	    //Copy the token into command
	    strncpy(command,token,strlen(token)-1);   
	 }
	 //If not then its an argument
	 else{
	     //Copy the token into arguments
	     strncpy(arguments[totalArg],token,strlen(token)-1);
	 }
	 
	 //Increment totalArg
	 totalArg++;
	 
	 //Get next token
	 token = strtok(NULL,delim);
     }
     
     return totalArg;
 }



#endif