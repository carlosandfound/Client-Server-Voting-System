#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

// function that checks if file doesnt exist or is empty
bool isFilePopulated(char* filepath) {
  FILE* f = fopen(filepath, "r");
  if (!f) {
    printf("Error: The following file does not exist: '%s'\n", filepath);
    return false;
  } else {
    fseek (f, 0, SEEK_END);
    long size = ftell(f);
    if (size == 0) {
        printf("Error: The following file is empty: '%s'\n", filepath);
        fclose(f);
        return false;
    } else {
      fclose(f);
      return true;
    }
  }
}

char *trimwhitespace(char *str) {
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;

  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int makeargv(const char *s, const char *delimiters, char ***argvp) {
  int error;
  int i;
  int numtokens;
  const char *snew;
  char *t;
  char *saveptr;

  if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
    errno = EINVAL;
    return -1;
  }
  *argvp = NULL;
  snew = s + strspn(s, delimiters);
  if ((t = malloc(strlen(snew) + 1)) == NULL)
    return -1;
  strcpy(t,snew);
  numtokens = 0;
  if (strtok_r(t, delimiters,&saveptr) != NULL)
    for (numtokens = 1; strtok_r(NULL, delimiters,&saveptr) != NULL; numtokens++) ;

  if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
    error = errno;
    free(t);
    errno = error;
    return -1;
  }

  if (numtokens == 0)
    free(t);
  else {
    strcpy(t,snew);
    **argvp = strtok_r(t,delimiters,&saveptr);
    for (i=1; i<numtokens; i++)
      *((*argvp) +i) = strtok_r(NULL,delimiters,&saveptr);
  }
  *((*argvp) + numtokens) = NULL;
  return numtokens;
}
