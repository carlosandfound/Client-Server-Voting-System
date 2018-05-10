struct List {
  void* value;
  struct List* next;
};

struct List* createList() {
  struct List* newList = malloc(sizeof(struct List));

  newList->value = NULL;
  newList->next = NULL;

  return newList;
}

struct List* createListNode(void* value) {
  struct List* newNode = malloc(sizeof(struct List));

  newNode->value = value;
  newNode->next = NULL;

  return newNode;
}

void addNode(struct List* list, struct List* newNode) {
  struct List* temp = list->next;
  list->next = newNode;
  newNode->next = temp;
}
