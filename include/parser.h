#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include "../include/communication.h"

char * parse_search(char * input, reqdata userdata);
char * parse_disambiguation(char * input);
char * parse_content_from_json(char * input);
char * remove_html_metadata(char * input);
char * remove_char(char * input, char c);
char * replace_spaces(char * input);

#endif

//End of file

