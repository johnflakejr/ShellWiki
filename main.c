#include <stdio.h> 
#include <curl/curl.h>
#include <stdlib.h> 
#include <string.h> 

#define PAGE_SIZE 4096 
#define REQ_SIZE 256
#define DEBUG 1

char * remove_html_metadata(char * input)
{
  char * output = calloc(strlen(input), sizeof(char));
  int iter = 0; 
  int outindex = 0; 
  int meta = 0; 
  for (iter = 0; iter < strlen(input); iter++)
  {
    if ('<' == input[iter])
    {
      meta = 1; 
      continue; 
    }
    
    if ('>' == input[iter])
    {
      meta = 0;
      continue; 
    }

    if (!meta)
    {
      output[outindex] = input[iter]; 
      outindex++;
    }
  }
  return output; 
}

/*
  Prints how to use this simple program
*/
void usage()
{
  printf("Usage: \n\n"); 
  printf("./shellwiki [thing] \n"); 
}


/*
  Handle data from the web server: 
*/
static size_t handle_resp(char *ptr, size_t size, size_t nmemb, void *userdata)
{

#ifdef DEBUG
  printf("GOT %lu bytes of data. \n", size); 
  printf("GOT %lu nmemb. \n", nmemb); 
  char * resp = remove_html_metadata(ptr); 
  printf("GOT DATA: %s\n", resp); 
#endif

  return 0; 
}

int main(int argc, char ** argv)
{

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

  char * request_url = calloc (REQ_SIZE, sizeof(char)); 
  if (NULL == request_url)
  {
    return 1; 
  }

  //TODO: use safe string functions lol
  strcpy(request_url, "https://en.wikipedia.org/wiki/"); 

  //Article to get: 
  strcat(request_url, argv[1]); 

#ifdef DEBUG 
  printf("Request URL: %s\n",request_url); 
#endif
    
  curl_easy_setopt(curl, CURLOPT_URL, request_url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  char * response = calloc (PAGE_SIZE, sizeof(char)); 
  if (NULL == response)
  {
    return 1; 
  }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_resp);

  res = curl_easy_perform(curl);

  if (CURLE_OK != res)
  {
    fprintf(stderr, "err: %s\n", curl_easy_strerror(res));
  }

  curl_easy_cleanup(curl);
  free(response); 

  return 0;
}
