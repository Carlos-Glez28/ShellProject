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
#include "Builtins.h" //includes builtin commands
#define commandSize 7
char* readLine();//reads input
void printName();//prints shell name and cd
int eval(char *);//reads input and checks what to do with it
void parser(char*, char* argv[1024]);//parses into tokens
void forkP(char* argv[1024], int );//forks program
int rBuiltin(char* argv[1024]);//run builtin if it is a builtin command
void redirect(char * argv[1024], int, int);//redirection process
int isredirect(char * argv[1024], int);//checks whether the input contains a redirect symbol
int ispipe(char *argv[1024], char *left[1024], char *right[1024]);//checks if input has a pipe
void pipeEval(char *left[1024], char *right[1024], int flag);//handles piping

int length;


/*Prints cwd, handles batchfile or input and calls functions*/
int main(int argc, char* argv[]){
  int a = strcmp(argv[1], "Batchfile.txt");
  if(a == 0){//Checks for batchfile flag
    char input[1024];
    char *line;
    FILE *fp = fopen(argv[1], "r");//Opens batchfile
    while(fgets(input, sizeof(input), fp)){//reads from file
      line = input;
      int throwaway = eval(line);
      (void)throwaway;
    }
    fclose(fp);
  }else{//no batchfile
    char *line;
    system("clear");
    while(1){
      printName();
      line = readLine();//reads user input
      if(strcmp(line, "quit\n") == 0 || feof(stdin)){
        return EXIT_SUCCESS;//quit shell
      }
      int throwaway = eval(line);//evals input
      (void)throwaway;
    }
  }
}

char *readLine(){
  char input[1024];
  char *line;
  fgets(input, sizeof(input), stdin);//reads input
  line = input;//saves input to line
  return line;
}

void printName(){
  char host[1024];//hostnames
  char cwd[1024];//working directory
  if(getcwd(cwd, sizeof(cwd)) != NULL){//checks for errors
    gethostname(host, 1024);//gets hostname
    printf("%s%s MyShell >", host, cwd);//prints hostname and cwd
  }else{
    perror("getcwd() error\n");
  }
}

int eval(char *line){
  char *argv[1024];//input
  int flag = 0;//flag to check for &
  parser(line, argv);//splits into tokens
  if(strcmp(argv[length - 1], "&") == 0){
    flag = 1;//sets flag to true
    argv[length - 1] = NULL;// deletes &
  }
  int throwaway = isredirect(argv, flag);
  if(throwaway == 2){
    return 1;//exits bc pupe functions does it own thing
  }
  if(throwaway == 0){//no direction
    if(argv[0] == NULL){//if not input
      return 0;//ignores input
    }
    else if(isBuiltin(argv) == 1){//if it is a builtin
      rBuiltin(argv);//run the builtin commands
    }
    else{//not a builtin
      forkP(argv, flag);//execs the program
      return 1;
    }
    return 1;
  }
  return 0;
}

int rBuiltin(char *argv[1024]){
  int counter;
  for(counter = 0; counter < commandSize; counter++){//goes through builtin commands
    if(strcmp(argv[0], Builtin[counter]) == 0){//checks for match
      return(*builtin_func[counter])(argv);//run builtin
    }
  }
  return 0;
}

