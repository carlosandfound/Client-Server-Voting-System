/*
Names: Carlos Alvarenga, Jeremy Herzog
X500’s: alvar357, herzo175
id's: 5197501, 5142295
Lecture Section: Both students are in lecture section 16
Extra Credit: YES
*/

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

// kep track of buffer space for response message
ssize_t responseBuf = 0;

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
  pthread_mutex_t* masterLock;
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

// find specfic poll region in the tree
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

// read the dag file and create poll regions with candidates
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

  // (for debugging)
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

// Add votes of region to respective poll
char* addVotes(struct List* polls, char* region, char* votes) {

  // should not be null
  struct Poll* poll = findPoll(polls, region);

  // assumed max response size of 256
  char* msg = malloc(sizeof(char)*256);

  if (poll == NULL) {
    sprintf(msg, "NR;%s\0", region);
    responseBuf = 18;
    return msg;
  }

  if (poll->status != 1) {
    sprintf(msg, "RC;%s\0", region);
    responseBuf = 18;
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
  free(splitData);

  sprintf(msg, "SC;\0");
  responseBuf = 5;
  return msg;
}

// remove votes from region's respective poll
char* removeVotes(struct List* polls, char* region, char* votes) {
  // should not be null
  struct Poll* poll = findPoll(polls, region);
  char* msg = malloc(sizeof(char)*256);

  if (poll == NULL) {
    sprintf(msg, "NR;%s\0", region);
    responseBuf = 18;
    return msg;
  }

  if (poll->status != 1) {
    sprintf(msg, "RC;%s\0", region);
    responseBuf = 18;
    return msg;
  }

  char** splitData = malloc(sizeof(char)*strlen(votes));
  int numCandidates = makeargv(votes, ",", &splitData);

  for (int i = 0; i < numCandidates; i++) {
    char** splitCandidate = malloc(sizeof(char)*4);
    makeargv(splitData[i], ":", &splitCandidate);

    struct Map* candidate = findInMap(poll->candidates, splitCandidate[0]);
    int amount = atoi(splitCandidate[1]);

    if (candidate == NULL || (int)candidate->value - amount < 0) {
      sprintf(msg, "IS;%s\0", splitCandidate[0]);
      responseBuf = 103;
      return msg;
    } else {
      pthread_mutex_unlock(poll->lock);
      insertIntoMap(
        poll->candidates,
        splitCandidate[0],
        (void*)((int)candidate->value) - amount);
      pthread_mutex_unlock(poll->lock);
    }

    free(splitCandidate);
  }
  free(splitData);

  sprintf(msg, "SC;\0");
  responseBuf = 3;
  return msg;
}

// function thats closes/opens region poll
char* setStatus(struct List* polls, char* region, unsigned int status) {
  struct Poll* poll = findPoll(polls, region);
  char* msg = malloc(sizeof(char)*256);

  // should not be null unless region doesn't exist
  if (poll == NULL) {
    sprintf(msg, "NR;%s\0", region);
    responseBuf = 18;
    return msg;
  }

  if (poll->status == status || (poll->status == 2 && status == 0)) {
    if (poll->status == 0) {
      sprintf(msg, "PF;%s:%s\0", region, "Inital");
    } else if (poll->status == 1) {
      sprintf(msg, "PF;%s:%s\0", region, "Open");
    } else {
      sprintf(msg, "PF;%s:%s\0", region, "Closed");
    }

    responseBuf = 23;
    return msg;
  }

  if (poll->status > 1) {
    sprintf(msg, "RR;%s\0", region);
    responseBuf = 18;
    return msg;
  }

  // set parent status
  pthread_mutex_lock(poll->lock);

  if (poll->status == 1 && status == 0) {
    poll->status = 2;
  } else {
    poll->status = status;
  }

  pthread_mutex_unlock(poll->lock);

  // recursively set status of all children
  for (struct List* c = poll->children->next; c != NULL; c = c->next) {
    msg = setStatus(polls, ((struct Poll*)(c->value))->name, status);

    // NOTE: don't set status for child poll statuses being changed
    /*
    if (strcmp(msg, "SC;\0") != 0) {
      return msg;
    }
    */
  }

  sprintf(msg, "SC;\0");
  responseBuf = 3;
  return msg;
}

// find the winner of the election
char* findWinner(struct List* polls) {
  char* msg = malloc(sizeof(char)*256);
  char* winnerName;
  int winnerCount = 0;
  bool winner = false;

  for (struct List* n = polls->next; n != NULL; n = n->next) {
    struct Poll* p = (struct Poll*)n->value;

    // trying to find a winner when a poll is still open
    if (p->status == 1) {
      sprintf(msg, "RO;%s\0", p->name);
      responseBuf = 18;
      return msg;
    }

    if (p->status == 0) {
      sprintf(msg, "UE;\0");
      responseBuf = 18;
      return msg;
    }

    for (struct Map* c = p->candidates->next; c != NULL; c = c->next) {
      if (((int)c->value) > winnerCount) {
        winner = true;
        winnerName = c->key;
        winnerCount = (int)c->value;
      }
    }
  }

  if(winner) {
    sprintf(msg, "SC;Winner:%s\0", winnerName);
  } else {
    sprintf(msg, "SC:Winner:No winner\0");
  }
  responseBuf = 110;
  return msg;
}

// count the votes at a specific poll in region
char* countVotes(struct List* polls, char* region) {
  struct Poll* poll = findPoll(polls, region);

  // assume max string length
  char* msg = malloc(sizeof(char)*256);
  strcpy(msg, "");

  // should not be null
  if (poll == NULL) {
    sprintf(msg, "NR;%s\0", region);
    responseBuf = 18;
    return msg;
  }

  struct Map* candidates = createMap();

  countVotesR(poll, candidates);

  // count vote for parent node
  /*
  for (struct Map* c = poll->candidates->next; c != NULL; c = c->next) {
    struct Map* c2 = findInMap(candidates, c->key);

    if (c2 == NULL) {
      insertIntoMap(candidates, strdup(c->key), c->value);
    } else {
      insertIntoMap(candidates, c->key, (void*)((int)c2->value + (int)c->value));
    }
  }

  // count votes for children
  for (struct List* c = poll->children->next; c != NULL; c = c->next) {
    countVotesR((struct Poll*)c->value, candidates);
  }
  */

  if (candidates->next == NULL) {
    // send no votes msg
    sprintf(msg, "SC;No votes.\0");
    responseBuf = 12;
    return msg;
  } else {
    strcat(msg, "SC;");
  }

  responseBuf = 256;

  for (struct Map* c = candidates->next; c != NULL; c = c->next) {
    char* info = malloc(sizeof(char)*15);

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

void countVotesR(struct Poll* poll, struct Map* candidates) {
  // NOTE: not thread safe but only one thread should run per candidates struct
  for (struct Map* c = poll->candidates->next; c != NULL; c = c->next) {
    struct Map* c2 = findInMap(candidates, c->key);

    if (c2 == NULL) {
      insertIntoMap(candidates, strdup(c->key), c->value);
    } else {
      insertIntoMap(candidates, c->key, (int)c2->value + (int)c->value);
    }
  }

  // recurse through children
  for (struct List* c = poll->children->next; c != NULL; c = c->next) {
    countVotesR((struct Poll*)c->value, candidates);
  }
}

// extra credit add region request handling
char* addRegion(struct List* polls, char* parentName, char* newRegion) {
  struct Poll* parent = findPoll(polls, parentName);

  // assume max string length
  char* msg = malloc(sizeof(char)*256);
  strcpy(msg, "");

  // should not be null
  if (parent == NULL) {
    sprintf(msg, "NR;%s\0", parent);
    responseBuf = 18;
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

  sprintf(msg, "SC;\0");
  return msg;
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

// get request from client and call appropriate function to handle request
// send back response in the end
void handleRequest(struct ThreadArgs* args) {
  // Buffer for data.
  // assumed max buffer length
  int bufferLength = 256;
  char* buffer = malloc(sizeof(char)*bufferLength);

  int readSize = recv(args->socket, (void*)buffer, bufferLength, 0);
  printf(
    "Request received from client at %s:%i,%s\n",
    inet_ntoa(args->clientAddress->sin_addr),
    (int)ntohs(args->clientAddress->sin_port),
    buffer);

  char** msgStrings = malloc(sizeof(char)*bufferLength);
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
      responseBuf = 18;
    } else {
      responseData = addVotes(args->polls, msgStrings[1], msgStrings[2]);
    }
  } else if (strcmp(msgStrings[0], "RV") == 0) {
    if (isLeaf(args->polls, msgStrings[1]) != 0) {
      responseData = malloc(sizeof(char)*3 + sizeof(char)*strlen(msgStrings[1]));
      sprintf(responseData, "NL;%s\0", msgStrings[1]);
      responseBuf = 18;
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
    pthread_mutex_lock(args->masterLock);
    responseData = addRegion(args->polls, msgStrings[1], msgStrings[2]);
    pthread_mutex_unlock(args->masterLock);
  } else {
    responseData = malloc(sizeof(char)*256);
    sprintf(responseData, "UC;%s\0", msgStrings[0]);
    responseBuf = 256;
  }

  // send back response data
  printf(
    "Sending response to client at %s:%i,%s\n",
    inet_ntoa(args->clientAddress->sin_addr),
    (int)ntohs(args->clientAddress->sin_port),
    responseData);

  write(args->socket, responseData, responseBuf);
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
  } else if (!isFilePopulated(argv[1])) {
    exit(1);
  } else {
    // read in DAG
    struct List* polls = createList();
    readDag(argv[1], polls);

    pthread_mutex_t* pollLock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pollLock, NULL);

    // Create a TCP socket.
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
      perror("setsockopt(SO_REUSEADDR) failed");
    }

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
      printf("Server listening on port %s\n", argv[2]);
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
      args->masterLock = pollLock;

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
}
