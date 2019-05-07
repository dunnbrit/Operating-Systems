/**********************************Setup**************************************/

/*Libraries*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

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
	printf("dir open\n");
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
		    /*Set a loop counter*/
		    int loop = 0;
		    /*Loop until at last line of file*/
		    while(fgets(readLine,100,fp) != 0){
			/*Get the number of lines*/
			loop++;
		    }
		    
		    printf("Lines: %d\n",loop);
		    

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



/********************************Main******************************************/

int main(){
/*Newest Directory to Use*/
    /*Holds the newest dirName*/
    char dirName[256];
    /*Call function to get the newest directory name*/
    getNewestDirectory(dirName);
    printf("Newest entry found is: %s\n", dirName);
    
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
    /*First get the name of the start room*/
    getStartRoom(currentRoom, dirName);
    printf("currentRoom: %s|",currentRoom);
    getCurrentRoomInfo(currentRoom, connections, &numConnections, &roomType, dirName);
    
    
    
    
    return 0;
}


