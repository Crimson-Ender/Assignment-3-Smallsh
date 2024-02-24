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

int running = 1;    //boolean that is turned to 0 when the user calls "exit". Is the conditional for the while loop
int exitStatus = 0; //integer that records the exit status of the most recently run program
int fg_only = 0;    //boolean that is toggled by the sigtstp signal and either enables or disables background processes
int sig_kill = 0;
int child_processes[100];
int process_no = 0;

int main(){
    /*Main function that handles the shell*/
    startup_text(); //print the startup text

    //variables needed to get the user's input line
    int num_chars_entered = -5;
    int current_char = -5;
    size_t buffer_size = 0;
    char* line = NULL;
    
    do{
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, fg_bg_signal);

        printf("%d: ", getpid());   //prompts the user for an input line
        fflush(stdin);  //flushes standard in
        num_chars_entered = getline(&line, &buffer_size, stdin);    //gets the input line from the user
        line[strlen(line)-1] = '\0';    //change the last character of the line to a null character to prevent bugs with the newline character

        fflush(stdout);
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
    fflush(stdout);
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
    char* command[2048]; //initializes an array of 2048 characters to
    char* line_copy[2048];
    strcpy(line_copy, line);

    //printf("line entered #1: %s\n", line);

    char* token = strtok_r(line, " ", &save_string); //parses out the command from the user input

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
        //printf("line entered #2: %s\n", line);
        //printf("line copy: %s\n", line_copy);
        process_command(command, arg1, arg2, line_copy);
    }

}

void process_command(char* command, char* arg1, char* arg2, char* line){
    //uses branching logic to determine what the next course of action is
    if(strcmp(command, "cd")==0){
        //change directory
        //printf("this is what would change directory");
        change_directory(command, arg1);
    }else if(strcmp(command, "status")==0){
        //return the exit status of the last executed program
        return_status(command);
    }else if(strcmp(command, "exit")==0){
        //this will end the program
        running = 0;
        return;
    }else if(strcmp(command, "#")==0){
        //this will be a comment and the program will ignore it
        return;
    }else{
        //this will run a command that is not build in
        not_built_in(command, line);
    }

}

void change_directory(char* command, char* path){
    char* current_dir[100]; //initialize an array to store the string of the file path
    if(path == NULL){
        //if the user did not input a path, the program will automatically set the path variable to the home directory
        path = getenv("HOME");
    }
    int ret = chdir(path);  //change directory to the path in the variable
    // printf("success status: %d\n",ret);
    // printf("current directory: %s\n", getcwd(current_dir, 100));
    return;
}

void return_status(char* command){
    //simply prints out the exit status of the last command that was run
    printf("Exit status: %d\n", exitStatus);
    return;
}

char* variable_expansion(char* line){
    char* extended_string = malloc(sizeof(char)*2048);
    
    char* delimPlace = strstr(line, "$$");
    char* pretext[delimPlace-line+1];
    char* posttext[strlen(delimPlace+2)+1];

    strncpy(pretext,line, (delimPlace-line));
    //printf("pretext == %s\n", pretext);
    strcpy(posttext, delimPlace+2);
    //printf("posttext == %s\n", posttext);

    sprintf(extended_string, "%s%d%s", pretext, getpid(), posttext);

    free(line);
    return extended_string;
}

