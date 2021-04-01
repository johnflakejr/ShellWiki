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
int parse_resp (char * input, reqdata_t * request_data)
{

  if ((NULL == input) || (NULL == request_data))
  {
    return 1;
  }

  if (GET_CONTENT == request_data->req_type)
  {
    char * content = parse_content_from_json(input); 

    if (NULL != content)
    {
      if (NULL != strstr(content, "may refer to"))
      {
        request_data->res_type = GET_DISAMBIGUATION;
        printf("There are multiple pages.  Try one of these:\n\n"); 
        return 0;
      }
      else
      {
        //This is where the actual information is printed to the user.
        request_data->res_type = RESP_OK;
        printf("%s\n",content); 
      }
    }
    else
    {
      //This is where the actual information is printed to the user.
      request_data->res_type = RESP_OK;
      printf("This article could not be displayed.\n");
    }
  }

  else if (GET_SEARCH == request_data->req_type)
  {
    char * req = parse_search(input, *request_data);
    request_data->next_choice = req;
    request_data->res_type = GET_CONTENT;
  }

  //Disambiguation
  else
  {
    char * req = parse_disambiguation(input, *request_data);
    request_data->next_choice = req;
  }

  return 0;
}


/*
TODO: change handle resp to only deal with getting entire content of response.

      After getting the content, we can then parse all of it.
  
      Currently, there's an issue with incompletely parsing long messages
*/
size_t handle_resp (char *ptr, size_t size, size_t nmemb, void * userdata)
{
  //Userdata is a struct with a size and a character array
  response_data_t * resp = (response_data_t *) userdata; 

  int offset = resp->length;
  resp->length = resp->length + nmemb;

  resp->data = realloc(resp->data, resp->length);

  if (NULL == resp->data)
  {
    return 0;
  }
  strncpy(resp->data + offset, ptr, nmemb);
  return (size * nmemb);
}

/**
 * @brief function to actually request the given page
 * @param wiki_page wiki page string to request
 * @param request_data data provided by the requester to this function
 */
void make_request(char * wiki_page, reqdata_t * request_data)
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

  if (request_data->verbose)
  {
    printf("Performing request now\n");
  }

  //Content query command
  if (GET_CONTENT == request_data->req_type)
  {
    com = "?action=query&prop=extracts&exsentences=10&exlimit=5&titles=";
  }

  //Search query command
  else if (GET_SEARCH == request_data->req_type)
  {
    com = "?action=query&list=search&srlimit=100&srsearch=";
  }

  //Disambiguation query command
  else
  {
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

  response_data_t resp;
  resp.length = 0;
  resp.data = NULL;

  //Data to pass to the handler function
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &resp);

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

  //Parse response: 
  parse_resp(resp.data, request_data);

  curl_easy_cleanup(curl);
  free(request_url); 
}

//End of file