void redirect(char *argv[1024], int flag, int ctr){
  int p = ctr; //cycles throught token starting at ctr
  pid_t pid; //forks
  int savedin = dup(0);//saves read
  int savedout = dup(1);//saves write

  pid = fork();//forks process
  if(pid < 0){// checks for error
    printf("Error, can't fork\n");
  }
  else if(pid == 0){//child
    if(flag == 1){//redirect token >
      int output = open(argv[p + 1], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
      dup2(output, 1); //dups write side
      close(output);//STDOUT
      argv[p] = NULL;// > is now NULL
      argv[p + 1] = NULL;// file is now null
      p++;
    }
    else if(flag == 2){// redirect token <
      int input = open(argv[p + 1], O_CREAT | O_RDONLY, 0666);// opens file in r mode
      dup2(input, 0);//dups read
      close(input);//stdin
      argv[p] = NULL;//< null
      argv[p + 1] = NULL;// file NULL
      p++;
    }
    if(flag == 3){// redirect token >>
      int output = open(argv[p + 1], O_WRONLY | O_APPEND | O_CREAT, 0666);
      dup2(output, 1);// dups write side
      close(output);//closes STDOUT
      argv[p] = NULL;// >> null
      argv[p + 1] = NULL;// file NULL
      p++;
    }
    execvp(argv[0], argv);//executes command
  }
  else if(pid > 0){
    waitpid(pid, NULL, WCONTINUED);//waits
  }
  dup2(savedin, 0);//restores read
  close(savedin);
  dup2(savedout, 1);// restores stdout
  close(savedout);
}

void forkP(char *argv[1024], int flag){
  pid_t pid;
  pid = fork();//forks process
  if(pid < 0){//error handling
    printf("Error, can't fork\n");
  }
  else if(pid == 0){//child process
    if(execvp(argv[0], argv) < 0){//checks errors
      printf("Command not found\n");
      exit(0);//exits process;
    }
    printf("Error not a valid command");//not a command at all
  }
  else if(pid > 0 && flag == 0){//parent process and no background
    int state;
    if(waitpid(pid, &state, 0) < 0){//error in waitng
      printf("ERROR\n");
    }
  }
}

int isredirect(char *argv[1024], int flag){
  int redirected = 0;//flag
  char *left[1024];//left side of token
  char *right[1024];//right side
  int i = 0;
  while(argv[i] != NULL){//checks if there are still strings
    if(ispipe(argv, left, right) == 1){//checks for a pipe
      pipeEval(left, right, flag);//calls pipe
      return 2;//returns pipe flag
    }if(strcmp(argv[i], ">") == 0){//checks for >
      redirect(argv, 1, i);//redirects
      redirected = 1;//sets redirect flag to true
    }if(strcmp(argv[i], "<") == 0){//checks for <
      redirect(argv, 2, i);//redirects
      redirected = 1;
    }if(strcmp(argv[i], ">>") == 0){//checks for >>
      redirect(argv, 3, i);//redirects
      redirected = 1;
    }
    i++;
  }
  return redirected;//flag
}

int ispipe(char *argv[1024], char *left[1024], char *right[1024]){
  int PNum = 0;
  int i = 0;
  int j;
  int k;
  while(argv[i] != NULL){//check if there is a string
    if(strcmp(argv[i], "|") == 0){//checks for pipe
      for(j = 0; j < i; j++){//left side
        left[j] = malloc(sizeof(char) * sizeof(argv[i]));//memory allocation
        char *str = argv[j];//token into string
        left[j] = str;//stores ptr into array
      }
      int m = 0;
      for(k = i + 1; k < length; k++){//checks for end of array
        right[m] = malloc(sizeof(char) * sizeof(argv[k]));//memory allocation
        char *str = argv[k];//token into string
        right[m] = str;//stores ptr into array
        m++;
      }
      PNum = 1;//flag is true
    }
    i++;
  }
  return PNum;
}

void pipeEval(char *left[1024], char *right[1024], int flag){
  int fd[2];//r and w to end of pipe
  pid_t pid;//child process
  pid_t pid2;// second child
  int SIn = dup(0);//saves read
  int SOut = dup(1);//saves write
  (void)SIn;
  (void)SOut;
  pipe(fd);//pipes r and w
  pid = fork();
  if(pid < 0){//error
    perror("Error forking\n");
    exit(1);
  }
  if(pid == 0){//child 1
    dup2(fd[1], STDOUT_FILENO);//dups stdout
    close(fd[0]);
    if(isBuiltin(left) == 1){//checks for builtin
      rBuiltin(left);//runs
    }else{//not builtin
      if(execvp(left[0], left) < 0){//checks for error
        printf("Error exec\n");
      }
    }
  }
  else{//parent process
    pid2 = fork();
    if(pid2 < 0){//error
      perror("Error forking");
      exit(1);
    }
    else if(pid2 == 0){//child 2
      dup2(fd[0], STDIN_FILENO);//dups read
      close(fd[1]);//close write
      if(isBuiltin(right) == 1){//checks builtin
        rBuiltin(right);//runs builtin
      }else{
        if(execvp(right[0], right) < 0){//checks for built in
          printf("Error exec");
        }
      }
    }
    else if(pid > 0 && flag == 0){//parent process
      close(fd[0]);//closes r
      close(fd[1]);//closes w
      waitpid(pid, NULL, 0);// wait for 1st child to finish
      waitpid(pid2, NULL, 0);//wait for 2nd child process
    }
  }
}

void parser(char *line, char* argv[1024]){
  char *t = NULL;//saves string
  char p[] = {" \n"};//separater
  int ctr = 0;
  t = strtok(line, p);//saves
  argv[ctr] = t;//stores first word
  ctr++;
  while(t != NULL){
    t = strtok(NULL, p);//takes strings
    argv[ctr] = t;//saves strings
    ctr++;
  }
  length = ctr - 1;//actual length of array is calculated
}
