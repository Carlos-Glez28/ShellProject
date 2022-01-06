#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "Builtins.h"
int s_cd(char **args); //changes directory
int s_clr(char **args); //clears the screen
int s_dir(char **args); //displays all directories in the current directory
int s_environ(char **args); //displays all environment variables
int s_echo(char **args); //prints anything after the "echo" token
int s_help(char **args); //prints the User Manual to the screen using the more filter (manually created)
int s_pause(char **args); //pauses the shell until ONLY ENTER IS PRESSED.
int s_quit(char **args); //quits the Shell
int isBuiltin(char *argv[]);// checks that input is a builtin

extern char **environ;//global environmentvariable

char* Builtin[] = { "cd", "clr", "dir", "environ", "echo", "help", "pause", };// holds all builtin names

int (*builtin_func[])(char **) = { &s_cd, &s_clr, &s_dir, &s_environ, &s_echo, &s_help, &s_pause};//holds address of functions

int isBuiltin(char *argv[]){
  int i;
  for(i = 0; i < sizeof(Builtin) / sizeof(Builtin[0]); i++){//iterates through argv
    if(strcmp(argv[0], Builtin[i]) == 0){//checks if builtin
      return 1;
    }
  }
  return 0;//not a builtin value
}

int s_quit(char **args){
  if(strcmp(args[0], "quit") == 0){
    printf("Exiting Shell\n");
    exit(0);
  }
  return(0);//error handling, wont compile without this
}

int s_cd(char **args){
  if(args[1] == NULL){//checks if there is an argument
    char wd[1024];
    getcwd(wd, sizeof(wd));//gets current directory
    printf("The current Directory is: %s\n", wd);//prints directory
  }else{
    if(chdir(args[1]) != 0){
      perror("cd error\n");//can't change directory so error handling
    }
  }
  return(1);
}

int s_dir(char **args){
  DIR *dir;//directory pointer
  struct dirent *rdr = NULL;;//reads directory
  dir = opendir("./");//opens cd
  if(dir != NULL){//if directory isnt empty
    while(rdr == readdir(dir)){//reads through directory
      puts(rdr -> d_name);//prints
      (void) closedir(dir);//closers dir
    }
  }else{
    perror("Error: Directory cannot be opened");
  }
  return 0;
}

int s_environ(char **args){
  int i;
  char *eVariable = *environ;//environment pointer
  for(i = 1; eVariable; i++){//goes until null
    printf("%s\n", eVariable);//prints
    eVariable = *(environ + i);//sets next eviron
  }
  return 0;
}

int s_pause(char **args){
  char Buffer[500] = " ";//User input
  printf("Shell is paused. Press enter to continue\n");
  while(strcmp(Buffer, "\n") != 0){//while user hasn't pressed ENTER
    fgets(Buffer, sizeof(Buffer), stdin);
  }
  printf("Shell has resumed\n");
  return(1);
}

int s_clr(char **args){
  printf("\e[1;1H\e[2J");//ansi escape code
  return(1);
}

 int s_help(char **args){
  FILE *fp = fopen("readme_doc", "r");//file points to get readme
  int a = s_clr(args);//clears screen
  (void)a;
  int j = 0;
  struct winsize Wsize;//dimensions of window
  ioctl(0, TIOCGWINSZ, &Wsize);//dimensions of window
  char MFile[1024];//lines from file
  while(fgets(MFile,1024, fp) != NULL){//checks for end of file
    printf("%s", MFile);//print strings
    j++;
    if(j == Wsize.ws_row - 1){//if it reaches the end of the number of rows
      char buffer[10];//checks for input
      while(fgets(MFile, 1024, fp) != NULL){//not end of file
        fgets(buffer, sizeof(buffer), stdin);//not end of file
        if(strcmp(buffer, "\n") == 0){//enter
          char * t = strtok(MFile, "\n");//gets rid of new line
          (void)t;
          printf("%s", MFile);//prints next line
        }
        if(strcmp(buffer, "q\n") == 0){//checks if user enters q
          fclose(fp);//close file
          return 1;
        }
      }
      fclose(fp);//closes file
      return 1;
    }
  }
  return 0;
}

int s_echo(char **args){
  int i = 1;
  while(args[i] != NULL){//checks if there are strings
    printf("%s ", args[i]);//prints word
    i++;
  }
  printf("%s", "\n");//newline
  return 1;
}
