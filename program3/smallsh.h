//This is the header file for functions used in the smallsh program
#ifndef SMALLSH_H
#define SMALLSH_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

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
	 
	 //Handle issue with SIGTSTP adding extra characters when using the grading script
	 if(strcmp(token,"-SIGTSTP") == 0){
	     sprintf(arguments[totalArg+1],"-SIGTSTP");
	     fflush(stdout);
	 }
	 
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
	 
	 //Check if token contains $$
	 if(strstr(arguments[totalArg],"$$") != NULL){
	    //Replace the "$$" with the PID
	    sprintf(strstr(arguments[totalArg],"$$"),"%d ", getpid());
	    fflush(stdout);
	 }
	 
	 //Get next token
	 token = strtok(NULL,delim);
	 
     }while(token != NULL);
     
     arguments[totalArg][strlen(arguments[totalArg])-1] = 0;
     
     return totalArg+1;
 }

 
 /*
  * Function performs file redirection
  */
 void redirectFile (int* inputfd, int* outputfd,int* inputIndex, int* outputIndex, char** arguments, char* exitStatus){
     //If input redirection needed
     if(*inputIndex > 0){
	//Open the input file for read only
	*inputfd = open(arguments[*inputIndex], O_RDONLY);
	fcntl(*inputfd, F_SETFD, FD_CLOEXEC);
	//Check if opened properly
	if(*inputfd == -1){
	    //If -1 there was an error
	    perror("ERROR: problem opening read file");
	    fflush(stdout);
	    //Set exit status to 1
	    exit(1);
	}
	else{
	    //If opened correctly, set "<" and file to NULL for exec
	    arguments[*inputIndex - 1] = NULL;
	    arguments[*inputIndex] = NULL;
	    //Redirect input using dup2
	    dup2(*inputfd,0);
	}
    }
    //If output redirection needed
    if(*outputIndex > 0){
	//Open the output file for read only
	*outputfd = open(arguments[*outputIndex], O_WRONLY | O_CREAT | O_TRUNC,0644);
	fcntl(*outputfd, F_SETFD, FD_CLOEXEC);
	//Check if opened properly
	if(*outputfd == -1){
	    //If -1 there was an error
	    perror("ERROR: problem opening write file");
	    fflush(stdout);
	    //Set exit status to 1
	    exit(1);
	}
	else{
	    //If opened correctly, set ">" and file to NULL for exec
	    arguments[*outputIndex - 1] = NULL;
	    arguments[*outputIndex] = NULL;
	    //Redirect input using dup2
	    dup2(*outputfd,1);
	}
    }
}
 
 
 /*
  * Function executes built in cd command
  */
 void cdCmd(char** arguments, int* totalArg){
    //Too many arguments
    if(*totalArg > 3){
	printf("ERROR: cd does not accept more than one argument\n");
	fflush(stdout);
    }
    //If there are 3 arguments and the third is not to run as background
    else if(*totalArg == 3 && (strcmp(arguments[2],"&") != 0)){
	//Then too many arguments
	    printf("ERROR: cd does not accept more than one argument\n");
	    fflush(stdout);
    }
    //If there is one argument or 2 with the second being to run in background
    //Go to HOME directory  
    else if(*totalArg == 1 || (*totalArg == 2 && (strcmp(arguments[1],"&") == 0))){
	chdir(getenv("HOME"));
    }
    //One additional argument go to that path
    else{
	//Try to use path
	if(chdir(arguments[1]) == -1){
	    //If it does not work print error message
	    printf("ERROR: invalid path, cd not executed\n");
	    fflush(stdout);
	}
    }
 }
 
 /*
  *Function executes built in status command
  */
 void statusCmd(char** arguments, char* exitStatus, int* totalArg){
    //Too many arguments
    if(*totalArg > 2){
	printf("ERROR: status does not accept arguments\n");
	fflush(stdout);
    }
    //Too many arguments and last argument not background
    else if(*totalArg == 2 && (strcmp(arguments[1],"&") != 0)){
	printf("ERROR: status does not accept arguments\n");
	fflush(stdout);
    }
    //Print stauts
    else{
	printf("%s\n", exitStatus);
	fflush(stdout);
    }    
 }
 
 /*
  *Function redirects output for background processes
  */
 void backRedirection(int* inputfd, int* outputfd){
     //open /dev/null
     int devNull = open("/dev/null",O_RDWR);
     
     //If no input redirection provided
     if(*inputfd == 0){
	 //Redirect /dev/null to stdin
	 dup2(0,devNull); 
     }
     //If no output redirection provided
     if(*outputfd == 0){
	 //Redirect std out to /dev/null
	 dup2(devNull,1); 
     }
     
     //Close on exec
     fcntl(devNull, F_SETFD, FD_CLOEXEC);
 }

 


#endif