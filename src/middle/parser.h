#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

uint8_t cli_parser(char* line, char* argv[], uint8_t max_args);
void cli_execute(uint8_t argc, char* argv[]);

#endif