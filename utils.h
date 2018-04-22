#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

char* strpre(char* s, const char* t) {
  size_t len = strlen(t);
  size_t i;
  memmove(s + len, s, strlen(s) + 1);
  for (i = 0; i < len; ++i) {
    s[i] = t[i];
  }
  return s;
}

// create output directory
void rmkdir(const char *dir) {
  char tmp[256];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp),"%s",dir);
  len = strlen(tmp);

  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0;
  }

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  }
  mkdir(tmp, S_IRWXU);
}

// function to decode the given name
// only english alphabet and whitespace characters are considered
char* decode(char* name) {
  char* lowercase_letters[26] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"};
  char* uppercase_letters[26] = {"A","B","C","D","E","F","G","H,","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
  int x = name[0];
  int name_size = strlen(name);
  char* decoded_name = malloc(sizeof(char)*name_size);
  strcpy(decoded_name, "");
  for (int i = 0; i < name_size; i++) {
    int ascii_value = name[i];
    int uppercase_av = ascii_value - 65; // uppercase_ascii_value
    int lowercase_av = ascii_value - 97; // lowercase_ascii_value
    if ((uppercase_av >= 0) && (uppercase_av <= 25)) {
      if (uppercase_av >= 24) {
	strcat(decoded_name, uppercase_letters[uppercase_av % 24]);
      } else {
	strcat(decoded_name, uppercase_letters[uppercase_av + 2]);
      }
    } else if ((lowercase_av >= 0) && (lowercase_av <= 25)) {
      if (lowercase_av >= 24) {
	strcat(decoded_name, lowercase_letters[lowercase_av % 24]);
      } else {
	strcat(decoded_name, lowercase_letters[lowercase_av + 2]);
      }
    } else if (ascii_value == 32) {
      strcat(decoded_name, " ");
    } else {
      printf("Name contains non-alphanumeric symbols \n");
      exit(0);
    }
  }
  return decoded_name;
}

// completely remove directory
void remove_directory(const char* path) {
  DIR* dir = opendir(path);
  struct dirent* dp;

  if (dir != NULL) {
    while ((dp = readdir(dir)) != NULL) {
      if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
	continue;
      } else if (dp->d_type == DT_DIR) {
	char* newpath = malloc(sizeof(char)*4096);
	strcpy(newpath, path);
	strcat(newpath, "/");
	strcat(newpath, dp->d_name);
	remove_directory(newpath);
      } else {
	char* newpath2 = malloc(sizeof(char)*4096);
	strcpy(newpath2, path);
	strcat(newpath2, "/");
	strcat(newpath2, dp->d_name);
	remove(newpath2);
      }
    }
    rmdir(path);
  }
}

// function that checks if file doesnt exist or is empty
bool isFilePopulated(char* filepath) {
  struct stat st;
  if(stat(filepath, &st)) {
    return false;
  } else if(st.st_size <= 1) {
    return false;
  } else {
    return true;
  }
}

// function that checks if directory doesn't exist or is empty (contains empty candidate files)
bool isDirPopulated(char* directory) {
  DIR* dir = opendir(directory);
  struct dirent* dp;
  int n = 0;

  if (dir == NULL) {
    printf("Error: input directory does not exist\n");
    closedir(dir);
    return false;
  } else {
    while ((dp = readdir(dir)) != NULL) {
      if (dp->d_type == DT_DIR || !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") || !strcmp(dp->d_name, ".DS_Store")) {
	continue;
      } else {
	char* path = malloc(sizeof(char)*4096);
	strcpy(path, directory);
	strcat(path, "/");
	strcat(path, dp->d_name);
	if (!isFilePopulated(path)) {
	  free(path);
	  continue;
	} else {
	  free(path);
	  n = 1;
	  closedir(dir);
	  return true;
	}
      }
    }
  }
  if (!n) {
    printf("Error: input directory is empty\n");
    closedir(dir);
    return false;
  }
  return true;
}

// check if any input directory files don't correspond to leaf nodes
/*
void verifyLeafNodes(char* inputdir, struct Map* parents, int numFiles) {
  DIR* dir = opendir(inputdir);
  struct dirent* dp;
  int count = 0;

  while ((dp = readdir(dir)) != NULL) {
    if (dp->d_type == DT_DIR || !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") || !strcmp(dp->d_name, ".DS_Store")) {
      continue;
    }

    struct Map* currentParent = parents->next;

    while (currentParent != NULL) {
      if (strcmp(dp->d_name, (char*)currentParent->value) == 0) {
	count ++;
	break;
      } else {
	currentParent = currentParent->next;
      }
    }
  }

  if (count == numFiles) {
    printf("Error: input directory does not exist\n");
    exit(1);
  }

  if (count >= 1) {
    printf("%d file(s) in input directory aren't leaf nodes\n", count);
    exit(1);
  }
}
*/

char *trimwhitespace(char *str)
{
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
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
