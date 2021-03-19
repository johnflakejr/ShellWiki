#include <stdio.h> 
#include <curl/curl.h>
#include <stdlib.h> 
#include <string.h> 
#include "parser.h"

#define PAGE_SIZE 4096 
#define REQ_SIZE 256
#define DEBUG 1

/*
  Prints how to use this simple program
*/
void usage()
{
  printf("Usage: \n\n"); 
  printf("./shellwiki [thing] \n"); 
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

    strncpy(page + (total_len - cur_len), argv[i], cur_len); 

    if (i < (argc - 1))
    {
      strncpy(page + (total_len), "_", sizeof(char)); 
      total_len++; 
    }

  }
  page[total_len] = '\0'; 
  return page; 
}

/**
 * @brief Handle data from the web server.
 * @param ptr string of raw data received
 */
static size_t handle_resp(char *ptr, size_t size, size_t nmemb, void *userdata)
{

  //Get the response without HTML data. 
  char * resp = remove_html_metadata(ptr); 

#ifdef DEBUG
  //printf("GOT %lu 'size'. \n\n\n", size); 
  //printf("GOT %lu nmemb. \n\n\n", nmemb); 
  printf("%s\n", resp); 
  //printf("GOT USER DATA: %s\n", userdata); 
  free(resp); 
#endif

  return (size * nmemb);  
}

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
 
  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();

  if (NULL == curl) 
  {
    return 1; 
  }

  char * request_url = calloc(REQ_SIZE, sizeof(char)); 

  if (NULL == request_url)
  {
    return 1; 
  }

  //TODO: Account for language. 
  char * base_wiki_url = "https://en.wikipedia.org/wiki/"; 
  strncpy(request_url, base_wiki_url, strlen(base_wiki_url)); 

  char * wiki_page = combine_args_to_page(argc, argv);
  strncat(request_url, wiki_page, strlen(wiki_page)); 

#ifdef DEBUG 
  printf("Request URL: %s\n",request_url); 
#endif
    
  curl_easy_setopt(curl, CURLOPT_URL, request_url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_resp);

  res = curl_easy_perform(curl);

  if (CURLE_OK != res)
  {
    fprintf(stderr, "err: %s\n", curl_easy_strerror(res));
  }

  curl_easy_cleanup(curl);

  return 0;
}

//End of file

