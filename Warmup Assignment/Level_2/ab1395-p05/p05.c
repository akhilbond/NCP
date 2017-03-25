//Akhilesh Bondlela
//RUID: 159007598
//NetID: ab1395

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

//Header function which includes functions repeated throughout the program
#include"check.h"

int main(int argc, char *argv[]) {

  int sysflag = 0;

  //Variable to hold the file size
  long end;
  //Variable to hold the total number of times the pattern appears
  int sum = 0;

  //An Array to hold the various patterns inputted by the user
  char* pattern[argc];
  int i;

  //For loop to fill the pattern array with the provided patterns
  for(i=2;i<argc;i++){
    if(strcmp(argv[i],"--systemcalls") == 0){
      argc--;
      sysflag = 1;
      while(i<argc){
      pattern[i] = argv[i+1];
      i++;
      }
    }
      pattern[i] = argv[i];
  }


  //Calls the make lower function on all of the patterns in the array
  //This makes all the letters in the pattern lowercase
  for(i=2;i<argc;i++){
    make_lower(pattern[i]);
  }

  //Reading the data from the file//

  //Initialize a file pointer
  FILE* fp;

  //Open the file
  fp = fopen(argv[1],"r");

  //Check if the file pointer is NULL
  if(fp==NULL)
  {
    //If file pointer NULL, then error and exit
    perror("./p05");
    exit(1);
  }
  else
  {
    //Move the pointer to the end
    fseek(fp, 0, SEEK_END);
    //Record the file size in the variable end
    end = ftell(fp);
    //Move the file pointer back to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    //Integer for the number of matches found in the file
    int found;

    //This for loop itterates through each of the entries in the pattern array
    for(i=2;i<argc;i++){
      //Makes sure that the pointer starts at the beginning every iteration
      fseek(fp, 0, SEEK_SET);

      //Check the pattern size of the current pattern
      int patternsize = sizeof(pattern[i]);
      //Create a buffer of size of pattern
      char* buffer = malloc(patternsize+1);
      char* readbuff = malloc(sizeof(char));

      //Variable currentPos holds the current pointer position
      long currentPos = ftell(fp);

      //As long as the file pointer is not at the end of the file
      while(currentPos<end){

      if(sysflag==1){

      } else{
      //Move the pointer to a specific position given by currentPos
      fseek(fp,0,currentPos);
    }

      //Read the file contents and stores it in the buffer
      if(sysflag == 1){
        int tmp;
          for(tmp = 0; tmp<patternsize-1; tmp++){
            read(fp,readbuff,1);
            buffer[tmp] = readbuff[tmp];
          }
      }else{
      int tmp;
        for(tmp = 0; tmp<patternsize; tmp++){
          fread(readbuff,1,1,fp);
          buffer[tmp] = *readbuff;
        }
      }

      //Sets the concluding character 0 for the string in the buffer
      buffer[patternsize]=0;
      //Looks for the pattern[i] in the buffer
      found = check(pattern[i], buffer);

    //Sums up the number of times that the pattern matches through the file
    sum = sum + found;
    //Iterates the currentPos variable by the size of the current pattern
    currentPos = currentPos + patternsize;
  }
      //Frees the memory allocated for the buffer
      free(buffer);
      //Prints the number of times the pattern is found in the file
      printf("%d\n", sum);
      //Set the sum variable back to 0 for the next iteration
      sum = 0;
    }

    //Close the file
    fclose (fp);
  }


  //Conclude the program
  return 0;

}
