#include <stdio.h> 
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include "../include/parser.h"
#include "../include/util.h"

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
  char * disam_choice; 

} reqdata; 

/**
 * @brief Handle data from the web server.
 * @param ptr string of raw data received
 */
static size_t handle_resp(char *ptr, size_t size, size_t nmemb, void * userdata)
{

  if (GET_CONTENT == ((reqdata *) userdata)->req_type)
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
  }
  else
  {
    char * req = parse_disambiguation(ptr); 
    ((reqdata *) userdata)->disam_choice = req;
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

  //Disambiguation
  else
  {
    com = "?action=query&prop=links&pllimit=15&titles=";
  }

  strncpy(request_url, base_wiki_url, strlen(base_wiki_url)); 
  strncat(request_url, com, strlen(com)); 

  //Strip quotes from wiki_page: 
  
  wiki_page = remove_quotes(wiki_page); 
  wiki_page = replace_spaces(wiki_page); 
  strncat(request_url, wiki_page, strlen(wiki_page)); 
  strncat(request_url, flags, strlen(flags));

  //printf("Request URL: %s\n",request_url); 
  curl_easy_setopt(curl, CURLOPT_URL, request_url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) request_data);

#ifdef DEBUG
  printf("Request URL: %s\n",request_url); 
#endif

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_resp);

  res = curl_easy_perform(curl);

  if (CURLE_OK != res)
  {
    fprintf(stderr, "err: %s\n", curl_easy_strerror(res));
  }

  curl_easy_cleanup(curl);
  free(request_url); 
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
  request.is_disam = false;
  request.disam_choice = NULL;
  request.req_type = GET_CONTENT; 

  make_request(wiki_page, &request); 

  //If that didn't work because of disambiguation, handle that: 
  if (request.is_disam)
  {
    request.req_type = GET_DISAMBIGUATION;
    make_request(wiki_page, &request); 
  }

  if (NULL != request.disam_choice)
  {
    request.req_type = GET_CONTENT;
    make_request(request.disam_choice, &request); 
  }
  
  return 0;
}

//End of file
