#pragma once

#ifndef __PARSER_H__
#define __PARSER_H__

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "constants.h"

char** parse_atomic_command(char *string, int *count, char*);

#endif // __PARSER_H__