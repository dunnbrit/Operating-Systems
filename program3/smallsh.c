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
for(y=0;y<2;y++){
/*************Prompt and Getting Input*************************************/

    //Prompt for the command
    printf(": ");
    fflush(stdout);
    //Get input
    getline(&input,&bufferSize,stdin);

    //Get arguments using function
    totalArg = getArgs(input, arguments);

/*************Handling Different Commands**********************************/

    int i;
    for(i=0;i<totalArg;i++){
	printf("%s",arguments[i]);
    }
    /******Blank Line**************************************/
    if(strcmp(arguments[0], "")== 0){
	//Continue to prompt again
	break;
    }
    /******Comment****************************************/
    else if(arguments[0][0] == '#'){
	//Ignore line and continue to prompt again
	break;
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