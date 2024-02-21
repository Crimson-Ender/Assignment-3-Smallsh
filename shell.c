/*
*   MAX BAKER
*   bakerm7@oregonstate.edu
*   February 19th, 2024
*   shell.c
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "shell.h"

int running = 1;
int exitStatus = 0;

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
        line[strlen(line)-1] = '\0';

        if(strlen(line)==0|| strcmp(line, "\n")==0){
            printf("empty line");
        }else{
            line[sizeof(line)-1] = "\0";
            // if(strstr(line, "$$")){
            //     line = variable_expansion(line);
            // }
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

    //gets the current time and converts it into the current local time of the user
    time_t current;
    time(&current);
    struct tm *current_time = localtime(&current);

    //prints out the time and a different greeting at different times of the day
    if(current_time->tm_hour >=18){
        //after 6pm
        printf("Good evening! The current time is %d:%0.2dpm on %d/%d/%d\n", 
        current_time->tm_hour-12, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
    }else if(current_time->tm_hour >= 12 && current_time->tm_hour < 18){
        //after 12pm but before 6pm
        printf("Good afternoon! The current time is %d:%0.2dpm on %d/%d/%d\n", 
        current_time->tm_hour-12, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
    }else if(current_time->tm_hour < 12 && current_time->tm_hour >=4){
        //before 12pm but after 4am
        printf("Good morning! The current time is %d:%0.2dam on %d/%d/%d\n", 
        current_time->tm_hour, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
    }else{
        //between midnight and 4am
        printf("The current time is %d:%0.2dam on %d/%d/%d. You should probably be going to bed now\n", 
        current_time->tm_hour, current_time->tm_min, current_time->tm_mon+1,current_time->tm_mday,current_time->tm_year+1900);
        printf("Trust me, you're gonna feel like crap in the morning if you don't. I know I am not the boss of you, but still\n");
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
    //runs through the line the user entered and parses out the command and the first two arguments entered
    char* save_string; //to prvent the command the user passed from getting ruined in the token process
    char* command[256]; //initializes an array of 256 characters to 

    char* token = strtok_r(line, " ", &save_string);

    if(token == NULL){
        return;
    }
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
        process_command(command, arg1, arg2, line);
    }

}

void process_command(char* command, char* arg1, char* arg2, char* line){

    if(strcmp(command, "cd")==0){
        printf("this is what would change directory");
        change_directory(command, arg1);
    }else if(strcmp(command, "status")==0){
        return_status(command);
    }else if(strcmp(command, "exit")==0){
        //this will end the program
        running = 0;
        return;
    }else if(strcmp(command, "#")==0){
        return;
    }else{
        //this will run a command that is not build in
        not_built_in(command, line);
    }

}

void change_directory(char* command, char* path){
    char* current_dir[100];
    if(path == NULL){
        path = getenv("HOME");
    }
    int ret = chdir(path);
    printf("success status: %d\n",ret);
    printf("current directory: %s\n", getcwd(current_dir, 100));
    return;
}

void return_status(char* command){
    printf("Exit status: %d\n", exitStatus);
    return;
}

char* variable_expansion(char* line){
    char* extended_string = malloc(sizeof(char)*512);
    char* delimPlace = strstr(line, "$$");

    char* pretext[255];
    char* posttext[255];
    //memset(pretext);
    strncpy(pretext, line, delimPlace- line);
    //memset(posttext);
    strncpy(posttext, line, line-delimPlace);

    sprintf(extended_string, "%s%d%s", pretext, getpid(), posttext);

    free(line);
    return extended_string;
}

void not_built_in(char* command, char* line){
        
    if(line[strlen(line)-1] == '&'){
        printf("this would run as a background process once that functionality is implemented\n");
    }

    char* args[] = {command, NULL};
    
    pid_t spawnpid = fork();
    int background_process = 0;
    switch(spawnpid){
        case -1:
            perror("Fork failed!\n");
            exit(1);
            break;
        case 0:
            printf("executing file %s\n", args[0]);
            execvp(command, args);
            perror("execvp() failed!");
            exit(2);
            break;
        default:
            printf("I am the parent of the child %d\n", spawnpid);
            spawnpid = waitpid(spawnpid,  &exitStatus, 0);
            printf("the parent is done waiting. The pid of the child that terminated is %d\n",spawnpid);
            break;
    }
    return;

}

char* parse_arguments(char* line, char* exec_path){
    char* save_string;
    char* token = strtok_r(line, " ", &save_string);
    char* arg[512];
    
    arg[0] = exec_path;
    int i = 1;
    while(token != NULL){

        token = strtok_r(NULL, " ", &save_string);        
        if(token!=NULL){
            arg[i] = token;
        }
        i++;
    }
    arg[i] = NULL;

    printf("arg[0] == %s\n",arg[0]);
    // for(int j = 0; j<i; j++){
    //     if(arg[j] == NULL){
    //         printf("NULL\n");
    //         break;
    //     }else{
    //         printf("%s\n", arg[j]);
    //     }
    // }

    return arg;
    
}