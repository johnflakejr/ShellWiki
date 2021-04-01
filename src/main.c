#include <stdio.h> 
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <unistd.h>
#include "../include/parser.h"
#include "../include/communication.h"
#include "../include/util.h"
#define DEBUG 1

int main(int argc, char ** argv)
{
  /*
    The user needs to supply a search term
  */
  if (2 > argc)
  {
    usage(); 
    return 1; 
  }
  
  int     opt;
  options my_options;

  my_options.lucky = false;
  my_options.verbose = 0;

  if (1 == get_options(argc, argv, &my_options))
  {
    usage();
    return 1;
  }

  char * wiki_page;
  wiki_page = combine_args_to_page(argc - (optind - 1), argv + (optind - 1));

  //First, search for the page
  reqdata request;
  request.next_choice = NULL;
  request.req_type    = GET_SEARCH;
  request.res_type    = RESP_OK;
  request.is_lucky    = my_options.lucky;
  request.verbose     = my_options.verbose;

  if (my_options.verbose)
  {
    if(my_options.lucky)
    {
      printf("Lucky mode is ON\n");
    }
    else
    {
      printf("Lucky mode is OFF\n");
    }

    printf("Searching for the term \"%s\".\n", wiki_page);
  }

  make_request(wiki_page, &request); 
  
  while (RESP_OK != request.res_type)
  {
    //If that didn't work because of disambiguation, handle that: 
    if (GET_DISAMBIGUATION == request.res_type)
    {
      request.req_type = GET_DISAMBIGUATION;
      make_request(wiki_page, &request); 
    }

    if (NULL == request.next_choice)
    {
      break;
    }
    else
    {
      request.req_type = GET_CONTENT;
      make_request(capitalize_arg(request.next_choice), &request); 
    }
  }
  
  free(wiki_page); 
  return 0;
}

//End of file

