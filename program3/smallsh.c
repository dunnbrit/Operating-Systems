//Files and Libraries
#include "smallsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


int main(){
/**************Signals*****************************************************/
    //Use a flag for entering and exiting foreground only mode
    int fgOnlyMode = 0;
    
    /********Hand;e SIGINT****************************************/
    //Initialize sigaction struct
    struct sigaction ignore_action = {0};
    //On SIGINT ignore signal
    ignore_action.sa_handler = SIG_IGN;
    //Define the sigaction function - which signal to be handled by which sigaction struct
    sigaction(SIGINT, &ignore_action, NULL);
    
    /********Handle SIGTSTP***************************************/
    //SIGSTOP function
    void catchSIGTSTP(int signo){
	//If currently not in foreground only mode
	if(fgOnlyMode == 0){
	    //Set to foreground only mode
	    fgOnlyMode = 1;
	    //Print message
	    char* messageFG = "Entering foreground-only mode (& is now ignored)";
	    write(STDOUT_FILENO, messageFG, 49);
	}
	//If currently in foreground only mode
	else{
	    //Set to normal mode
	    fgOnlyMode = 0;
	    //Print message
	    char* messageBG = "Exiting foreground-only mode";
	    write(STDOUT_FILENO, messageBG, 28);
	}
    }
    //Initialize sigaction struct
    struct sigaction SIGTSTP_action = {0};
    sigemptyset(&SIGTSTP_action.sa_mask); 
    //On SIGTSTP go to catchSIGTSTP function
    SIGTSTP_action.sa_handler = catchSIGTSTP;
    //Block other signals until function executes
    sigfillset(&SIGTSTP_action.sa_mask);
    //Set flag to resume what was happening before
    SIGTSTP_action.sa_flags = SA_RESTART;
    //Define the sigaction function - which signal to be handled by which sigaction struct
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
      
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
char exitStatus [50];
//Input and Output files
int inputfd;
int outputfd;
//Used to hold the background processes
pid_t background[25];
//Background process counter
int processCount = 0;
//Loop counter
int i;


//Before any foreground or background command is run set the exit status to 0
sprintf(exitStatus,"exit status %i", 0);
fflush(stdout);


while(1){
/*************Prompt and Getting Input*************************************/
    
    //If there are any background processes running
    if(processCount > 0){
	pid_t childPID;
	int childExit;
	
	//Then check if they are complete
	for(i = 0; i < processCount; i++){
	    childPID = waitpid(background[i], &childExit, WNOHANG);
	    //If the process is complete
	    if(childPID != 0){
		//Set the exit status
		if(WIFEXITED(childExit)){
		    //Normal exit, set status
		    sprintf(exitStatus,"exit status %i",WEXITSTATUS(childExit));
		    fflush(stdout);
		}
		if(WIFSIGNALED(childExit)){
		    //Exit by signal, set status
		    sprintf(exitStatus,"terminated by signal %i", WTERMSIG(childExit));
		    fflush(stdout);
		}
		
		//Print process id and exit status
		printf("background pid %i is done : %s\n",background[i],exitStatus);
		fflush(stdout);
		
		//Remove process id from background array
		int j;
		int k = i;
		for(j = processCount; k < j; k++){
		    background[k] = background[k+1];
		}
		//Decrease process count
		processCount--;
	    }
	}
    }
    
    
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
	    //Kill all processes
	    int m;
	    for(m = 0; m < processCount; m++){
		kill(background[m],getppid());
	    }
	    exit(0);  
	}
    }
    
    /*******Built in cd***********************************/
    else if(strcmp("cd",arguments[0]) == 0){
	cdCmd(arguments,&totalArg);
    }
    
    /*******Built in status*******************************/
    else if(strcmp("status",arguments[0]) == 0){
	statusCmd(arguments, exitStatus, &totalArg);
    }
    
    /******Non built in Commands*************************/
    else{	
	//If not to be run in the background (run in foreground)
	//Last argument is not & or in foreground only mode
	if((strcmp(arguments[totalArg-1],"&") != 0) || fgOnlyMode == 1){
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
		    sprintf(exitStatus,"exit status %i", 1);
		    fflush(stdout);
		    break;

		//Child process
		case 0:
		    //Child should ignore SIGTSTP so change handler
		    SIGTSTP_action.sa_handler = SIG_IGN;
		    //Child should terminate if SIGINT
		    ignore_action.sa_handler = SIG_DFL;
		    
		    //Check if redirection needs to be done
		    if(inputIndex > 0 || outputIndex > 0){
			//Call redirection function
			redirectFile(&inputfd,&outputfd,&inputIndex,&outputIndex,arguments,exitStatus);
		    }
		    //If not
		    else{
			//Set final argument to NULL to indicate end
			//Used later for exec
			arguments[totalArg] = NULL;
		    }
		    
		    //If in foreground only mode and was supposed to be a background command
		    if(fgOnlyMode == 1 && (strcmp(arguments[totalArg-1],"&") == 0)){
			//Set & to Null
			arguments[totalArg -1] = NULL;
		    }

		    //Exec command (this replaces the fork)
		    if(execvp(arguments[0],arguments) < 0){
			//If exec returned < 0 there was an error
			perror("ERROR: Exec failure");
			//Set exit status to 1
			exit(1);
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

	    //Set the exit status
	    if(WIFEXITED(childExitMethod)){
		//Normal exit, set status
		sprintf(exitStatus,"exit status %i",WEXITSTATUS(childExitMethod));
		fflush(stdout);
	    }
	    if(WIFSIGNALED(childExitMethod)){
		//Exit by signal, set status
		sprintf(exitStatus,"terminated by signal %i", WTERMSIG(childExitMethod));
		fflush(stdout);
	    } 
	}
	//Else run as background process
	else{
	    //Variable to hold the processID
	    pid_t backspawnpid = -5;
	    //Hold info on child termination
	    int backChildExitMethod = -5;

	    //Fork a child to run the command
	    backspawnpid = fork();
	    switch(backspawnpid){
		//Error
		case -1:
		    perror("ERROR: backspawnpid");
		    fflush(stdout);
		    //Exit with error set to 1
		    sprintf(exitStatus,"exit status %i", 1);
		    fflush(stdout);
		    break;	
		
		//Child process
		case 0:
		    //Child should ignore SIGTSTP so change handler
		    SIGTSTP_action.sa_handler = SIG_IGN;
		    
		    //Check if redirection needs to be done
		    if(inputIndex > 0 || outputIndex > 0){
			//Call redirection function
			redirectFile(&inputfd,&outputfd,&inputIndex,&outputIndex,arguments,exitStatus);
		    }
		    
		    //Set final argument to NULL to indicate end
		    //Used later for exec
		    arguments[totalArg-1] = NULL;
		    
		    //Do background redirection
		    backRedirection(&inputfd, &outputfd);
		    
		    //Exec command (this replaces the fork)
		    if(execvp(arguments[0],arguments) < 0){
			//If exec returned < 0 there was an error
			perror("ERROR: Exec failure");
			//Set exit status to 1
			exit(1);
			//Terminate process
			kill(getpid(),getppid());
		    }
		    break;

		//Parent process
		default:
		    //Print the process id
		    printf("background pid is %i\n", backspawnpid);
		    //Add process id to background processes
		    background[processCount] = backspawnpid;
		    //Increase background process count
		    ++processCount;
		    break;  
	    }
	    //Do not wait for child
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