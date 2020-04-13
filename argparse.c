//
// Created by swanbej3 on 2/1/20.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "argparse.h"
#include <string.h>

static int numElements(char* line){
    enum {BET, ARG, QUOTE, COMMENT} state = BET;
    int count = 0;
    while(*line != '\0'){
        if(!isspace(*line)){
            if(*line == '#' ){
                if(state != QUOTE){
                    state = COMMENT;
                }
            }
            if(state == BET){
                ++count;
                state = ARG;
            }
            if(state == ARG){
                if(*line == '\"'){
                    state = QUOTE;
                }
            }
            else{
                if(*line == '\"'){
                    state = ARG;
                }
            }

        }
        else{
            if(state == ARG){
                state = BET;
            }
        }
        if(state == COMMENT){
            *line = '\0';
        }
        else{
            ++line;
        }

    }


    if(state == QUOTE){
        fprintf(stderr, "Unclosed quotation marks");
    }
    return count;
}

void removeQuotes(char* line) {
    char *new = line;
    if (line != NULL) {
    while (*line != '\0') {
        if (*new != '\"') {
            *line = *new;
            ++line;
        }
        ++new;
    }
    }
}

char** argparse(char* line, int* argcp){
    int elements = numElements(line);
    *argcp = elements;
    int count = 0;

    char** args = malloc((elements+1)*sizeof(char*));
    enum {BET, ARG, QUOTE} state = BET;
    while(*line != '\0'){
        if(!isspace(*line)){

            if(state == BET){
                args[count] = line;
                ++count;
                state = ARG;
            }
            if(state == ARG){
                if(*line == '\"'){
                    state = QUOTE;
                }
            }
            else{
                if(*line == '\"'){
                    state = ARG;
                }
            }
        }
        else{
            if(state == ARG){
                *line = '\0';
                state = BET;
            }
        }

        ++line;

    }
    for(int i = 0; i < elements; i++){
        removeQuotes(args[i]);
    }

    return args;
}

