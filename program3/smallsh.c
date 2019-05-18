//Files and Libraries
#include "smallsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





int main(){
    
//Used to hold the full command and arguments inputed by user
char* input;
//Max size of user input
size_t bufferSize = 0;
//Used to hold the command
char* command;
//Used to hold the max number of arguments
char* arguments[512];
int i;
for(i=0; i < 512; i++){
    arguments[i]=calloc(100,sizeof(char));
}
//Holds the total number of arguments
int totalArg;



//Prompt for the command
printf(": ");
fflush(stdout);

getline(&input,&bufferSize,stdin);
fflush(stdin);



//Call function to determine command
totalArg = getCommand(input, command, arguments);

//If input has more than 2048 characters then exit
if(strlen(input) > 2048){
    return 1;
}

//Free memory
free(input);
//Set to null
input = NULL;

    
    return 0;
}