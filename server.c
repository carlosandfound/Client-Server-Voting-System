#define _BSD_SOURCE
#define NUM_ARGS 2

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
#include "list.h"

#define MAX_CONNECTIONS 100

struct Poll {
  char* name;
  struct Poll* parent;
  struct List* children;
  struct Map* candidates;
  unsigned int status;
  pthread_mutex_t* lock;
};

struct ThreadArgs {
  struct List* polls;
  int socket;
  struct sockaddr_in* clientAddress;
};

struct Poll* createPoll(char* name) {
  struct Poll* p = malloc(sizeof(struct Poll));

  p->name = name;
  p->parent = NULL;
  p->status = 0;
  p->children = createList();
  p->candidates = createMap();
  p->lock = malloc(sizeof(pthread_mutex_t));

  pthread_mutex_init(p->lock, NULL);

  return p;
}

struct Poll* findPoll(struct List* polls, char* name) {
  struct List* n = polls->next;

  while (n != NULL) {
    if (strcmp(((struct Poll*)(n->value))->name, name) == 0) {
      return (struct Poll*)n->value;
    } else {
      n = n->next;
    }
  }

  return NULL;
}

void readDag(char* dagPath, struct List* polls) {
  FILE* dagFile = fopen(dagPath, "r");

  char* line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, dagFile)) != -1) {
    // filter newlines
    if (line[read - 1] == '\n') {
      line[read - 1] = '\0';
    }

    // split dag line by spaces
    char** pollNames = malloc(sizeof(char)*strlen(line));

    int numNames = makeargv(line, ":", &pollNames);

    struct Poll* parent;

    // create parent if not already in master list
    if ((parent = findPoll(polls, pollNames[0])) == NULL) {
      parent = createPoll(strdup(pollNames[0]));
      struct List* n = createListNode((void*)parent);
      addNode(polls, n);
    }

    for (int i = 1; i < numNames; i++) {
      struct Poll* child;

      // add child to master list if it isn't already there
      if ((child = findPoll(polls, pollNames[i])) == NULL) {
        child = createPoll(strdup(pollNames[i]));
        child->parent = parent;
        struct List* n = createListNode((void*)child);
        addNode(polls, n);
      }

      // add child to parents
      struct List* n = createListNode((void*)child);
      addNode(parent->children, n);
    }

    free(pollNames);
  }

  // verify polls
  // all polls
  /*
  for (struct List* n = polls->next; n != NULL; n = n->next) {
    printf("name: %s\n", ((struct Poll*)(n->value))->name);
  }

  // polls and children
  for (struct List* n = polls->next; n != NULL; n = n->next) {
    struct Poll* parent = (struct Poll*)(n->value);
    printf("parent: %s, children: ", parent->name);

    for (struct List* c = parent->children->next; c != NULL; c = c->next) {
      printf("%s ", ((struct Poll*)(c->value))->name);
    }

    printf("\n");
  }
  */

  fclose(dagFile);
}

char* addVotes(struct List* polls, char* region, char* votes) {
  printf("add votes: region: %s, votes: %s\n", region, votes);

  // should not be null
  struct Poll* poll = findPoll(polls, region);
  // assumed max response size of 100
  char* msg = malloc(sizeof(char)*100);

  if (poll == NULL) {
    printf("Trying to insert into unknown poll\n");
    sprintf(msg, "NR;%s\0", region);
    return msg;
  }

  if (poll->status != 1) {
    printf("Trying to insert into closed poll\n");
    sprintf(msg, "RC;%s\0", region);
    return msg;
  }

  char** splitData = malloc(sizeof(char)*strlen(votes));
  int numCandidates = makeargv(votes, ",", &splitData);

  for (int i = 0; i < numCandidates; i++) {
    char** splitCandidate = malloc(sizeof(char)*4);
    makeargv(splitData[i], ":", &splitCandidate);

    struct Map* candidate = findInMap(poll->candidates, splitCandidate[0]);

    pthread_mutex_lock(poll->lock);

    if (candidate == NULL) {
      insertIntoMap(poll->candidates, strdup(splitCandidate[0]), (void*)atoi(splitCandidate[1]));
    } else {
      insertIntoMap(
        poll->candidates,
        strdup(splitCandidate[0]),
        (void*)((int)candidate->value) + atoi(splitCandidate[1]));
    }

    pthread_mutex_unlock(poll->lock);

    free(splitCandidate);
  }

  // aggreate upwards
  // NOTE no longer need to aggreate upwards
  /*
  if (poll->parent != NULL) {
    msg = addVotes(polls, poll->parent->name, votes);

    if (strcmp(msg, "SC;\0") != 0) {
      return msg;
    }
  }
  */

  // verify candidates
  for (struct Map* c = poll->candidates->next; c != NULL; c = c->next) {
    printf("candidate: %s, votes: %i\n", c->key, (int)c->value);
  }

  free(splitData);

  sprintf(msg, "SC;\0");
  return msg;
}

