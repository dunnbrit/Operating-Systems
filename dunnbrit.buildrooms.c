/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>



/*Struct for rooms*/
struct Room{
    /*Room name*/
    char* name;
    /*Counter for outbound connections*/
    int numOutboundConnections;
    /*Array to link outbound connections of 3-6*/
    struct Room* outboundConnections[6];
    /*Room type*/
    char* roomType;   
};

/*Array for room types*/
char* RoomType[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};
/*Array for file names*/
char* FileName[10] = {"/blue_room", "/red_room", "/yellow_room", "/green_room", "/pink_room", "/purple_room", "/orange_room", "/black_room", "/white_room", "/silver_room"};
/*Array for room names*/
char* RoomName[10] = {"blue", "red", "yellow", "green", "pink", "purple", "orange", "black", "white", "silver"};



/*
 * Allocate(): allocates memory for all the room structs. Returns a pointer for all the rooms.
 */
struct Room* allocate(){
    /*Allocate memory for all rooms*/
    int numRooms = 7;
    struct Room *roomsPtr = malloc(numRooms*sizeof(struct Room));
    int i;
    /*Return the pointer*/
    return roomsPtr;
}

/*
 *  Returns true if all rooms have 3 to 6 outbound connections, false otherwise
 */
int IsGraphFull(struct Room* input)  
{
    /*Check the number of outbound connections of each room*/
    int i;
    for(i=0; i < 7; i++){
	/*If a room has less than 3 connection*/
	if(input[i].numOutboundConnections < 3){
	    /*then return false*/
	    return 0;
	}
    }
    return 1;
}

/*
 *  Returns a random Room, does NOT validate if connection can be added
 */
struct Room* GetRandomRoom(struct Room* input)
{
    /*Get a random number*/
    int randomNum = rand() % 7;
    
    /*Return the random room*/
    return &input[randomNum];
}

/*
 * Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
 */
int CanAddConnectionFrom(struct Room* x) 
{
    if(x->numOutboundConnections < 6){
	return 1;
    }
    else{
	return 0;
    }
}

/*
 *  Returns true if a connection from Room x to Room y already exists, false otherwise
 */ 
int ConnectionAlreadyExists(struct Room* x, struct Room* y)
{
    int i;
    /*Go through all connection to check if there is a match*/
    for(i=0; i < x->numOutboundConnections; i++){
	/*If the room is the same room*/
	if(x->outboundConnections[i] == y){
	    /*Then return true*/
	    return 1;
	}
    }
    /*if here then there was not a connection so return false*/
    return 0;
}

/*
 * Connects Rooms x and y together, does not check if this connection is valid
 */
void ConnectRoom(struct Room* x, struct Room* y) 
{
    /*add room y to room x outboundConnections*/
    x->outboundConnections[x->numOutboundConnections]= y;
    /*increase room x number of outbound connections*/
    x->numOutboundConnections++;
}

/*
 *  Returns true if Rooms x and y are the same Room, false otherwise
 */
int IsSameRoom(struct Room* x, struct Room* y) 
{
    /*Check if the same struct Room ptr*/
    if(x == y){
	/*if so return true same room*/
	return 1;
    }
    else{
	/*Otherwise return false*/
	return 0;
    }
}

void AddRandomConnection(struct Room* input)  
{
    /*Variables for random Rooms*/
    struct Room* A;  
    struct Room* B;
    
    /*Get a random room*/
    while(1){
	/*Call function to get a ptr to a random room*/
        A = GetRandomRoom(input);
	
	/*Check if A has space for more outbound connections*/
	if (CanAddConnectionFrom(A) == 1){
	    /*If so then exit loop*/
	    break;
	}
    }

    /*Get another random room*/
    do{
	/*Call fucntion to get a ptr to a andom room*/
	B = GetRandomRoom(input);
    }
    /*Check coonection is possible, if not get a new random room*/
    while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1|| ConnectionAlreadyExists(A, B) == 1);

    /*Connect Rooms*/
    ConnectRoom(A, B);
    ConnectRoom(B, A);

}



