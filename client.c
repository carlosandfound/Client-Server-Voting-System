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

/*
alias co1="./client PA4_Care_Pkg/TestCases/TestCase01/input.req 127.0.0.1 7501"
alias co2="./client PA4_Care_Pkg/TestCases/TestCase02/input.req 127.0.0.1 7501"
alias co3="./client PA4_Care_Pkg/TestCases/TestCase03/input.req 127.0.0.1 7501"

alias c1="./client2 PA4_Care_Pkg/TestCases/TestCase01/input.req 127.0.0.1 7501"
alias c2="./client2 PA4_Care_Pkg/TestCases/TestCase02/input.req 127.0.0.1 7501"
alias c3="./client2 PA4_Care_Pkg/TestCases/TestCase03/input.req 127.0.0.1 7501"

alias so1="./server PA4_Care_Pkg/TestCases/TestCase01/voting_regions.dag 7501"
alias so2="./server PA4_Care_Pkg/TestCases/TestCase02/voting_regions.dag 7501"
alias so3="./server PA4_Care_Pkg/TestCases/TestCase03/voting_regions.dag 7501"

alias s1="./server2 PA4_Care_Pkg/TestCases/TestCase01/voting_regions.dag 7501"
alias s2="./server2 PA4_Care_Pkg/TestCases/TestCase02/voting_regions.dag 7501"
alias s3="./server2 PA4_Care_Pkg/TestCases/TestCase03/voting_regions.dag 7501"
*/

char* count_candidates(char* fpath) {
  if (!isFilePopulated(fpath)) {
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
    return candidate_counts;
  }
}

void createRequest(char* line, char* request) {
  char** splitLine = malloc(sizeof(char)*strlen(line));
  int numTokens = makeargv(line, " ", &splitLine);

  if (strcmp(splitLine[0], "Open_Polls") == 0) {
    int padding = 15 - strlen(splitLine[1]);
    sprintf(request, "OP;%s%*s;\0", splitLine[1], padding, " ");
  } else if (strcmp(splitLine[0], "Close_Polls") == 0) {
    int padding = 15 - strlen(splitLine[1]);
    sprintf(request, "CP;%s%*s;\0", splitLine[1], padding, " ");
  } else if (strcmp(splitLine[0], "Add_Votes") == 0) {
    int padding = 15 - strlen(splitLine[1]);
    char* voteCounts = count_candidates(splitLine[2]);
    sprintf(request, "AV;%s%*s;%s\0", splitLine[1], padding, " ", voteCounts);
    free(voteCounts);
  } else if (strcmp(splitLine[0], "Remove_Votes") == 0) {
    int padding = 15 - strlen(splitLine[1]);
    char* voteCounts = count_candidates(splitLine[2]);
    sprintf(request, "RV;%s%*s;%s\0", splitLine[1], padding, " ", voteCounts);
    free(voteCounts);
  } else if (strcmp(splitLine[0], "Count_Votes") == 0) {
    int padding = 15 - strlen(splitLine[1]);
    sprintf(request, "CV;%s%*s;\0", splitLine[1], padding, " ");
  } else if (strcmp(splitLine[0], "Return_Winner") == 0) {
    sprintf(request, "RW;               ;\0");
  } else if (strcmp(splitLine[0], "Add_Region") == 0) {
    int padding = 15 - strlen(splitLine[1]);
    sprintf(request, "AR;%s%*s;%s\0", splitLine[1], padding, " ", splitLine[2]);
  } else {
    sprintf(request, "%s;               ;\0", splitLine[0]);
  }
}

void sendRequest(char* ip, int port, char* request) {
  // connect to server
  int sock = socket(AF_INET , SOCK_STREAM , 0);

  struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr*)&address, sizeof(address)) == 0) {
    printf("Initiated connection with server at %s:%d\n", ip, port);
    // send(sock, (void*)request, strlen(request), 0);
    printf("Sending request to server: %s\n", request);
    write(sock, (void*)request, strlen(request)+1);

    // print response from server
    char* response = malloc(sizeof(char)*1024); // assumed max response size of 1024
    read(sock, response, 1024);

    printf("Recieved response from server: %s\n", response);
  } else {
    perror("Connection failed!");
  }

  printf("Closed connection with server at %s:%d\n", ip, port);
  close(sock);
}

int main(int argc, char** argv) {

	if (argc != 4) {
    // req file, server ip, server port
		printf("Wrong number of args, expected %d, given %d\n", 4, argc - 1);
		exit(1);
	} else {
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
      char* req = malloc(sizeof(char)*256);
      createRequest(line, req);

      //if (err != 1) {
        sendRequest(argv[2], atoi(argv[3]), req);
      //}

      free(req);
    }
  }
}
