/*
*   MAX BAKER
*   bakerm7@oregonstate.edu
*   February 19th, 2024
*   shell.h
*
*/

#ifndef __SHELL_H
#define __SHELL_H

void startup_text();
int check_blank(char* line);
void parse_command(char* line);
void process_command(char* command, char* arg1, char* arg2, char* line);
char* variable_expansion(char* line);
void change_directory(char* command, char* path);
void return_status(char* command);
void not_built_in(char* command, char* line);

#endif