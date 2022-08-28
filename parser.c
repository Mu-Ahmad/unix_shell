#include "headers/parser.h"


char** parse_atomic_command(char* string, int* count, char* seperators)
{

  int len = strlen(string);
  
  *count = 0;
  
  int i = 0;
  while (i < len)
  {
    while (i < len)
    {
      if (strchr(seperators, string[i]) == NULL)
        break;
      i++;
    }
    
    int old_i = i;
    while (i < len)
    {
      if (strchr(seperators, string[i]) != NULL)
        break;
      i++;
    }

    if (i > old_i) *count = *count + 1;
  }
  
  char **strings = malloc(sizeof(char *) * (*count + 1));
  
  i = 0;

  char buffer[16384];
  int string_index = 0;
  while (i < len)
  {
    while (i < len)
    {
      if (strchr(seperators, string[i]) == NULL)
        break;
      i++;
    }
    
    int j = 0;
    while (i < len)
    {
      if (strchr(seperators, string[i]) != NULL)
        break;
      
      buffer[j] = string[i];
      i++;
      j++;
    }
    
    if (j > 0)
    {
      buffer[j] = '\0';

      int to_allocate = sizeof(char) *
                        (strlen(buffer) + 1);
      
      strings[string_index] = malloc(to_allocate);
      
      strcpy(strings[string_index], buffer);
      
      string_index++;
    }
  }

  strings[string_index] = NULL;
  return strings;
}
