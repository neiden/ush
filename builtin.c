
#include "builtin.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int isBuiltin(char** args);


void builtinExit(char* args[], int elements){
    int rtrnVal = 0;
    if(elements > 1){
        rtrnVal = atoi(args[1]);
    }
    exit(rtrnVal);
}

void builtinAecho(char* args[], int elements){
    int nflag = 0;
    if(elements > 1 && strcmp(args[1], "-n") == 0 ){
        nflag = 1;
        ++args;
        ++args;
    }
    else{
        ++args;
    }

    while(*args != NULL){
        dprintf(1, "%s", *args);
        ++args;
        if(*args != NULL){
            dprintf(1, " ");
        }
    }
    if(nflag != 1){
        dprintf(1, "\n");
    }

}

void builtinCD(char* args[], int elements){
  if(elements > 1){
    if(strcmp(args[1], "..") == 0 ){
      chdir("..");
    }
    else{
      chdir(args[1]);
    }
  }
}

void builtinEnvSet(char* args[], int elements){
    if(elements > 2){
        setenv(args[1], args[2], 1);
    }
    else{
        printf("Require a variable name and definition.\n");

    }
}

void builtinUnsetEnv(char* args[], int elements){
    if(elements > 1){
        unsetenv(args[1]);
    }
    else{
        printf("No variable name specified.\n");
    }
}

typedef void (*builtin_t)(char* args[], int elements);
static builtin_t builtins[] = {&builtinExit, &builtinAecho, &builtinCD, &builtinEnvSet, &builtinUnsetEnv, NULL};
static char* builtinNames[] = {"exit", "aecho","cd", "envset", "unsetenv", NULL};


int builtin(char** args, int elements){
    int idx = isBuiltin(args);
    if(idx != -1){
        builtins[idx](args, elements);
    }
    return idx != -1;

}

static int isBuiltin(char** args){
    int i = 0;
    while(builtinNames[i] != NULL &&
    strcmp(builtinNames[i], args[0]) != 0){
        ++i;
    }

    return builtinNames[i] != NULL ? i : -1;
}


