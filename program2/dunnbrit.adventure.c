/**********************************Setup**************************************/

/*Libraries*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/*Global variables to help with threads*/
/*Create a mutex*/
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
/*Holds Threads*/
pthread_t mainThread;
pthread_t secondThread;
/*File pointer to write time to*/
FILE* filePtrTime;

/**********************************Functions**********************************/

/*
 * Returns the newest directory name (using pointers)
 */

void getNewestDirectory(char* newestDirName){
    /*Added a few comments and delete print statements but used code from: 
     *https://oregonstate.instructure.com/courses/1719569/pages/2-dot-4-manipulating-directories
     */
    
    /*newestDirName will store the name found in dirName in main*/
    
    /*Modified timestamp of newest subdir examined */
    int newestDirTime = -1;
    /*Prefix we're looking for */
    char targetDirPrefix[32] = "dunnbrit.rooms."; 
    /*Holds the name of the newest dir that contains prefix */
    /*char newestDirName[256];*/
    /*Clear newestDirName*/
    memset(newestDirName, '\0', sizeof(newestDirName)); 
    /*Holds the directory we're starting in*/
    DIR* dirToCheck; 
    /*Holds the current subdir of the starting dir*/
    struct dirent *fileInDir;
    /*Holds information we've gained about subdir*/
    struct stat dirAttributes;
    
    /*Open up the directory this program was run in*/
    dirToCheck = opendir(".");
    
    /*Make sure the current directory could be opened*/
    if(dirToCheck > 0){
	/*Check each entry in dir*/
	while ((fileInDir = readdir(dirToCheck)) != 0){
	    /*If entry has prefix*/
	    if (strstr(fileInDir->d_name, targetDirPrefix) != 0){
		/*Get attributes of the entry*/
		stat(fileInDir->d_name, &dirAttributes);
		
		/*If this time is bigger*/
		if ((int)dirAttributes.st_mtime > newestDirTime){
		    /*Change the newest directory time to the bigger time*/
		    newestDirTime = (int)dirAttributes.st_mtime;
		    /*Clear the newestDirName*/
		    memset(newestDirName, '\0', sizeof(newestDirName));
		    /*Copy the directory name into newestDirName*/
		    strcpy(newestDirName, fileInDir->d_name);  
		}
	    }
	}
    }
    /*Close the directory we opened*/
    closedir(dirToCheck);
}

/*
 *Returns the start room name(using pointers) 
 */
void getStartRoom(char* startRoom, char* myDir){
    /*Holds the open directory*/
    DIR* dirOpened;
    /*Holds the current file to be read*/
    struct dirent *currentDirFile;
    /*Holds the file pointer of the opened file*/
    FILE* fp;
    /*Holds the file path to open file*/
    char filePath[100];
    /*Holds the line read from file*/
    char readLine[100];
    
    /*Open the directory with the files*/
    dirOpened = opendir(myDir);
    
    /*Make sure the directory opened*/
    if(dirOpened > 0){
	
	/*Go through each file in the directory*/
	while((currentDirFile = readdir(dirOpened)) != 0){ 
	    
	    /*If file name contains "_room"*/
	    if(strstr(currentDirFile->d_name,"_room") != 0){
		/*Clear file path*/
		memset(&filePath,0,sizeof(filePath));
		/*Create file path*/
		sprintf(filePath,"%s/%s",myDir,currentDirFile->d_name);
		/*Open the file for reading*/
		fp = fopen(filePath,"r");
		
		/*Make sure the file opened*/
		if(fp != 0){
		    /*Loop until at last line of file*/
		    while(fgets(readLine,100,fp) != 0){
			/*Don't do anything with the read lines until at last line*/
		    }
		    
		    /*Check if the line contains "START_ROOM*/
		    if(strstr(readLine,"START_ROOM")){
			/*Set the file pointer back to the beginning of the file but after "ROOM NAME: "*/
			if(fseek(fp,11,SEEK_SET)== 0){
			    /*If successful get the room name*/
			    fgets(readLine,100,fp);
			    /*Copy the room name into start room*/
			    strncpy(startRoom,readLine,strlen(readLine)-1);
			    /*Close file*/
			    fclose(fp);
			    /*Exit function*/
			    return;
			}
		    }
		    /*Close file*/
		    fclose(fp);
		    /*Reset pointer to null*/
		    fp = 0;
		}
	    }  
	}
    }
    /*Close the directory*/
    closedir(dirOpened);
}

/*
 *Returns the current room name, the connected room names, number of connections,
 *and room type (using pointers)
 */
