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
#define GET_REDIRECTS 0x00
#define GET_CONTENT 0x01
#define GET_DISAMBIGUATION 0x02
#define GET_SEARCH 0x04
#define RESP_OK 0xFF

typedef struct reqdata {

  //Lets the underlying function know what API request to make
  int req_type;

  //Lets the user know what to do next
  int res_type;

  //Response makes it clear that this is a disambiguation
  char * next_choice; 

} reqdata; 


/**
 * @brief Handle data from the web server.
 * @param ptr string of raw data received
 * @param size number of pages to parse
 * @param size of the page
 * @param userdata pointer to data provided by the user
 */
static size_t handle_resp (char *ptr, size_t size, size_t nmemb, void * userdata)
{
  if ((NULL == ptr) || (NULL == userdata))
  {
    return 0; 
  }

  if (GET_CONTENT == ((reqdata *) userdata)->req_type)
  {
    char * content = parse_content_from_json(ptr); 

    if (NULL != content)
    {

      size_t len = strlen(content);
    
      if (len < 10)
        printf("Not much here...\n"); 
      /*
      {
        printf("Getting redirects...\n"); 
        ((reqdata *) userdata)->res_type = GET_REDIRECTS;
        return 0; 
      }
      */

      if (NULL != strstr(content, "may refer to"))
      {
        ((reqdata *) userdata)->res_type = GET_DISAMBIGUATION;
        printf("There are multiple pages.  Try one of these...\n"); 
        return 0; 
      }
      else
      {
        ((reqdata *) userdata)->res_type = RESP_OK;
        printf("%s\n",content); 
      }
    }
  }
  else if (GET_SEARCH == ((reqdata *) userdata)->req_type)
  {
    char * req = parse_search(ptr); 
    ((reqdata *) userdata)->next_choice = req;
  }

  //Disambiguation
  else
  {
    char * req = parse_disambiguation(ptr); 
    ((reqdata *) userdata)->next_choice = req;
  }

  return (size * nmemb);  
}


/**
 * @brief function to actually request the given page
 * @param wiki_page wiki page string to request
 * @param request_data data provided by the requester to this function
 */
void make_request(char * wiki_page, reqdata * request_data)
{
  CURL * curl;
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


  //Extract data from this page. 
  if (GET_CONTENT == request_data->req_type)
  {
    com = "?action=query&prop=extracts&exsentences=10&exlimit=5&titles=";
  }

  //Find out if this page has other information
  else if (GET_SEARCH == request_data->req_type)
  {
    com = "?action=query&list=search&srsearch=";
  }

  //Disambiguation
  else
  {
    com = "?action=query&prop=links&pllimit=20&titles=";
  }

  strncpy(request_url, base_wiki_url, strlen(base_wiki_url)); 
  strncat(request_url, com, strlen(com)); 

  //Strip quotes from wiki_page: 
  wiki_page = remove_char(wiki_page, '"'); 
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

  CURLcode res = curl_easy_perform(curl);

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

  //Get the desired page from the user: 
  char * wiki_page = combine_args_to_page(argc, argv);

  //First, try making a request for the given page.
  reqdata request;
  request.next_choice = NULL;
  request.req_type = GET_SEARCH;


  printf("Making initial page search...\n"); 

  make_request(wiki_page, &request); 

  //printf("Dealing with response type: %d...\n", request.res_type); 
  //If that didn't work because of disambiguation, handle that: 
  if (GET_DISAMBIGUATION == request.res_type)
  {
    request.req_type = GET_DISAMBIGUATION;
    make_request(wiki_page, &request); 
  }

  if (NULL != request.next_choice)
  {
    request.req_type = GET_CONTENT;
    make_request(capitalize_arg(request.next_choice), &request); 
  }
  
  return 0;
}

//End of file

