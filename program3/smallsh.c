//Files and Libraries
#include "smallsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


int main(){
/*************Setup********************************************************/  
//Used to hold the full command and arguments inputed by user
char* input;
//Max size of user input
size_t bufferSize = 0;
//Used to hold the max number of arguments
char* arguments[512];
int z;
for(z=0; z < 512; z++){
    arguments[z]=calloc(100,sizeof(char));
}
//Holds the total number of arguments
int totalArg;
//Location of file input and output redirection
int inputIndex;
int outputIndex;
//Holds exit status
int exitStatus = 0;
//Input and Output files
int inputfd;
int outputfd;



while(1){
/*************Prompt and Getting Input*************************************/

    //Prompt for the command
    printf(": ");
    fflush(stdout);
    //Get input
    getline(&input,&bufferSize,stdin);

    //Set Input and Output index to 0 to mean none
    inputIndex = 0;
    outputIndex = 0;
    //Clear input and output by setting to -5
    inputfd = -5;
    outputfd = -5;
    //Get arguments using function
    totalArg = getArgs(input, arguments,&inputIndex, &outputIndex);

/*************Handling Different Commands**********************************/

    /******Blank Line**************************************/
    if(strcmp(arguments[0],"") == 0){
	//Continue to prompt again
    }
    
    /******Comment****************************************/
    else if(arguments[0][0] == '#'){
	//Ignore line and continue to prompt again
    }
    
    /******Built in exit**********************************/
    else if(strcmp("exit",arguments[0]) == 0){
	exitCmd(arguments,&totalArg);
    }
    
    /*******Built in cd***********************************/
    else if(strcmp("cd",arguments[0]) == 0){
	cdCmd(arguments,&totalArg);
    }
    
    /*******Built in status*******************************/
    
    /******Non built in Commands*************************/
    else{	
	//If not to be run in the background
	if((strcmp(arguments[totalArg-1],"&") != 0)){
	    //Variable to hold the processID
	    pid_t spawnpid = -5;
	    //Hold info on child termination
	    int childExitMethod = -5;
	    
	    //Fork a child to run the command
	    spawnpid = fork();
	    switch(spawnpid){
		//Error
		case -1:
		    perror("ERROR: spawnpid");
		    fflush(stdout);
		    //Exit with error set to 1
		    exit(1);
		    break;
		    
		//Child process
		case 0:
		    //Check if redirection needs to be done
		    if(inputIndex > 0 || outputIndex > 0){
			//Call redirection function
			redirectFile(&inputfd,&outputfd,&inputIndex,&outputIndex,arguments);
		    }
		    //If not
		    else{
			//Set final argument to NULL to indicate end
			//Used later for exec
			arguments[totalArg] = NULL;
		    }
		    
		    //Exec command (this replaces the fork)
		    if(execvp(arguments[0],arguments) < 0){
			//If exec returned < 0 there was an error
			perror("ERROR: Exec failure");
			//Set exit status to 1
			exitStatus = 1;
			//Terminate process
			kill(getpid(),getppid());
		    }
		    break;
		    
		//Parent process
		default:
		    //Do nothing just wait on child
		    break;
	    }
	    //Have parent wait on child to terminate before continuing
	    waitpid(spawnpid, &childExitMethod,0);
	}
    }


/*************Freeing memory/clean up before next command***************************/
    //Free memory
    free(input);
    //Clear memory
    for(z=0; z < 512; z++){
	memset(arguments[z],0,sizeof(arguments[z]));
    }
    //Set to null
    input = NULL;
    
}//End of while loop
    return 0;

}