#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h> 
#include <string.h> 

char * parse_disambiguation(char * input);
char * parse_content_from_json(char * input);
char * remove_html_metadata(char * input);
char * remove_quotes(char * input);
char * replace_spaces(char * input);

#endif

//End of file

