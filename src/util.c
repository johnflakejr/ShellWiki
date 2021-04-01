#include "../include/util.h" 
#include <ctype.h> 
#include <stdio.h>
#include <unistd.h> 

/*
  Prints how to use this simple program
*/
void usage()
{
  printf("shellwiki: read brief entries from Wikipedia from your terminal!\n");
  printf("Usage: \n\n"); 
  printf("./shellwiki [-l] [article] \n\n\n"); 
  printf("-l: \"I'm feeling lucky\" - get first result.\n");
  printf("-v: verbose\n\n");
}

/**
 * @brief parse command line options
 * @param argc Arg count
 * @param argv Arg array
 * @param my_options pointer to options struct
 * @return 0 on success
 */
int get_options(int argc, char ** argv, options * my_options)
{
  int opt = -1;

  while (-1 != (opt = getopt(argc, argv, "lv")))
  {
    switch (opt)
    {
      case 'l':
        my_options->lucky = true;
      break;

      case 'v':
        my_options->verbose = 1;
      break;

      default: 
        return 1;
      break;
    }
  }

  //Get the desired page from the user: 
  //optind is the index given after regular args
  //printf("Optind: %d\n",optind);

  if (optind >= argc)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief Given a string, capitalize its first letter.
 * @param input input string
 * @return char * pointer to the modified string
 */
char * capitalize_arg(char * input)
{
  if (NULL == input)
  {
    return NULL; 
  }

  input[0] = toupper(input[0]);

  return input;
}

/**
 * @brief Given the arguments, parse and return the_page_name
 * @param argc Argument Count
 * @param argv Array of arguments
 * @return char * string for the_combined_page_name
 */
char * combine_args_to_page(int argc, char ** argv)
{

  //Initialize length to the first arg.  Reallocate as needed. 
  size_t total_len = 0;
  char * page = NULL; 
  int    i; 

  //Combine arguments
  for (i = 1; i < argc; i++)
  {
    size_t cur_len = strlen(argv[i]); 
    total_len += cur_len; 

    page = realloc(page, total_len + 1); 
    if (NULL == page)
    {
      return NULL; 
    }

    
    strncpy(page + (total_len - cur_len), capitalize_arg(argv[i]), cur_len); 

    if (i < (argc - 1))
    {
      strncpy(page + (total_len), "_", sizeof(char)); 
      total_len++; 
    }

  }
  page[total_len] = '\0'; 
  return page; 
}

//End of file