char* removeVotes(struct List* polls, char* region, char* votes) {
  printf("remove votes: region: %s, votes: %s\n", region, votes);

  // should not be null
  struct Poll* poll = findPoll(polls, region);
  char* msg = malloc(sizeof(char)*100);

  if (poll == NULL) {
    printf("Trying to insert into unknown poll\n");
    sprintf(msg, "NR;%s\0", region);
    return msg;
  }

  if (poll->status != 1) {
    printf("Trying to remove from closed poll\n");
    sprintf(msg, "RC;%s\0", region);
    return msg;
  }

  char** splitData = malloc(sizeof(char)*strlen(votes));
  int numCandidates = makeargv(votes, ",", &splitData);

  for (int i = 0; i < numCandidates; i++) {
    char** splitCandidate = malloc(sizeof(char)*4);
    makeargv(splitData[i], ":", &splitCandidate);

    struct Map* candidate = findInMap(poll->candidates, splitCandidate[0]);
    int amount = atoi(splitCandidate[1]);

    if (candidate == NULL) {
      printf("Trying to subtract from non-existent candidate\n");
      sprintf(msg, "IS;%s\0", splitCandidate[0]);
      return msg;
    } else if ((int)candidate->value - amount < 1) {
      pthread_mutex_lock(poll->lock);
      insertIntoMap(poll->candidates, strdup(splitCandidate[0]), (void*)0);
      pthread_mutex_unlock(poll->lock);
    } else {
      pthread_mutex_unlock(poll->lock);
      insertIntoMap(
        poll->candidates,
        strdup(splitCandidate[0]),
        (void*)((int)candidate->value) - amount);
      pthread_mutex_unlock(poll->lock);
    }

    free(splitCandidate);
  }

  // aggreate upwards
  // NOTE no longer need to aggreate upwards
  /*
  if (poll->parent != NULL) {
    msg = removeVotes(polls, poll->parent->name, votes);

    if (strcmp(msg, "SC;\0") != 0) {
      return msg;
    }
  }
  */

  // verify candidates
  for (struct Map* c = poll->candidates->next; c != NULL; c = c->next) {
    printf("candidate: %s, votes: %i\n", c->key, (int)c->value);
  }

  free(splitData);

  sprintf(msg, "SC;\0");
  return msg;
}

char* setStatus(struct List* polls, char* region, unsigned int status) {
  struct Poll* poll = findPoll(polls, region);
  char* msg = malloc(sizeof(char)*100);

  // should not be null
  if (poll == NULL) {
    printf("Trying to insert into unknown poll\n");
    sprintf(msg, "NR;%s\0", region);
    return msg;
  }

  if (poll->status == status || (poll->status == 2 && status == 0)) {
    printf("Trying to set status to same status\n");
    sprintf(msg, "PF;%s:%s\0", region, status == 0 ? "Closed" : "Open");
    return msg;
  }

  if (poll->status > 1) {
    printf("Trying to reopen poll\n");
    sprintf(msg, "RR;%s\0", region);
    return msg;
  }

  // set parent status
  pthread_mutex_lock(poll->lock);

  if (poll->status == 1 && status == 0) {
    poll->status = 2;
  } else {
    poll->status = status;
  }

  // printf("change status: poll: %s, status: %i\n", poll->name, poll->status);
  pthread_mutex_unlock(poll->lock);

  // recursively set status of all children
  for (struct List* c = poll->children->next; c != NULL; c = c->next) {
    msg = setStatus(polls, ((struct Poll*)(c->value))->name, status);

    if (strcmp(msg, "SC;\0") != 0) {
      return msg;
    }
  }

  sprintf(msg, "SC;\0");
  return msg;
}

