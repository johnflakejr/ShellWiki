#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>

typedef struct options {
    bool lucky;
    int verbose;
} options_t;

int get_options (int argc, char ** argv, options_t * my_options);
void usage ();
char * capitalize_arg (char * input);
char * combine_args_to_page (int argc, char ** argv);

#endif

//End of file

