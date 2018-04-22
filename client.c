#define _BSD_SOURCE
#define NUM_ARGS 4

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "utils.h"
#include "map.h"

#define SERVER_PORT 5000

char* count_candidates(char* fpath) {
  //printf("FPATH: %s\n", fpath);
  FILE* file = fopen(fpath, "r");
  if (!file) {
    printf("Error: File '%s' doesn't exist\n", fpath);
    exit(1);
  } else {
    FILE* file = fopen(fpath, "r");
    struct Map* candidates = createMap();

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
      // filter newlines
      if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
      }
      //printf("LINE:                                    %s\n", line);
      struct Map* candidate = findInMap(candidates, line);

      if (candidate == NULL) {
        insertIntoMap(candidates, strdup(line), (void*)1);
      } else {
        insertIntoMap(
          candidates,
          strdup(line),
          (void*)((int)candidate->value + 1));
      }
    }
    char* candidate_counts = malloc(sizeof(char)*4081);
    candidates = candidates->next;
    while (candidates != NULL) {
      //printf("NAME: %s\n", candidates->key);
      //printf("VAL: %d\n", (int)candidates->value);
      char name[100];
      char count[5];
      strcpy(name, candidates->key);
      sprintf(count, "%d", (int)candidates->value);
      strcat(candidate_counts, name);
      strcat(candidate_counts, ":");
      strcat(candidate_counts, count);
      if (candidates->next != NULL) {
        strcat(candidate_counts, ",");
      }
      candidates = candidates->next;
    }
    candidates = NULL;
    printf("COUNTS: %s\n", candidate_counts);
    return candidate_counts;
  }
}

/*
char* get_request(char* dirpath, char* line) {
  //printf("Getting request for line:.........%s\n", line);
  char** splitString = malloc(sizeof(char)*1024);
  int numTokens = makeargv(line, " ", &splitString);
  int return_winner = strcmp(splitString[0], "Return_Winner");
  int count_votes = strcmp(splitString[0], "Count_Votes");
  int open_polls = strcmp(splitString[0], "Open_Polls");
  int add_votes = strcmp(splitString[0], "Add_Votes");
  int remove_votes = strcmp(splitString[0], "Remove_Votes");
  int close_polls = strcmp(splitString[0], "Close_Polls");
  if (!return_winner) {
    return "RW;               ;\0";
  } else if (!count_votes || !open_polls || !close_polls) {
    char* request = malloc(sizeof(char)*1024);
    if (!count_votes) {
      strcat(request, "CV;");
    } else if (!open_polls) {
      strcat(request, "OP;");
    } else {
      strcat(request, "CP;");
    }
    char region_name[15];
    strcpy(region_name, splitString[1]);
    for (int i = 0; i < 15 - strlen(splitString[1]); i++) {
      strcat(region_name, " ");
    }
    strcat(request, region_name);
    strcat(request, ";\0");
    return request;
  } else if (!add_votes || !remove_votes){
    // printf("ADD OR REMOVE\n");
    char* request = malloc(sizeof(char)*1024);
    if (!add_votes) {
      strcat(request, "AV;");
    } else {
      strcat(request, "RV;");
    }
    char* region_path = malloc(sizeof(char)*1024);
    strcpy(region_path, dirpath);
    strcat(region_path, splitString[2]);
    char* counts = count_candidates(region_path);
    //free(region_path);
    char region_name[15];
    strcpy(region_name, splitString[1]);
    for (int i = 0; i < 15-strlen(splitString[1]); i++) {
      strcat(region_name, " ");
    }
    strcat(request, region_name);
    strcat(request, ";");
    strcat(request, counts);
    strcat(request, "\0");
    return request;
  } else {
    printf("BAD REQUEST\n");
    exit(1);
  }
}
*/

