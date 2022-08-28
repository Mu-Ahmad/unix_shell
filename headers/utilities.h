#pragma once

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "constants.h"
#include "parser.h"

typedef struct __internel_func_t
{
    char *function_index;
    int (*function)(int, char **);
} internel_func_t;

// Array of internel commands and their names
extern internel_func_t internel_funcs[];

pid_t Fork();
void printDir();
void unix_error(char*);
int readcommand(char*, char*);
void io_redirect(int, char **);
int Execute(char* , int, char**);
void insert_variable(int, char**);
char* conditional_structure(int *, char**);

#endif // __UTILITIES_H__