void getCurrentRoomInfo(char* roomName, char** connectedRooms, int* numConnected, int* type, char* myDir){
    /*Holds the open directory*/
    DIR* dirOpened;
    /*Holds the current file to be read*/
    struct dirent *currentDirFile;
    /*Holds the file pointer of the opened file*/
    FILE* fp;
    /*Holds the file path to open file*/
    char filePath[100];
    /*Holds the line read from file*/
    char readLine[100];
    
    /*Open the directory with the files*/
    dirOpened = opendir(myDir);
    
    /*Make sure the directory opened*/
    if(dirOpened > 0){

	/*Go through each file in the directory*/
	while((currentDirFile = readdir(dirOpened)) != 0){ 
	   
	    /*If file name contains the current room's name*/
	    if(strstr(currentDirFile->d_name,roomName) != 0){
		/*Clear file path*/
		memset(&filePath,0,sizeof(filePath));
		/*Create file path*/
		sprintf(filePath,"%s/%s",myDir,currentDirFile->d_name);
		/*Open the file for reading*/
		fp = fopen(filePath,"r");
		
		/*Make sure the file opened*/
		if(fp != 0){
		    /*Set a loop counter to get number of lines in file*/
		    int loop = 0;
		    /*Loop until at last line of file*/
		    while(fgets(readLine,100,fp) != 0){
			/*Get the number of lines*/
			loop++;
		    }
		    /*Save the number of connections*/
		    *numConnected = loop - 2;
		    
		    /*Move the file pointer back to the start of the file*/
		    fseek(fp,0,SEEK_SET);
		    /*Read the first line*/
		    fgets(readLine,100,fp);
		    /*Read the next number of connections lines */
		    int x;
		    for(x=0;x < *numConnected; x++){
			/*Move the pointer to room name following "CONNECTION: "*/
			fseek(fp,14,SEEK_CUR);
			/*Read next line*/
			fgets(readLine,100,fp);
			/*Copy the connecting room name into connected rooms*/
			strncpy(connectedRooms[x],readLine,strlen(readLine)-1);	
		    }
		    
		    /*Move the pointer to go past "ROOM TYPE: "*/
		    fseek(fp,11,SEEK_CUR);
		    /*Read last line*/
		    fgets(readLine,100,fp);
		    /*Use this line to get room type*/
		    if(strstr(readLine,"START_ROOM")!= 0){
			/*Save room type as start room*/
			*type = 0;
		    }
		    if(strstr(readLine,"MID_ROOM")!= 0){
			/*Save room type as mid room*/
			*type = 1;
		    }
		    if(strstr(readLine,"END_ROOM")!= 0){
			/*Save room type as end room*/
			*type = 2;
		    }
		    
		    /*Close file*/
		    fclose(fp);
		    /*Reset pointer to null*/
		    fp = 0;
		}
	    }  
	}
    }
    /*Close the directory*/
    closedir(dirOpened);
}

/*
 *Second thread to write time to file
 */
void* writeTime(void* arguement){
    /*Lock the mutex(will block second thread upon creation) but will lock mutex
     *and block the main thread when main thread unlocks mutex
     */
    pthread_mutex_lock(&myMutex);
    
    /*Get time. Code from:https://www.geeksforgeeks.org/strftime-function-in-c/ */
    time_t t ; 
    struct tm *tmp ; 
    char MY_TIME[50]; 
    time( &t ); 
    tmp = localtime( &t );
    strftime(MY_TIME, sizeof(MY_TIME), "%I:%M%p, %A, %B %d, %Y", tmp);
    
    /*Write time to file*/
    fprintf(filePtrTime,"%s\n",MY_TIME);

    /*Unlock mutex to allow main thread to read file*/
    pthread_mutex_unlock(&myMutex);
}

/*
 *Main thread function to create second thread and then block it
 */
void* startSecondThread(void* arguement){
    /*Lock the mutex*/
    pthread_mutex_lock(&myMutex);
    
    /*Holds result for second thread*/
    int resultSecond;
    /*Create the second thread(this will block it because the function tries to lock mutex*/
    resultSecond = pthread_create(&secondThread,0,writeTime,0);
    /*Check creation successful*/
    if(resultSecond != 0){
	printf("Error creating second thread\n");
    }
}


/********************************Main******************************************/

