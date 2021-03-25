#include <stdio.h> 
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include "../include/parser.h"

#define PAGE_SIZE 4096 
#define REQ_SIZE 256
#define DEBUG 1

#define GET_CONTENT 0x01
#define GET_DISAMBIGUATION 0x02

typedef struct reqdata {

  //Lets the underlying function know what API request to make
  int req_type;

  //Response makes it clear that this is a disambiguation
  bool is_disam;

} reqdata; 

/**
 * @brief Handle data from the web server.
 * @param ptr string of raw data received
 */
static size_t handle_disam(char *ptr, size_t size, size_t nmemb, void * userdata)
{
  parse_disambiguation(ptr); 

  return (size * nmemb);  
}

/**
 * @brief Handle data from the web server.
 * @param ptr string of raw data received
 */
static size_t handle_resp(char *ptr, size_t size, size_t nmemb, void * userdata)
{
  char * content = parse_content_from_json(ptr); 
  
  if (NULL != content)
  {

    if (NULL != strstr(content, "may refer to"))
    {
      ((reqdata *) userdata)->is_disam = true;
      printf("There are multiple pages.  Try one of these...\n"); 
    }
    else
    {
      ((reqdata *) userdata)->is_disam = false;
      printf("%s\n",content); 
    }
  }

  return (size * nmemb);  
}

void make_request(char * wiki_page, reqdata * request_data)
{
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();

  if (NULL == curl) 
  {
    return;
  }

  char * request_url = calloc(REQ_SIZE, sizeof(char)); 

  if (NULL == request_url)
  {
    return;
  }

  char * base_wiki_url = "https://en.wikipedia.org/w/api.php";
  char * com;
  char * flags = "&formatversion=2&format=json"; 

  if (GET_CONTENT == request_data->req_type)
  {
    com = "?action=query&prop=extracts&exsentences=10&exlimit=5&titles=";
  }
  else
  {
    com = "?action=query&prop=links&titles=";
  }

  strncpy(request_url, base_wiki_url, strlen(base_wiki_url)); 
  strncat(request_url, com, strlen(com)); 
  strncat(request_url, wiki_page, strlen(wiki_page)); 
  strncat(request_url, flags, strlen(flags));

  //printf("Request URL: %s\n",request_url); 
  curl_easy_setopt(curl, CURLOPT_URL, request_url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) request_data);

  if (GET_CONTENT == request_data->req_type)
  {
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_resp);
  }
  else
  {
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_disam);
  }

  res = curl_easy_perform(curl);

  if (CURLE_OK != res)
  {
    fprintf(stderr, "err: %s\n", curl_easy_strerror(res));
  }

  curl_easy_cleanup(curl);
  free(request_url); 
}



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

  char * wiki_page = combine_args_to_page(argc, argv);

  //First, try making a request for the given page.
  reqdata request;
  request.req_type = GET_CONTENT; 
  make_request(wiki_page, &request); 

  //If that didn't work because of disambiguation, handle that: 

  if (request.is_disam)
  {
    request.req_type = GET_DISAMBIGUATION;
    make_request(wiki_page, &request); 
  }


  
  return 0;
}

//End of file

