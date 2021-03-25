#include "../include/parser.h" 
#include <stdio.h> 
#include <stdbool.h> 
#include <cjson/cJSON.h>

void parse_disambiguation(char * input)
{
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
    printf("%s\n",cJSON_Print(text)); 
  }
}




char * parse_content_from_json(char * input)
{
  cJSON * resp = cJSON_Parse(input); 
  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  cJSON * extract = NULL;  
  cJSON * page = cJSON_GetArrayItem(pages, 0);
  extract = cJSON_GetObjectItemCaseSensitive(page, "extract"); 
  return cJSON_Print(extract);
}



/**
 * @brief Remove HTML characters <.*> from the file. 
 * @param input the input string with HTML characters in it. 
 * @return string without HTML metadata. 
 */
char * remove_html_metadata(char * input)
{
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

