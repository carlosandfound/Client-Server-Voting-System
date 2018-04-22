#include <stdlib.h>
#include <string.h>

struct Map {
  char* key;
  void* value;
  struct Map* next;
};

struct Map* createMap() {
  struct Map* newMap = malloc(sizeof(struct Map));

  newMap->key = NULL;
  newMap->value = NULL;
  newMap->next = NULL;

  return newMap;
}

struct Map* createMapNode(const char* key, const void* value) {
  struct Map* newMap = malloc(sizeof(struct Map));

  newMap->key = key;
  newMap->value = value;
  newMap->next = NULL;

  return newMap;
}

struct Map* findInMap(struct Map* map, const char* key) {
  struct Map* n = map->next;

  while (n != NULL) {
    if (strcmp(n->key, key) == 0) {
      return n;
    } else {
      n = n->next;
    }
  }

  return NULL;
}

struct Map* insertIntoMap(struct Map* map, const char* key, const void* value) {
  struct Map* n = findInMap(map, key);

  if (n == NULL) {
    // if key not found
    n = createMapNode(key, value);
    struct Map* temp = map->next;
    map->next = n;
    n->next = temp;
  } else {
    n->value = value;
  }

  return n;
}
