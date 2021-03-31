#include "../include/communication.h"
#include "../include/parser.h"
#include "../include/util.h"
#include <curl/curl.h>
#define DEBUG 1



/**
 * @brief Handle data from the web server.
 * @param ptr string of raw data received
 * @param size number of pages to parse
 * @param size of the page
 * @param userdata pointer to data provided by the user
 */
size_t handle_resp (char *ptr, size_t size, size_t nmemb, void * userdata)
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
      if (NULL != strstr(content, "may refer to"))
      {
        ((reqdata *) userdata)->res_type = GET_DISAMBIGUATION;
        printf("There are multiple pages.  Try one of these:\n\n"); 
        return 0;
      }
      else
      {
        //This is where the actual information is printed to the user.
        ((reqdata *) userdata)->res_type = RESP_OK;
        printf("%s\n",content); 
      }
    }
    else
    {
      //This is where the actual information is printed to the user.
      ((reqdata *) userdata)->res_type = RESP_OK;
      printf("This article could not be displayed.\n");
    }
  }

  else if (GET_SEARCH == ((reqdata *) userdata)->req_type)
  {
    char * req = parse_search(ptr, *((reqdata *) userdata));
    ((reqdata *) userdata)->next_choice = req;
    ((reqdata *) userdata)->res_type = GET_CONTENT;
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
  char * request_url = calloc(REQ_SIZE, sizeof(char)); 
  CURL * curl = curl_easy_init();

  if ((NULL == curl) || (NULL == request_url)) 
  {
    return;
  }


  //TODO: Add options for some of these flags, perhaps.
  char * base_wiki_url = "https://en.wikipedia.org/w/api.php";
  char * com;
  char * flags = "&formatversion=2&format=json"; 

  //Content query command
  if (GET_CONTENT == request_data->req_type)
  {
    com = "?action=query&prop=extracts&exsentences=10&exlimit=5&titles=";
  }

  //Search query command
  //50 total results.  User will then parse through them page by page
  else if (GET_SEARCH == request_data->req_type)
  {
    com = "?action=query&list=search&srlimit=20&srsearch=";
  }

  //Disambiguation query command
  else
  {
    printf("Disambiguation...\n"); 
    com = "?action=query&prop=links&pllimit=10&titles=";
  }

  strncpy(request_url, base_wiki_url, strlen(base_wiki_url)); 
  strncat(request_url, com, strlen(com)); 

  //Strip quotes from search term and replace spaces with "_"
  wiki_page = remove_char(wiki_page, '"'); 
  wiki_page = replace_spaces(wiki_page); 

  strncat(request_url, wiki_page, REQ_SIZE - strlen(request_url)); 
  strncat(request_url, flags, REQ_SIZE - strlen(request_url));

  curl_easy_setopt(curl, CURLOPT_URL, request_url);

  //Follow redirects
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  //Data to pass to the handler function
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) request_data);

  if (request_data->verbose)
  {
    printf("Requesting API URL: %s\n",request_url); 
  }

  //Function for handling response
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_resp);

  //Perform the actual request
  CURLcode res = curl_easy_perform(curl);

  if (CURLE_OK != res)
  {
    fprintf(stderr, "err: %s\n", curl_easy_strerror(res));
  }

  curl_easy_cleanup(curl);
  free(request_url); 
}

//End of file

