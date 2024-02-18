#ifndef __SHELL_H
#define __SHELL_H

void startup_text();
int check_blank(char* line);
void parse_command(char* line);
void process_command(char* command, char* arg1, char* arg2);
char* variable_expansion(char* line);

#endif