int main(){
/*Initialize Random*/
srand(time(0));


/*Create a new directory*/    
    /*Path for files in the directory*/
    char dirPath[] = "dunnbrit.buildrooms.";
    
    /*Get the process id for the rooms program*/
    int pid = getpid();
    
    /*Will hold full directory name*/
    char dirName[30];
    /*Combine dirPath and pid into dirName*/
    sprintf(dirName,"%s%d",dirPath,pid);
    
    /*Make rooms directory*/
    if(mkdir(dirName,0755) != 0){
	printf("Error making directory");
	exit;
    }
 

/*Get 3 fileNames/Rooms not to use*/
    int skipNum[3];
    int i;    
    /*Copy dirName into new variable because it won't work after loop otherwise*/
    char directoryName[30]; 
    strcpy(directoryName,dirName);
    
    do{
	/*Clear memory if skipNum didn't assign different values*/
	memset(&skipNum,0,sizeof(skipNum));
	for(i = 0; i < sizeof(skipNum);i++){
	    /*Get a random number from 0-9 which corresponds to the FileName/RoomName array*/
	    skipNum[i] = rand() % 10;
	}
    /*Make sure none of the numbers are the same*/
    }while(skipNum[0] == skipNum[1] || skipNum[1] == skipNum[2] || skipNum[0] == skipNum[2]);
 
    
/*Create all the rooms before the files*/
    /*Call allocate to allocate memory for all the room structs*/
    struct Room* allRooms = 0;
    allRooms = allocate();
    
    /*Add selected rooms to allRooms*/
    int index = 0;
    for(i=0; i < 10; i++){
	/*Check if the fileName is one to skip*/
	if(i != skipNum[0] && i != skipNum[1] && i != skipNum[2]){
	    /*If not then assign room in struct*/
		/*Assign name to room in allRooms*/
		allRooms[index].name = calloc(16,sizeof(char));
		strcpy(allRooms[index].name, RoomName[i]);
		/*Set number of connections to 0*/
		allRooms[index].numOutboundConnections = 0;
		/*Increment index*/
		index++;
	}
    }
    
  
/*Create all room connections in graph*/
    /*Call IsGraphFull until enough connections have been made*/
    while (IsGraphFull(allRooms) == 0){
	/*Call AddRandomConnection to connect two rooms*/
	AddRandomConnection(allRooms);
    }

/*Assign Room Types*/
    int start;
    int end;
    
    do{
	/*Get random start room and end room*/
	start = rand() % 7;
	end = rand() % 7;
    /*Repeat until two different rooms are selected*/
    }while(start == end);
    
    /*Assign the room types*/
    for(i=0;i < 7; i++){
	/*Allocate memory*/
	allRooms[i].roomType = calloc(16,sizeof(char));
	/*if start room*/
	if(i == start){
	    /*Assign type START_ROOM*/
	    strcpy(allRooms[i].roomType, RoomType[0]);
	}
	/*if end room*/
	else if(i == end){
	    /*Assign type END_ROOM*/
	    strcpy(allRooms[i].roomType, RoomType[2]);	    
	}
	/*otherwise its a mid room*/
	else{
	    /*Assign type MID_ROOM*/
	    strcpy(allRooms[i].roomType, RoomType[1]);
	}
    }
	
  
/*Make the new files with room info*/
    int file_descriptor; /*used to check for errors*/
    char filepath[45];
    index = 0;
    for(i=0; i < 10; i++){
	/*Check if the fileName is one to skip*/
	if(i != skipNum[0] && i != skipNum[1] && i != skipNum[2]){
	    /*Clear the last filepath*/
	    memset(&filepath,0,sizeof(filepath));
	    
	    /*Create the new filepath*/
	    strcat(filepath,directoryName);
	    strcat(filepath,FileName[i]);

	    /*Create the file in the directory created earlier*/
	    file_descriptor = open(filepath, O_RDWR | O_CREAT, 0644);
	    
	    /*Check no errors in file creation*/
	    if(file_descriptor < 0){
		printf("Error with file creation");
	    }
	    /*If not then add room info*/
	    else{
		printf("ROOM NAME: %s\n", allRooms[index].name);
		/*Loop through all the connecting rooms*/
		int j;
		for(j=0; j < allRooms[index].numOutboundConnections; j++){
		    printf("CONNECTION %d: %s\n", j, allRooms[index].outboundConnections[j]->name);
		}
		printf("ROOM TYPE: %s\n", allRooms[index].roomType);
		/*Increment index*/
		index++;
	    
		/*Close file*/
		close(file_descriptor);
	    }
	}
    }
    
    
return 0;
}