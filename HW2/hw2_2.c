#include <stdio.h>
#include <string.h>

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
    char command[MAX_CMD];
    command[0] = command[MAX_CMD-1] = 0; // for safety
    int argc = 0;
    char *argv[MAX_ARG] = { NULL };

    while(1){
        printf("$ ");
        fgets(command, MAX_CMD - 1, stdin);
        command[strlen(command)-1] = 0; // trim \r
        if(strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0)
            break;
    
        ParseCommand(command, &argc, argv);
        printf("argc = %d\n", argc);

        for(int i = 0; i < argc; i++)
            printf("argv[%d] = %s\n", i, argv[i]);
        
        printf("argv[%d] = %p\n", argc, argv[argc]); // argv[argc] must be NULL
    }
    printf("Bye!\n");
    return 0;
}