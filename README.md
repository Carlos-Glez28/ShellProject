# Project 2 Developing a Linux shell
__Shell Manual__

__Description:__ This project consists of a shell program using the command line terminal
or whatever command line the user has. The commands will either be written in a file and
the program will take care of the rest or it will be user inputted. If a file is used, when
the end of the file is reached the program terminates. All inputs are to be followed by
enter or return depending on the user's system.

__Execution__

User input:

          ./shell cd

  Comment: The user can input any command line argument or letters, I have not
  been able to debug the part that gives me a segmentation fault if there is no
  argument.

File Input:

        ./shell Batchfile.txt

  Comment: The file must be Batchfile.txt, contain commands and will terminate the
  program once the end has been reached.

__General Example of using a generic shell__

        Unix/Linux shells are case sensitive and will
        not work if it is not spelled exactly as it previously
        was.
        {command} {argument}

__Builtins__

The program consisted of 8 builtin functions. The first builtin is __cd__ which changes the current
directory of or if no arguments are passed then the path of current directory is printed out. The next builtin
is __dir__ which lists the contents of the dir. The builtin __clr__ simply clears the screen of all previous
text. The builtin __environ__ prints the environment strings to the screen. The builtin
function __echo__ takes an argument and displays the argument to the screen or the file that it is being directed to.
The next builtin is __help__ which displays the README_DOC file. The next builtin is __pause__ which pauses the Shell
until "enter" or "return" is pressed. "Return" is for macOS users. The final builtin is
__quit__ which ultimately quits the user out of the shell.

Note: Aside from the builtin commands the user can type in commands that can be used in a linux shell. Those commands
get exec to the actual unix shell.

__I/O Redirection__
- Input Redirection: Using the '__<__' symbol the user will use the file as the argument
to the command that is being calculated.
      Example: Command Argument < file
- Output Redirection: Using the '__>__' or '__>>__' symbol the user will use the file after those symbols are the place where the result of the command is printed to.
      Example: Command Argument > file
      Example: Command Argument >> file

__Piping__

The piping symbol '__|__' can also be used in this project. The command before the pipe symbol will be the
 command that is used as the input for the pipe. The command after the symbol will read the pipe and run that as the input.
Piping allows the two commands to speak to each other directly.

__Running Background__

The '__&__' after a command will make the command run in the background and lets other commands run at the same time.
