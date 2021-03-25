#include "../include/parser.h" 
#include <stdio.h> 
#include <stdbool.h> 
#include <cjson/cJSON.h>

char * parse_content_from_json(char * input)
{
  cJSON * resp = cJSON_Parse(input); 
  //printf("%s\n", cJSON_Print(resp));
  cJSON * query = cJSON_GetObjectItemCaseSensitive(resp, "query"); 
  //printf("%s\n", cJSON_Print(query));
  cJSON * pages = cJSON_GetObjectItemCaseSensitive(query, "pages"); 
  //printf("%s\n", cJSON_Print(pages));

  //TODO: account for disambiguation pages. 
  cJSON * page = cJSON_GetArrayItem(pages, 0);
  //printf("%s\n", cJSON_Print(page));
  cJSON * extract = cJSON_GetObjectItemCaseSensitive(page, "extract"); 
  //printf("%s\n", cJSON_Print(extract));
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

