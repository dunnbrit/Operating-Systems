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
void getStartRoom(char* startRoom){
    
}

/*
 *Returns the current room name, the connected room names, number of connections,
 *and room type (using pointers)
 */
void getCurrentRoomInfo(char* roomName, char** connectedRooms, int* numConnected, int* type){
    
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
    getStartRoom(currentRoom);
    
    
    /*getCurrentRoomInfo(currentRoom, connections, &numConnections, &roomType);*/
    
    
    
    
    return 0;
}


