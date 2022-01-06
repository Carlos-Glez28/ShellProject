#ifndef BUILTINS_DOT_H
#define BUILTINS_DOT_H
extern char* Builtin[];//placed here so it can be accessed by shell.c without issues
extern int (*builtin_func[])(char **);//placed here so it can be accessed by shell.c without issues
int s_cd(char **args); //changes directory
int s_clr(char **args); //clears the screen
int s_dir(char **args); //displays all directories in the current directory
int s_environ(char **args); //displays all environment variables
int s_echo(char **args); //prints anything after the "echo" token
int s_help(char **args); //prints the User Manual to the screen using the more filter (manually created)
int s_pause(char **args); //pauses the shell until ONLY ENTER IS PRESSED.
int s_quit(char **args); //quits the Shell
int isBuiltin(char *argv[]);// checks that input is a builtin

#endif
