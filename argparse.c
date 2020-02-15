//
// Created by swanbej3 on 2/1/20.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "argparse.h"

static int numElements(char* line){
    enum {BET, ARG, QUOTE} state = BET;
    int count = 0;
    while(*line != '\0'){
        if(!isspace(*line)){
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

        ++line;
    }


    if(state == QUOTE){
        fprintf(stderr, "Unclosed quotation marks");
    }
    return count;
}

char** argparse(char* line, int* argcp){
    int elements = numElements(line);
    *argcp = elements;
    int count = 0;

    char** args = (char**)malloc((elements + 1)*sizeof(char*));
    enum {BET, ARG, QUOTE} state = BET;


    while(*line != '\0'){
        if(!isspace(*line)){
            if(state == BET){
                if(*line != '\"'){
                    args[count] = line;
                    ++count;
                }
                state = ARG;
            }
            if(state == ARG){
                if(*line == '\"'){
                    if(line[1] == '\"'){
                        args[count] = NULL;
                    }
                    else{
                        ++line;
                        args[count] = line;

                    }
                    ++count;
                    state = QUOTE;
                }
            }
            else{
                if(*line == '\"'){
                    *line = '\0';
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

    return args;
}
