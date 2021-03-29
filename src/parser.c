#include "../include/parser.h" 
#include <stdio.h> 
#include <stdbool.h> 
#include <cjson/cJSON.h>

/**
 * @brief removes the two character '\' 'n' sequence from input
 * @param input string to remove "\n" from
 * @return char* pointer to the altered string
 */
static char * remove_newlines(char * input)
{
  if (NULL == input)
  {
    return NULL;
  }
  
  char * src = input;
  char * dst = input;

  while ('\0' != *src)
  {
    *dst = *src;

    if (('n' == *dst) && ('\\' == *(dst-1)) )
    {
      dst -= 2;
    }

    dst++;
    src++;
  }

  *dst = '\0';

  return input;
}

/**
 * @brief replace all spaces with underscores in the input string
 * @param input the string to alter
 * @return char* pointer to the altered string
 */
char * replace_spaces(char * input)
{
  if (NULL == input)
  {
    return NULL;
  }
  
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

/**
 * @brief remove the specified character from the input string
 * @param input the string to alter
 * @param c the character to remove
 * @return the altered string
 */
char * remove_char(char * input, char c)
{
  if (NULL == input)
  {
    return NULL;
  }
  
  char * src = input;
  char * dst = input;

  while ('\0' != *src)
  {
    *dst = *src;

    if (c != *dst)
    {
      dst++;
    }

    src++;
  }

  *dst = '\0';
  return input;
}


/**
 *  @brief Given a JSON response from the server, parse it.
 *
 */

char * parse_disambiguation(char * input)
{
  char * page_to_request = NULL;
  cJSON * links = NULL;  

  cJSON * resp = cJSON_Parse(input); 
  if (NULL == resp)
  {
    return NULL;
  }

  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  if (NULL == query)
  {
    return NULL;
  }

  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  if (NULL == pages)
  {
    return NULL;
  }


  cJSON * page = cJSON_GetArrayItem(pages, 0);
  if (NULL == page)
  {
    return NULL;
  }



  links = cJSON_GetObjectItemCaseSensitive(page, "links"); 

  int size = cJSON_GetArraySize(links); 

  if (0 == size)
  {
    return NULL;
  }

  for (int i = 0; i < size; i++)
  {
    cJSON * link = cJSON_GetArrayItem(links, i);
    cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
    printf("%d: %s\n", i, cJSON_Print(text)); 
  }

   
  int choice = 0;
  printf("Enter the number for the intended article: ");
  scanf("%d", &choice); 
  
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

char * parse_search(char * input, bool is_lucky)
{
  char * page_to_request = NULL;

  cJSON * resp = cJSON_Parse(input); 
  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "search"); 
  cJSON * page = NULL;

  int size = cJSON_GetArraySize(pages); 

  printf("%d results.\n", size); 

  if (0 == size)
  {
    return NULL;
  }

  

  int choice = 0;

  if (is_lucky)
  {
    cJSON * link = cJSON_GetArrayItem(pages, choice);
    cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
    page_to_request = cJSON_Print(text); 
  }
  else
  {

    for (int i = 0; i < size; i++)
    {
      cJSON * link = cJSON_GetArrayItem(pages, i);
      cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
      printf("%d: %s\n", i, cJSON_Print(text)); 
    }
    printf("Enter the number for the intended article: ");

    scanf("%d", &choice); 
    printf("Getting %d\n",choice); 
    
    if ((choice >= 0) && (choice < size))
    {
      cJSON * link = cJSON_GetArrayItem(pages, choice);
      cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
      page_to_request = cJSON_Print(text); 
    }
    else
    {
      printf("Invalid choice.\n"); 
    }
  }

  return page_to_request;
}

/**
 * @brief get the text extract from the JSON reply
 */
char * parse_content_from_json(char * input)
{
  cJSON * resp = cJSON_Parse(input); 
  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  cJSON * extract = NULL;  
  cJSON * page = cJSON_GetArrayItem(pages, 0);
  extract = cJSON_GetObjectItemCaseSensitive(page, "extract"); 
  char * res = remove_html_metadata(cJSON_Print(extract));

  res = remove_newlines(res);
  res = remove_char(res, '\\');
  return res;
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

  int  outindex  = 0; 
  bool b_is_meta = false; 

  //Skip over instances of <.*>
  for (int iter = 0; iter < strlen(input); iter++)
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

