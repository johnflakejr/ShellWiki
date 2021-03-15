#include "parser.h" 

char * remove_html_metadata(char * input)
{
  char * output = calloc(strlen(input), sizeof(char));
  if (NULL == output)
  {
    return NULL; 
  }

  int iter = 0; 
  int outindex = 0; 
  int meta = 0; 

  //Skip over instances of <.*>
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

//End of file

