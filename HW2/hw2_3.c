#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CMD 2048
#define MAX_ARG 256

void ParseCommand(char *command, int *argc, char *argv[]){
    int i = 0;
    char *result;
    char *double_quo = strstr(command, "\"");
    char *single_quo = strstr(command, "\'");

    if(double_quo != NULL || single_quo != NULL){
        if(double_quo != NULL)
        {
            result = strtok(command, "\"");

            while(result != NULL){
                argv[i] = result;
                result = strtok(NULL, "\"");
                i++;
            }
            *argc = i;
            argv[i] = NULL;
        }else if(single_quo != NULL)
        {
            result = strtok(command, "\'");
            
            while(result != NULL){
                argv[i] = result;
                result = strtok(NULL, "\'");
                i++;
            }
            *argc = i;
            argv[i] = NULL;
        }
    }else{
        result = strtok(command, " ");
            
        while(result != NULL){
            argv[i] = result;
            result = strtok(NULL, " ");
            i++;
        }
        *argc = i;
        argv[i] = NULL;
    }
}

int main(){
    printf("Welcome to Mini Shell!\n");
    char command[MAX_CMD];
    command[0] = command[MAX_CMD-1] = 0; // for safety
    int argc = 0;
    char *argv[MAX_ARG] = { NULL };
    int child_pid, exitcode;

    while(1){
        printf("$ ");
        fgets(command, MAX_CMD - 1, stdin);
        command[strlen(command)-1] = 0; // trim \r
        if(strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0){
            printf("Bye!\n");
			exit(0);
        }
		ParseCommand(command, &argc, argv);

        child_pid = fork();
        
        if(child_pid < 0){
            fprintf(stderr, "fork faild\n");
            exit(-1);
        }else if(child_pid == 0){
            execvp(argv[0], argv);
        }else{
            wait(&exitcode);
        }
    }

    return 0;
}
