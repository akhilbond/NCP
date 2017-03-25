//Method to check the number of matches in the string
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>


int check(char const* pattern, char const* filestring){

  //Integer value to keep track of number of matches
  int count = 0;

  //Finds the size of the given pattern
  int patternLen = strlen(pattern);
  //Finds the size of the given file contents
  int fileLen = strlen(filestring);

  //Creates a buffer with the size of the pattern + 1(for the terminating 0)
  char* buffer = malloc(patternLen);

  //for loop to put a portion of the filestring into the buffer
  int i = 0;
  for (i = 0; i < fileLen; i++) {

    strncpy(buffer, filestring+(i), patternLen);

    //If statement checks if the pattern matches the substring
    if (strcmp(pattern, buffer) == 0 && strlen(pattern)==strlen(buffer)) {
      //increment the counter
      count ++;
    }

  }


  free(buffer);
  //Return the number of matches
  return count;

}

void make_lower(char* string){
  int i;
  int len = strlen(string);

  for(i=0;i<len;i++){
    if(strcmp(string," ") == 0){
      return;
    }
    else{
      string[i] = tolower(string[i]);
    }
}

}
