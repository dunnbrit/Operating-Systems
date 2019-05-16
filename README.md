# Operating-Systems
Spring 2019

Program 1: Matrix Operations
  Bash shell scripts used to compute different matrix operations including: dims, transpose, mean, add, and multiply
  Command to run grading script: ./p1gradingscript matrix

Program 2: Adventure
  Adventure game written in C89 similar to: http://en.wikipedia.org/wiki/Colossal_Cave_Adventure
  Creates a directory using process ID. Then generates 7 different room files. 
  The game reads the room files to generate game play. User input is taken to determine next game move. 
  The game also tracks the current time. Time keeping is done through reading and writing to a file named "currentTime.txt" using threads and mutexes.
  Commands to compile and run:
  gcc -o dunnbrit.buildrooms dunnbrit.buildrooms.c
  dunnbrit.buildrooms
  gcc -o dunnbrit.adventure dunnbrit.adventure.c -lpthread
  dunnbrit.adventure
  
