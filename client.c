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

#define SERVER_PORT 7501

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
    //printf("COUNTS: %s\n", candidate_counts);
    return candidate_counts;
  }
}

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

int main(int argc, char** argv) {

	if (argc != 4) {
		printf("Wrong number of args, expected %d, given %d\n", 4, argc - 1);
		exit(1);
	} else {
    printf("IP: %s\n", argv[2]);
    printf("PORT: %s\n", argv[3]);
    int server_ip = atoi(argv[2]);
    int server_port = atoi(argv[3]);

    int sock = socket(AF_INET , SOCK_STREAM , 0);

  	// Specify an address to connect to (we use the local host or 'loop-back' address).
  	struct sockaddr_in address;
  	address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[3]));
    address.sin_addr.s_addr = inet_addr(argv[2]);

    if (connect(sock, (struct sockaddr *) &address, sizeof(address)) == 0) {
      read_requests(sock, argv[1]);
  		close(sock);
  	} else {
  		perror("Connection failed!");
  	}
  }
}
