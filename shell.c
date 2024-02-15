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

int main(){
    startup_text();

    int num_chars_entered = -5;
    int current_char = -5;
    size_t buffer_size = 0;
    char* line_entered = NULL;
    
    while(1){
        printf("%d:", getpid());
        fflush(stdin);
        num_chars_entered = getline(&line_entered, &buffer_size, stdin);

        // if(line_entered[0] = '\n'){

        // }
        if(check_blank(line_entered) == 1){
            printf("empty line");
        }
        else{
            printf("line entered: %s\n",line_entered);
        }

        free(line_entered);
        line_entered = NULL;
    }


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