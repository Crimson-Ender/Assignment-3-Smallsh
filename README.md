# Assignment 3: smallsh
by Max Baker(bakerm7@oregonstate.edu)

OVERVIEW:
This program is a simple unix shell, written in the C programming language. Users will interface with the program through a command prompt, and they will be able to execute a number of commands through the command line interface. A handful are built-in and written from scratch, while others execute pre-existing programs.

CONTENTS:
This program consists of two code files; shell.c and shell.h, as well as a makefile to compile it.

FEATURES:
This program has most of the functionality of a simple UNIX shell. It contains three custom built-in commands, and the ability to run a number of other bash commands.

BUILT-IN COMMANDS

cd - Changes the directory; the user can either specify a file path to navigate to, or pass in no argument and be returned to the home directory as specified in the HOME enviroment variable. This command only takes one argument and will ignore the rest of the arguments passed into it.

status - prints out the exit status of the most recently run command. This command takes no arguments, and will simply ignore all arguments passed.

exit - ends the program and clears dynamic memory allocated. This command also takes no arguments, and will simply ignore all arguments passed as well.

"#" - the hashtag symbol represents a comment. If the # symbol is used as the command for a line, the program will just ignore the line and prompt the user for a new line.

EXAMPLES OF OTHER COMMANDS THAT AREN'T BUILT IN
ls - lists the files present at a specific path; with no arguments it prints the contents of the current directory

echo - prints back the string you put in; the smallsh program itself with expand the variable $$ into the process id of the smallsh program.

ping - pings the server you are running the shell on

mkdir - makes a directory

in short, you can run most linux commands through this shell


HOW TO COMPILE:
This program is compiled with a Makefile. All you need to do to complile it is use the "make" command in the terminal once you have navigated to a directory containing the program's files. The makefile compiles the program using GCC to the C99 standard. The program will be compiled into an executable called "smallsh". You will be able to run it in the terminal with the command "smallsh".

KNOWN ISSUES:
I couldn't get the signal that is thrown when a background child process terminates to work properly, so it is currently unused. Otherwise, background child processes are completely functional.