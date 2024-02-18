#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "shell.h"

int running = 1;

int main(){
    startup_text();

    int num_chars_entered = -5;
    int current_char = -5;
    size_t buffer_size = 0;
    char* line = NULL;
    
    do{
        printf("%d: ", getpid());
        fflush(stdin);
        num_chars_entered = getline(&line, &buffer_size, stdin);

        if(strlen(line)==0|| strcmp(line, "\n")==0){
            printf("empty line");
        }else{
            if(strstr(line, "$$")){
                line = variable_expansion(line);
            }
            parse_command(line);
        }

        free(line);
        line = NULL;
    }while(running == 1);


    printf("\n");
    return 0;
}

void startup_text(){
    //doesn't return anything, just prints out the title of the program, my name, and the current date and time
    printf("\n\n==============================================================\n\n");
    printf("SmallShell (smallsh) by Max Baker\n");

    time_t current;
    time(&current);
    struct tm *current_time = localtime(&current);

    if(current_time->tm_hour >=18){
        printf("Good evening! The current time is %d:%dpm on %d/%d/%d\n", 
        current_time->tm_hour-12, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
    }else if(current_time->tm_hour >= 12 && current_time->tm_hour < 18){
        printf("Good evening! The current time is %d:%dpm on %d/%d/%d\n", 
        current_time->tm_hour-12, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
    }else{
        printf("Good morning! The current time is %d:%dam on %d/%d/%d\n", 
        current_time->tm_hour, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
    }

    printf("\n==============================================================\n\n");
}

int check_blank(char* line){
    //loops through the string to check if it is not just full of whitespace or just a newline character

    int is_blank = 1;
    for(int i = 0; i < strlen(line); i++){
        if(line[i] != ' ' || line[i] != '\n'){
            is_blank = 0;
        }
    }
    return is_blank;
}

void parse_command(char* line){
    char* save_string; //to prvent the command the user passed from getting ruined in the token process
    char* command[256];

    char* token = strtok_r(line, " ", &save_string);

    strcpy(command, token);

    if(strcmp(command, "#")==0){
        //this is a comment, we do not process anything from here.
        printf("comment");
        return;
    }else if(strlen(command)==0){
        //printf("empty line");
        return;
    }else{
        
        char* arg1 = strtok_r(NULL, " ", &save_string);
        char* arg2 = strtok_r(NULL, " ", &save_string);

        //printf("DEBUG: command: %s, argument 1: %s, argument 2: %s\n", command, arg1, arg2);
        process_command(command, arg1, arg2);
    }

}

void process_command(char* command, char* arg1, char* arg2){

    if(strcmp(command, "cd")==0 || strcmp(command, "cd\n")==0){
        printf("this is what would change directory");
    }else if(strcmp(command, "status")==0){
        printf("this would print status");
    }else if(strcmp(command, "exit")==0){
        //this will end the program
        running = 0;
        return;
    }else if(strcmp(command, "#")==0){
        return;
    }else{
        //this will run a command that is not build in
    }

}

void change_directory(char* command, char* arg1){

}

char* variable_expansion(char* line){
    char* expanded_string = malloc(strlen(line)+(sizeof(int)*10));
    char* variable[10];
    sprintf(variable, "%d", getpid());
    int i, j = 0;

    for(i = 0; i<strlen(line); i++){
        if(line[i] == '$'){
            strcat(expanded_string,variable);
            i++;
        }else{
            expanded_string[j] = line[i];
        }
        j++;
    }

    printf("Expanded string == %s", expanded_string);

    free(line);
    return expanded_string;
}