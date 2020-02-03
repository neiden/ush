
#include "builtin.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int isBuiltin(char** args);
static void execBuiltin(char** args, int builtin, int elements);

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

typedef void (*builtin_t)(char* args[], int elements);
static builtin_t builtins[] = {&builtinExit, &builtinAecho, NULL};
static char* builtinNames[] = {"exit", "aecho"};
static int numBuiltins = sizeof(builtinNames) / sizeof(builtinNames[0]);

int builtin(char** args, int elements){
    int idx = isBuiltin(args);
    if(idx != -1){
        execBuiltin(args, idx, elements);
        return 1;
    }
    else{
        return 0;
    }
}

static int isBuiltin(char** args){
    while(*args != NULL) {
        for (int i = 0; i < numBuiltins; i++) {
            if (strcmp(*args, builtinNames[i]) == 0) {
                return i;
            }
        }
        ++args;
    }
    return -1;
}


static void execBuiltin(char** args, int idx, int elements){
    switch(idx){
        case 0: builtinExit(args, elements);
                break;
        case 1: builtinAecho(args, elements);
                break;
    }
}