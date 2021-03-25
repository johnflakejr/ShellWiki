#include "../include/parser.h" 
#include <stdio.h> 
#include <stdbool.h> 
#include <cjson/cJSON.h>

char * replace_spaces(char * input)
{
  if (NULL == input)
  {
    return NULL;
  }
  
  size_t len = strlen(input); 
  char * src = input;
  char * dst = input;

  while ('\0' != *src)
  {
    *dst = *src;

    if (' ' == *dst)
    {
      *dst = '_';
    }

    dst++;
    src++;
  }

  *dst = '\0';
  return input;
}

char * remove_quotes(char * input)
{
  if (NULL == input)
  {
    return NULL;
  }
  
  size_t len = strlen(input); 
  char * src = input;
  char * dst = input;

  while ('\0' != *src)
  {
    *dst = *src;

    if ('"' != *dst)
    {
      dst++;
    }

    src++;
  }

  *dst = '\0';
  return input;
}

char * parse_disambiguation(char * input)
{
  char * page_to_request = NULL;

  cJSON * resp = cJSON_Parse(input); 
  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  cJSON * links = NULL;  
  cJSON * page = cJSON_GetArrayItem(pages, 0);

  links = cJSON_GetObjectItemCaseSensitive(page, "links"); 

  int size = cJSON_GetArraySize(links); 

  for(int i = 0; i < size; i++)
  {
    cJSON * link = cJSON_GetArrayItem(links, i);
    cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
    printf("%d: %s\n", i, cJSON_Print(text)); 
  }

   
  int choice = 0;
  printf("Enter the number for the intended article: ");
  scanf("%d", &choice); 
  printf("Getting %d\n",choice); 
  
  if (choice >= 0 && (choice < size))
  {
    cJSON * link = cJSON_GetArrayItem(links, choice);
    cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
    page_to_request = cJSON_Print(text); 
  }
  else
  {
    printf("Invalid choice.\n"); 
  }

  return page_to_request;
}




char * parse_content_from_json(char * input)
{
  cJSON * resp = cJSON_Parse(input); 
  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  cJSON * extract = NULL;  
  cJSON * page = cJSON_GetArrayItem(pages, 0);
  extract = cJSON_GetObjectItemCaseSensitive(page, "extract"); 
  return remove_html_metadata(cJSON_Print(extract));
}



/**
 * @brief Remove HTML characters <.*> from the file. 
 * @param input the input string with HTML characters in it. 
 * @return string without HTML metadata. 
 */
char * remove_html_metadata(char * input)
{
  if (NULL == input)
  {
    return NULL;
  }

  char * output = calloc(strlen(input), sizeof(char));

  if (NULL == output)
  {
    return NULL; 
  }

  int iter = 0; 
  int outindex = 0; 
  bool b_is_meta = false; 

  //TODO: better implement to avoid false positives
  //Skip over instances of <.*>
  for (iter = 0; iter < strlen(input); iter++)
  {

    //Opening tag -> start metadata
    if ('<' == input[iter])
    {
      b_is_meta = true; 
      continue; 
    }
    
    //Closing tag -> done with metadata
    if ('>' == input[iter])
    {
      b_is_meta = false;
      continue; 
    }

    if (!b_is_meta)
    {
      output[outindex] = input[iter]; 
      outindex++;
    }
    else
    {
      //HTML tags...
    }
  }

  return output; 
}

//End of file

