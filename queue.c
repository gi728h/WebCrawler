#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A linked list (LL) node to store a queue entry
struct QNode
{
    char *url;
    struct QNode *next;
};

// The queue, front stores the front node of LL and rear
// stores the last node of LL
struct Queue
{
    struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode *newNode(const char *k)
{
    
    struct QNode *temp = (struct QNode *)malloc(sizeof(struct QNode));
    if (temp == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    temp->url = malloc(strlen(k) + 1);
    if (temp->url == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(temp);
        exit(EXIT_FAILURE);
    }

    strcpy(temp->url, k);  // Use strcpy instead of strncpy
    temp->next = NULL;

    return temp;
}

// A utility function to create an empty queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a url k to q
void enQueue(struct Queue *q, const char *k)
{
    // Create a new LL node
    struct QNode *temp = newNode(k);
    // printf("%c\n",temp->url);
    // If queue is empty, then new node is front and rear
    // both
    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove a url from given queue q
void deQueue(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return;

    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
}

void printQueue(struct Queue *queue) {
    if (queue == NULL || queue->front == NULL) {
        printf("Queue is empty\n");
        return;
    }

    // Iterate through the queue and print each URL
    struct QNode *current = queue->front;
    while (current != NULL) {
        printf("URL: %s\n", current->url);
        current = current->next;
    }
}