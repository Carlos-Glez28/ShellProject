CC=gcc
DEPS = Builtins.h

shell: shell.c Builtins.c
	gcc -o shell shell.c Builtins.c -Wall -Werror