char* findWinner(struct List* polls) {
  char* msg = malloc(sizeof(char)*100);
  char* winnerName;
  int winnerCount = 0;

  for (struct List* n = polls->next; n != NULL; n = n->next) {
    struct Poll* p = (struct Poll*)n->value;

    if (p->status == 1) {
      printf("Trying to find winner in open poll\n");
      sprintf(msg, "RO;%s\0", p->name);
      return msg;
    }

    for (struct Map* c = p->candidates->next; c != NULL; c = c->next) {
      if (((int)c->value) > winnerCount) {
        winnerName = c->key;
        winnerCount = (int)c->value;
      }
    }
  }

  sprintf(msg, "SC;Winner:%s\0", winnerName);
  return msg;
}

char* countVotes(struct List* polls, char* region) {
  struct Poll* poll = findPoll(polls, region);

  // assume max string length
  char* msg = malloc(sizeof(char)*100);
  strcpy(msg, "");

  // should not be null
  if (poll == NULL) {
    printf("Trying to insert count votes of unknown poll\n");
    sprintf(msg, "NR;%s\0", region);
    return msg;
  }

  if (poll->candidates->next == NULL) {
    // send no votes msg
    sprintf(msg, "SC;No votes.\0");
    return msg;
  } else {
    strcat(msg, "SC;");
  }

  for (struct Map* c = poll->candidates->next; c != NULL; c = c->next) {
    char* info = malloc(sizeof(char)*10);

    sprintf(info, "%s:%i", c->key, (int)c->value);

    strcat(msg, info);

    if (c->next != NULL) {
      strcat(msg, ",");
    }

    free(info);
  }

  strcat(msg, "\0");
  return msg;
}

char* addRegion(struct List* polls, char* parentName, char* newRegion) {
  struct Poll* parent = findPoll(polls, parentName);

  // assume max string length
  char* msg = malloc(sizeof(char)*100);
  strcpy(msg, "");

  // should not be null
  if (parent == NULL) {
    printf("Trying to add region to non-existent parent\n");
    sprintf(msg, "NR;%s\0", parent);
    return msg;
  }

  struct Poll* newPoll = createPoll(strdup(newRegion));

  pthread_mutex_lock(newPoll->lock);
  newPoll->parent = parent;
  pthread_mutex_unlock(newPoll->lock);


  struct List* ln = createListNode((void*)newPoll);
  struct List* pn = createListNode((void*)newPoll);

  addNode(polls, ln);

  pthread_mutex_lock(parent->lock);
  addNode(parent->children, pn);
  pthread_mutex_unlock(parent->lock);
}

unsigned int isLeaf(struct List* polls, char* region) {
  for (struct List* n = polls->next; n != NULL; n = n->next) {
    struct Poll* p = (struct Poll*)n->value;

    if (p->name == region) {
      if (p->children->next == NULL) {
        // is leaf
        return 0;
      } else {
        return 1;
      }
    }
  }

  return 0;
}

