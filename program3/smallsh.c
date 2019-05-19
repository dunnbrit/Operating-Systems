//Files and Libraries
#include "smallsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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


int y;
for(y=0;y<3;y++){
/*************Prompt and Getting Input*************************************/

    //Prompt for the command
    printf(": ");
    fflush(stdout);
    //Get input
    getline(&input,&bufferSize,stdin);

    //Get arguments using function
    totalArg = getArgs(input, arguments);

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



/*************Freeing memory before next command***************************/
    //Free memory
    free(input);
    //Clear memory
    for(z=0; z < 512; z++){
	memset(arguments[z],0,sizeof(arguments[z]));
    }
    //Set to null
    input = NULL;
}    
    return 0;
}