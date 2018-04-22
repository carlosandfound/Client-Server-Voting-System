struct Queue {
  struct QueueNode* front;
  struct QueueNode* back;
};

struct QueueNode {
  void* item;
  struct QueueNode* next;
};

struct Queue* createQueue() {
  struct Queue* q = malloc(sizeof(struct Queue));
  q->front = NULL;
  q->back = NULL;

  return q;
}

void enqueue(struct Queue* q, void* e) {
  struct QueueNode* qn = malloc(sizeof(struct QueueNode));
  qn->item = e;

  if (q->back == NULL) {
    q->front = qn;
    q->back = qn;
  } else {
    q->back->next = qn;
    q->back = qn;
  }
}

struct QueueNode* dequeue(struct Queue* q) {
  if (q->front == NULL) {
    return NULL;
  } else {
    struct QueueNode* qn = q->front;
    q->front = q->front->next;

    if (q->front == NULL) {
      q->back = NULL;
    }

    return qn;
  }
}