void not_built_in(char* command, char* line){
    //handles commands that aren't built in
    char* args[512];    //this will be the arguments that are passed as part of the command itself
    //args will not contain things like the filepath for I/O redirection or the indicator of it being a background process
    char* parsed_line[512]; //this will contain both the arguments and all of the other information things the user inputted
    //parsed_line will contain things like the file path for I/O direction and the indicators for background processes
    char* save_string = line;   //prevents the line that was input by the user from getting ruined by the token process
    char* token;

    int i = 0;  //counts the elements in args[]
    int h = 0;  //counts the elements in parsed_line[]
    int last_arg = 0; //a boolean variable that will be toggled to 1 if the program hits a "<", ">"" or "&"" character
    //this is done from preventing things that are not valid arguments for a not built-in program
    int in_index = 0;   //if the program recognizes a "<" character, the next item in the line will be the file path
    //for an input file for I/O redirection. 
    int out_index = 0;  //if the program recognizes a ">" character, the next item in the line will be the file path
    //for an input file for I/O redirection. 

    //printf("line entered #3: %s\n", line);

    while(token = strtok_r(save_string, " ", &save_string)){
        parsed_line[h] = token;
        if(strcmp(token, "<")==0){
            //this will determine the file path for input redirection
            in_index = h+1; //the file path will always be the next item that gets parsed
            if(last_arg == 0){
                //this will make it so every new token will only be added to parsed_line and not args[]
                last_arg = 1;
            }
        }
        if(strcmp(token, ">")==0){
            //this will determine the file path for output redirection
            out_index = h+1; //the file path will always be the next item that gets parseds
            if(last_arg == 0){
                //this will make it so every new token from here will only be added to parsed_line and not args[]
                last_arg = 1;
            }
        }
        if(last_arg == 0 && strcmp(token, "&")!=0){
            args[i] = token;
            i++;
        }
        h++;
    }

    args[i] = NULL; //sets the last argument of the args array to a NULL pointer, or else the programs will not run correctly
    
    pid_t spawnpid = fork();    //create a new process
    int background_process = 0;

    if(strcmp(parsed_line[h-1],"&") == 0 && fg_only == 0){
        
        background_process = 1;
        //signal(SIGCHLD, child_signal);
    }


    switch(spawnpid){
        case -1:
            //handles when a fork fails
            perror("Fork failed!\n");
            fflush(stdout);
            exit(1);
            break;
        case 0:
            signal(SIGTSTP, SIG_IGN);   //ignores the fg/bg toggle signal
            signal(SIGINT, int_signal); //reactivates the interrupt signal
            sig_kill = 0;   //this number keeps track of whether this process was killed by a signal
            //fork successful
            if(out_index != 0){
                //opens the file path
                int out_file_desc = open(parsed_line[out_index], O_RDWR | O_CREAT | O_APPEND, 0755);
                
                if(out_file_desc == -1){
                    //checks if the output file is valid. if not, print an error and end the process right there
                    perror("Invalid output file!\n");
                    fflush(stdout);
                    exit(2);
                    break;
                }else{
                    //if the file is good, redirect outputs to that file
                    dup2(out_file_desc, 1);
                }
            }
            if(in_index !=0){
                //gets the file descriptor for the redirected input
                int in_file_desc = open(parsed_line[in_index], O_RDWR | O_APPEND);

                if(in_file_desc == -1){
                    //checks if the input file is valid. If not, print an error and end the process right there
                    perror("Invalid input file!\n");
                    fflush(stdout);
                    exit(2);
                    break;
                }else{
                    //if the file is good, redirect inputs to that file
                    dup2(in_file_desc, 0);
                }
            }
            if(in_index == 0 && out_index == 0 && background_process == 1){
                //if the process is running as a background process, set the I/O to /dev/null
                int dev_null = open("/dev/null", O_RDWR);
                //send the inputs and outputs to hell where they belong
                dup2(dev_null, 0);
                dup2(dev_null, 1);
            }
            //printf("executing file %s\n", args[0]);
            fflush(stdout);
            execvp(command, args);  //execute command
            perror("execvp() failed!"); //if the command fails, print out an error
            exit(2);
            break;
        default:
            //printf("I am the parent of the child %d\n", spawnpid);
            fflush(stdout);
            if(background_process == 0){
                //if the child process is a foreground process, the program will wait until it terminates to continue
                spawnpid = waitpid(spawnpid,  &exitStatus, 0);
                //printf("the parent is done waiting. The pid of the child that terminated is %d\n",spawnpid);
                fflush(stdout);
            }else{
                //if the child process is not a foreground process, the program will not wait and continue on
                printf("Process %d running in the background\n", spawnpid);
                fflush(stdout);
                child_processes[process_no] = spawnpid;
                process_no++;
                spawnpid = waitpid(spawnpid, &exitStatus, WNOHANG);
            }
            break;
    }
    return;

}

void fg_bg_signal(int s){
    //toggles foreground only mode on or off
    if(fg_only == 0){
        fputs("\nEntering foreground only mode!\n", stdout);
        fg_only = 1;
        fflush(stdin);
    }else{
        fputs("\nRe-enabling background processes!\n", stdout);
        fg_only = 0;
        fflush(stdin);
    }

}

void int_signal(int s){
    //terminates the currently running child process. Is only enabled in child processes after they fork off
    fputs("Ending process %d\n", getpid());
    sig_kill = 1;
    exit(2);
    return; 
    }

void child_signal(int s){
    //unused signal handler for the termination of child processes
    printf("pid of terminated child process == %d. ", child_processes[process_no]);
    if(sig_kill == 1){
        fputs("Process terminated by signal. ", stdout);
    }else{
        fputs("Process terminated normally. ", stdout);    
    }

    printf("Exit status: %d\n", exitStatus);
}