void handleRequest(struct ThreadArgs* args) {
  // Buffer for data.
  // assumed max buffer length
  int bufLength = 3000;
  char* buffer = malloc(sizeof(char)*bufLength);

  int readSize = recv(args->socket, (void*)buffer, bufLength, 0);
  printf(
    "Request received from client at %s:%i\n",
    inet_ntoa(args->clientAddress->sin_addr),
    (int)ntohs(args->clientAddress->sin_port));
  printf("size: %i, message: %s\n", readSize, buffer);

  char** msgStrings = malloc(sizeof(char)*bufLength);
  int numTokens = makeargv(buffer, ";", &msgStrings);

  // trim whitespace
  for (int i = 0; i < numTokens; i++) {
    trimwhitespace(msgStrings[i]);
  }

  // handle request by type
  char* responseData;

  if (strcmp(msgStrings[0], "AV") == 0) {
    if (isLeaf(args->polls, msgStrings[1]) != 0) {
      responseData = malloc(sizeof(char)*3 + sizeof(char)*strlen(msgStrings[1]));
      sprintf(responseData, "NL;%s\0", msgStrings[1]);
    } else {
      responseData = addVotes(args->polls, msgStrings[1], msgStrings[2]);
    }
  } else if (strcmp(msgStrings[0], "RV") == 0) {
    if (isLeaf(args->polls, msgStrings[1]) != 0) {
      responseData = malloc(sizeof(char)*3 + sizeof(char)*strlen(msgStrings[1]));
      sprintf(responseData, "NL;%s\0", msgStrings[1]);
    } else {
      responseData = removeVotes(args->polls, msgStrings[1], msgStrings[2]);
    }
  } else if (strcmp(msgStrings[0], "OP") == 0) {
    responseData = setStatus(args->polls, msgStrings[1], 1);
  } else if (strcmp(msgStrings[0], "CP") == 0) {
    responseData = setStatus(args->polls, msgStrings[1], 0);
  } else if (strcmp(msgStrings[0], "RW") == 0) {
    responseData = findWinner(args->polls);
  } else if (strcmp(msgStrings[0], "CV") == 0) {
    responseData = countVotes(args->polls, msgStrings[1]);
  } else if (strcmp(msgStrings[0], "AR") == 0) {
    responseData = addRegion(args->polls, msgStrings[1], msgStrings[2]);
  } else {
    responseData = malloc(sizeof(char)*6);
    sprintf(responseData, "UC;%s\0", msgStrings[0]);
  }

  // send responseData
  printf(
    "Sending response to client at %s:%i\n",
    inet_ntoa(args->clientAddress->sin_addr),
    (int)ntohs(args->clientAddress->sin_port));

  printf("response: %s\n", responseData);
  write(args->socket, responseData, strlen(responseData));

  close(args->socket);
  printf(
    "Closed connection with client at %s:%i\n",
    inet_ntoa(args->clientAddress->sin_addr),
    (int)ntohs(args->clientAddress->sin_port));

  // free args and stuff
  free(responseData);
  free(msgStrings);
  free(buffer);

  free(args->clientAddress);
}

int main(int argc, char** argv) {
  if (argc != NUM_ARGS + 1) {
		printf("Wrong number of args, expected %d, given %d\n", NUM_ARGS, argc - 1);
		exit(1);
	}

  // read in DAG
  struct List* polls = createList();
  readDag(argv[1], polls);

  // Create a TCP socket.
	int sock = socket(AF_INET , SOCK_STREAM , 0);

	// Bind it to a local address.
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(atoi(argv[2]));
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (struct sockaddr *) &servAddress, sizeof(servAddress)) != 0 ||
    listen(sock, MAX_CONNECTIONS) != 0) {
      perror("error connecting to socket");
      exit(1);
  } else {
    printf("server listening on %s\n", argv[2]);
  }


  while (1) {
    // Now accept the incoming connections.
		struct sockaddr_in* clientAddress = malloc(sizeof(struct sockaddr_in));

		socklen_t size = sizeof(struct sockaddr_in);

    pthread_t t;
    struct ThreadArgs* args = malloc(sizeof(struct ThreadArgs));

    args->polls = polls;
    args->socket = accept(sock, (struct sockaddr*)clientAddress, &size);
    args->clientAddress = clientAddress;

    // spawn thread to handle request
    if (args->socket > - 1) {
      printf(
        "Connection initiated from client at %s:%i\n",
        inet_ntoa(args->clientAddress->sin_addr),
        (int)ntohs(args->clientAddress->sin_port));

      pthread_create(&t, NULL, handleRequest, args);
      pthread_detach(&t);
    }
  }

  // clean up
  shutdown(sock, SHUT_RDWR);
  close(sock);

  for (struct List* n = polls->next; n != NULL; n = n->next) {
    // free poll
    struct Poll* p = (struct Poll*)n->value;

    free(p->name);
    free(p->children);
    free(p->candidates);
    free(p->lock);

    free(p);
  }

  free(polls);
}
