//Files and Libraries
#include "smallsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
    //Get arguments using function
    totalArg = getArgs(input, arguments,&inputIndex, &outputIndex);

/*************Handling Different Commands**********************************/

    /******Blank Line**************************************/
    if(strcmp(arguments[0],"") == 0){
	printf("blank\n");
	fflush(stdout);
	//Continue to prompt again
    }
    /******Comment****************************************/
    else if(arguments[0][0] == '#'){
	printf("comment\n");
	fflush(stdout);
	//Ignore line and continue to prompt again
    }
    /******Built in exit**********************************/
    else if(strcmp("exit",arguments[0]) == 0){
	//Too many arguments and last argument is not to run in the background
	if(totalArg > 2){
	    printf("ERROR: exit does not accept arguments\n");
	    fflush(stdout);
	}
	else if(totalArg == 2 && (strcmp(arguments[1],"&") != 0)){
	    printf("ERROR: exit does not accept arguments\n");
	    fflush(stdout);
	}
	//No arguments or ignore argument to run as background process
	else{
	    //Need to add in something to kill all processes first
	    //Need exit status from processes
	    exit(0);  
	}
    }
    /*******Built in cd***********************************/
    else if(strcmp("cd",arguments[0]) == 0){
	
	char s[100];
	
	//Too many arguments
	if(totalArg > 2){
	    printf("ERROR: cd does not accept more than one argument\n");
	    fflush(stdout);
	}
	//No arguments go to HOME directory  
	else if(totalArg == 1){
	    chdir(getenv("HOME"));
	    printf("current dir (HOME): %s\n", getcwd(s,100));
	    fflush(stdout);
	}
	//One additional argument go to that path
	else{
	    if(chdir(arguments[1]) == -1){
		printf("ERROR: invalid path, cd not executed\n");
		fflush(stdout);
	    }
	    else{
		printf("current dir (PATH): %s\n", getcwd(s,100));
		fflush(stdout);
	    }
	}
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
		    perror("Error: spawnpid");
		    //Exit with error set to 1
		    exit(1);
		    break;
		    
		//Child process
		case 0:
		    //Check if redirection needs to be done
		    if(inputIndex > 0 || outputIndex > 0){
			//If input redirection needed
			if(inputIndex > 0){

			}
			//If output redirection needed
			if(outputIndex > 0){

			}
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
			perror("Exec failure");
			exit(1);
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


/*************Freeing memory before next command***************************/
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