/*
void read_requests(int sock, char* req_path) {
  if (!isFilePopulated(req_path)) {
    printf("Error: Input req file doesn't exist or is empty\n");
  } else {
    FILE* file = fopen(req_path, "r");

    char** splitString = malloc(sizeof(char)*1024);
    int numTokens = makeargv(req_path, "/", &splitString);
    char* dirpath = malloc(sizeof(char)*1024);
    for (int i = 0; i < numTokens-1; i++) {
      strcat(dirpath, splitString[i]);
      strcat(dirpath, "/");
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
      // filter newlines
      if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
      }
      char* request = get_request(dirpath, line);
      //strcpy(request, get_request(dirpath, line));
      printf("Request: %s\n", request);
      if((send(sock, request, strlen(request), 0)) < 0) {
        printf("SEND FAILED\n");
      }
    }
    free(splitString);
    free(dirpath);
    fclose(file);
  }
}
*/

int createRequest(char* line, char* request) {
  char** splitLine = malloc(sizeof(char)*strlen(line));
  int numTokens = makeargv(line, " ", &splitLine);

  if (strcmp(splitLine[0], "Open_Polls") == 0) {
    sprintf(request, "OP;%s;\0", splitLine[1]);
  } else if (strcmp(splitLine[0], "Close_Polls") == 0) {
    sprintf(request, "CP;%s;\0", splitLine[1]);
  } else if (strcmp(splitLine[0], "Add_Votes") == 0) {
    char* voteCounts = count_candidates(splitLine[2]);
    // NOTE: not sure if freeing will modify request...
    sprintf(request, "AV;%s;%s\0", splitLine[1], voteCounts);
    free(voteCounts);
  } else if (strcmp(splitLine[0], "Remove_Votes") == 0) {
    char* voteCounts = count_candidates(splitLine[2]);
    // NOTE: not sure if freeing will modify request...
    sprintf(request, "RV;%s;%s\0", splitLine[1], voteCounts);
    free(voteCounts);
  } else if (strcmp(splitLine[0], "Count_Votes") == 0) {
    sprintf(request, "CV;%s\0", splitLine[1]);
  } else if (strcmp(splitLine[0], "Return_Winner") == 0) {
    sprintf(request, "RW;;\0");
  } else {
    // error state
    return 1;
  }

  printf("sending request: %s\n", request);
  return 0;
}

void sendRequest(char* ip, int port, char* request) {
  // connect to server
  int sock = socket(AF_INET , SOCK_STREAM , 0);

  struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr *) &address, sizeof(address)) == 0) {
    // send(sock, (void*)request, strlen(request), 0);
    write(sock, (void*)request, strlen(request)+1);

    // print response from server
    char* response = malloc(sizeof(char)*1024); // assumed max response size of 1024
    read(sock, response, 1024);

    printf("response: %s\n", response);

    close(sock);
  } else {
    perror("Connection failed!");
  }
}

int main(int argc, char** argv) {

	if (argc != 4) {
    // req file, server ip, server port
		printf("Wrong number of args, expected %d, given %d\n", 4, argc - 1);
		exit(1);
	} else {
    printf("IP: %s\n", argv[2]);
    printf("PORT: %s\n", argv[3]);

    // read file
    if (!isFilePopulated(argv[1])) {
      printf("Error: Input req file doesn't exist or is empty\n");
      exit(1);
    }

    // read input file and send requests
    FILE* reqFile = fopen(argv[1], "r");

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    // change directory to that of input file
    char** splitPath = malloc(sizeof(char)*strlen(argv[1]));
    int pathTokens = makeargv(argv[1], "/", &splitPath);
    char* inputPath = malloc(sizeof(char)*strlen(argv[1]));
    strcpy(inputPath, "");

    for (int i = 0; i < pathTokens - 1; i++) {
      strcat(inputPath, splitPath[i]);
      strcat(inputPath, "/");
    }

    printf("%s\n", inputPath);

    chdir(inputPath);
    free(splitPath);
    free(inputPath);

    // translate each line of file and send translation to server
    while ((read = getline(&line, &len, reqFile)) != -1) {
      // filter newlines
      if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
      }

      // max msg size assumed to be 1024
      char* req = malloc(sizeof(char)*1024);
      int err = createRequest(line, req);

      if (err != 1) {
        sendRequest(argv[2], atoi(argv[3]), req);
      }

      free(req);
    }
  }
}
