//
// Created by swanbej3 on 2/1/20.
//
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"

static int numElements(char* line){
    enum {BET, ARG} state = BET;
    int count = 0;

    while(*line != '\0'){
        if(!isspace(*line)){
            if(state == BET){
                ++count;
            }
            state = ARG;
        }
        else {
            state = BET;
        }
        ++line;
    }
    return count;
}

char** argparse(char* line, int* argcp){
    int elements = numElements(line);
    *argcp = elements;
    int count = 0;

    char** args = (char**)malloc((elements + 1)*sizeof(char*));
    enum {BET, ARG} state = BET;

    while(*line != '\0') {

        if(!isspace(*line)) {
            if(state == BET){
                args[count] = line;
                ++count;
            }
            state = ARG;
        }
        else {
            if(state == ARG){
                *line = '\0';
            }
            state = BET;
        }
        ++line;
    }

    return args;
}
