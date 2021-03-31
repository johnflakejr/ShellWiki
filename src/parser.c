#include "../include/parser.h" 
#include <stdio.h> 
#include <stdbool.h> 
#include <cjson/cJSON.h>
#include "../include/communication.h"

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
    cJSON_Delete(resp);
    return NULL;
  }

  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  if (NULL == pages)
  {
    cJSON_Delete(resp);
    return NULL;
  }


  cJSON * page = cJSON_GetArrayItem(pages, 0);
  if (NULL == page)
  {
    cJSON_Delete(resp);
    return NULL;
  }



  links = cJSON_GetObjectItemCaseSensitive(page, "links"); 

  int size = cJSON_GetArraySize(links); 

  if (0 == size)
  {
    cJSON_Delete(resp);
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

  cJSON_Delete(resp);
  return page_to_request;
}


/**
 * @brief Lets the user choose a page from a cJSON array of pages
 * 
 */
char * get_choice_from_list (cJSON * pages)
{
  bool   choice_made = false;
  int    page = 0;
  char   choice = 0;
  int    int_choice = 0;
  char * str_choice = NULL;
  int    size = cJSON_GetArraySize(pages); 

  while (!choice_made)
  {

    for (int i = 0; i < 5; i++)
    {
      if ((i + (page * 5)) < size)
      {
        cJSON * link = cJSON_GetArrayItem(pages, i + (page * 5));
        cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
        printf("%d: %s\n", i, cJSON_Print(text)); 
      }
    }

    printf("Page %d\n", page);
    printf("Enter w for back and d for next page.\t\t\n");
    printf("Enter the number for the intended artice: ");

    scanf("%c",&choice);
    getchar();

    if ('d' == choice)
    {
      page++;

      if ((page * 5) >= size)
      {
        page--;
      }
      continue;
    }

    if ('w' == choice)
    {
      page--;
      if (page < 0)
      {
        page = 0;
      }
      continue;
    }

    int_choice = choice - '0';
    printf("Getting %d\n", int_choice); 
    
    if ((int_choice >= 0) && (int_choice < 5))
    {
      cJSON * link = cJSON_GetArrayItem(pages, int_choice + (page * 5));
      cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
      str_choice = cJSON_Print(text); 
      choice_made = true;
    }
    else
    {
      printf("Invalid choice.\n"); 
    }
  }

  return str_choice; 
}

/**
 *  @brief Given a JSON response from the server, parse it.
 */
char * parse_search (char * input, reqdata userdata)
{
  char * page_to_request = NULL;

  cJSON * resp = cJSON_Parse(input); 
  if (NULL == resp)
  {
    return NULL;
  }

  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  if (NULL == query)
  {
    cJSON_Delete(resp);
    return NULL;
  }

  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "search"); 
  if (NULL == pages)
  {
    cJSON_Delete(resp);
    return NULL;
  }

  int size = cJSON_GetArraySize(pages); 
  printf("%d results.\n", size); 

  if (0 == size)
  {
    cJSON_Delete(resp);
    return NULL;
  }

  if (userdata.is_lucky)
  {
    cJSON * link = cJSON_GetArrayItem(pages, 0);
    if (NULL == link)
    {
      cJSON_Delete(resp);
      return NULL;
    }

    cJSON * text = cJSON_GetObjectItemCaseSensitive(link, "title"); 
    if (NULL == text)
    {
      cJSON_Delete(resp);
      return NULL;
    }

    page_to_request = cJSON_Print(text); 
    if (NULL == page_to_request)
    {
      cJSON_Delete(resp);
      return NULL;
    }

    if (userdata.verbose)
    {
      printf("\"I'm feeling lucky\" option returned %s.\n", page_to_request);
    }
  }
  else
  {
    page_to_request = get_choice_from_list(pages);
  }

  cJSON_Delete(resp);
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
  cJSON_Delete(resp);
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
  }

  return output; 
}

//End of file