int main(){
   
/*Newest Directory to Use*/
    /*Holds the newest dirName*/
    char dirName[256];
    /*Call function to get the newest directory name*/
    getNewestDirectory(dirName);

/*Create main thread*/
    /*Holds result for thread*/
    int resultMain;

    /*Create main thread*/
    resultMain = pthread_create(&mainThread,0,startSecondThread,0);
   
    /*Check thread created successfully*/
    if(resultMain != 0){
	printf("Error creating main thread\n");
    }
    
/*File setup to write and read path */
    /*File pointer to write path to*/
    FILE* filePtr;
    /*File path for path of rooms*/
    char file_path[100];
    /*Holds the read in line from path.txt*/
    char pathLine[100];
    /*Create file path name*/
    sprintf(file_path,"%s/path.txt",dirName);
    /*Create and open file to write path of rooms to*/
    filePtr = fopen(file_path,"w+");    
   
/*File setup to write and read time*/
    /*File path for time*/
    char file_path_time[50] = "currentTime.txt";
    /*Create and open file to write time to*/
    filePtrTime = fopen(file_path_time,"w+");
    
/*Variables to hold room info*/
    /*Current room name*/
    char currentRoom[16];
    /*Room names connected to current room*/
    char* connections[6];
    /*Allocate memory*/
    int i;
    for(i=0; i < 6; i++){
	connections[i] = calloc(16,sizeof(char));
    }
    /*number of rooms connected to current room*/
    int numConnections;
    /*int to represent current room type 0= start 1=mid 2=end*/
    int roomType;
    
/*Run game*/
    /*Holds user input*/
    char* input = 0;
    /*Holds the size of allocated buffer*/
    size_t bufferSize = 0;
    /*Holds if a correct name was entered*/
    int correct;
    /*Used for a loop counter later*/
    int j;
    int y;
    /*Holds the number of steps taken*/
    int steps = 0;
    /*Holds line read from time file*/
    char timeLine[100];

    /*First get the name of the start room*/
    getStartRoom(currentRoom, dirName); 
    
    /*Loop until end room reached*/
    while(1){
	/*Get the current room's info*/
	getCurrentRoomInfo(currentRoom, connections, &numConnections, &roomType, dirName);
	/*If current room is the end room break the loop*/
	if(roomType == 2){
	    break;
	}
	/*Set correct to false*/
	correct = 0;
	/*Loop until a correct connected room is entered*/
	do{    
	    /*Print room info for game*/
	    printf("CURRENT LOCATION: %s\n",currentRoom);
	    printf("POSSIBLE CONNECTIONS: ");
	    for(i=0; i < numConnections;i++){
		printf("%s",connections[i]);
		if(i != numConnections-1){
		    printf(", ");
		}
		else{
		    printf(".\n");
		}
	    }
	    /*Get where to from user*/
	    printf("WHERE TO? >");
	    getline(&input,&bufferSize,stdin);

	    /*Check if the time command was entered*/
	    if(strncmp(input,"time", strlen(input)-1) == 0){
		/*Main thread unlocks mutex so second thread can write time to file*/
		pthread_mutex_unlock(&myMutex);
		/*Block main until the second thread completes writing time*/
		/*This is also being blocked because second thread locked mutex*/
		pthread_join(secondThread,0);
		
		/*Read time from file*/
		/*Start at beginning of file*/
		fseek(filePtrTime,0,SEEK_SET);
		/*Get last line from time file*/
		while(fgets(timeLine,100,filePtrTime) != 0){
		    /*Do nothing*/
		};
		/*Print time*/
		printf("\n %s\n",timeLine);
		
		/*Lock mutex again*/
		pthread_mutex_lock(&myMutex);
		/*Recreate second thread*/
		pthread_create(&secondThread,0,writeTime,0);
		
		/*Free memory*/
		free(input);
		/*Set to null*/
		input = 0;
		/*Get where to from user*/
		printf("WHERE TO? >");
		getline(&input,&bufferSize,stdin);
	    }	    
		/*Check if what the user entered matches any of the connecting rooms*/
		for(j=0; j < numConnections;j++){
		    /*If the input matches*/;
		    if(strncmp(input,connections[j], strlen(input)-1) == 0){
			/*then change correct to true*/
			correct = 1;
			/*Clear past room name*/
			memset(&currentRoom,0,sizeof(currentRoom));
			/*Save the input in currentRoom to be used later*/
			strncpy(currentRoom,input,strlen(input)-1);
		    }
		}

		/*Free memory*/
		free(input);
		/*Set to null*/
		input = 0;
		/*If incorrect room entered*/
		if(correct == 0){
		    printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.");
		    printf("\n");
		}	
	    printf("\n");
	/*Continue while correct is false*/
	}while(correct == 0);
	
	/*Increment step*/
	steps++;
	/*Write room to file*/
	fprintf(filePtr,"%s\n",currentRoom);
	/*Clear past room's info*/
	for(y=0; y < 6; y++){
	    memset(connections[y],0,sizeof(connections[y]));
	}
    };
    
    /*Once end room is found*/
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",steps);
    
    /*Move filePtr to beginning of file*/
    fseek(filePtr,0,SEEK_SET);
    /*Read and print each line*/
    while(fgets(pathLine,100,filePtr) !=0){
	printf("%s",pathLine);
    }
    /*Close path and time file*/
    fclose(filePtr);
    fclose(filePtrTime);
    
    /*Remove file*/
    remove(file_path);
    
    return 0;
}


