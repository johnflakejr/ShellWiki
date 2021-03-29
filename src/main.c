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
  
  bool lucky = false;
  int opt;

  while (-1 != (opt = getopt(argc, argv, "l")))
  {

    switch (opt)
    {
      case 'l':
        lucky = true;
      break;

      default: 
        usage();
        return 1;
      break;
    }
  }

  //Get the desired page from the user: 
  //optind is the index given after regular args
  printf("Optind: %d\n",optind);

  if (optind >= argc)
  {
    usage();
    return 1;
  }

  char * wiki_page;
  wiki_page = combine_args_to_page(argc - (optind - 1), argv + (optind - 1));

  //First, search for the page
  reqdata request;
  request.next_choice = NULL;
  request.req_type = GET_SEARCH;
  request.res_type = RESP_OK;
  request.is_lucky = lucky;

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

