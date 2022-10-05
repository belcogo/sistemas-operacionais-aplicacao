#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>

// states
#define PRODUCING 1
#define CONSUMING -1
#define ON_HOLD 0

// number of threads
#define N 5

// type
#define A 'A'
#define B 'B'
#define C 'C'

// price range
#define GOLD 1
#define SILVER 0
#define BRONZE -1

// quality
#define HIGH 1
#define MEDIUM 0
#define LOW -1

// struct product
typedef struct product {
  char type;
  short int price_range;
  short int quality;
  useconds_t produce_time;
  useconds_t consume_time;
} product_t;

// struct for queue
typedef struct node {
    product_t *product;
    struct node *next;
} node_t;

// iterator
int i;

// types array
char types[N] = {A, B, C, A, B};

// semaphores
sem_t mutex;
sem_t semaphores[N];
int semaphores_to_wait[N][4] = {
  {1, 2, 3, 4},
  {0, 2, 3, 4},
  {0, 1, 3, 4},
  {0, 1, 2, 4},
  {0, 1, 2, 3},
};

// state array
int state[N];

// product queue
node_t *head = NULL;
// product number
int number_of_products = 0;

void *producer(int);
void *consumer(int);
useconds_t generate_random_time();
void check_states(int i);
void enqueue(node_t **head, product_t *node_value);
product_t *dequeue(node_t **head);
void check_status(int idx);

int main() {
  pthread_t threads[N];
  int result;
  void *thread_result;

  result = sem_init(&mutex, 0, 1);
    if (result != 0) {
      perror("init mutex");
      exit(EXIT_FAILURE);
    }

  for (i = 0; i < N; i++) {
    state[i] = ON_HOLD;
    result = sem_init(&semaphores[i], 0, 0);
    if (result != 0) {
      perror("init sem");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < 3; i++) {
    if (i >= 3) {
      result = pthread_create(&threads[i], NULL, &consumer, i);
    } else {
      result = pthread_create(&threads[i], NULL, &producer, i);
    }

    if (result != 0) {
      perror("Something happened during thread creation");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < 3; i++) {
    result = pthread_join(threads[i], &thread_result);

    if (result != 0) {
      perror("Something happened during thread join");
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}

void *producer(int i) {
  // wait semaphore
  printf("%d\n", i);
  char type = types[i];
  printf("INÍCIO PRODUTOR - TIPO %c\n", type);
  product_t *new_product;
  useconds_t times[2] = {0, 0};

  new_product->type = type;
  new_product->price_range = GOLD;
  new_product->quality = HIGH;

  while(1) {
    if (times[0] == times[1]) {
      times[0] = generate_random_time();
      times[1] = generate_random_time();
    } else {
      if (times[0] > times[1]) {
        new_product->produce_time = times[0];
        new_product->consume_time = times[1];
      } else {
        new_product->produce_time = times[1];
        new_product->consume_time = times[0];
      }
      break;
    }
  }

  usleep(new_product->produce_time);
  enqueue(&head, new_product);
  if (number_of_products == 1) {
    //post semaphore
  }
  printf("FIM PRODUTOR - TIPO %c\n", type);
}

void *consumer(int i) {
  printf("%d\n", i);
  char type = types[i];
  printf("INÍCIO CONSUMIDOR - TIPO %c\n", type);

  //wait semaphores

  while(1) {
    char current_product_type = head->product->type;
    if (current_product_type == type) {
      usleep(head->product->consume_time);
      product_t *dequeued_product = dequeue(&head);
      printf("%s - %d", dequeued_product->type, dequeued_product->price_range);
    }
  }

  //post semaphore
  printf("FIM CONSUMIDOR - TIPO %c\n", type);
}

useconds_t generate_random_time() {
  useconds_t time_to_wait;
  time_to_wait = random() % 5000;
  return time_to_wait;
}

// queue source code: https://gist.github.com/kroggen/5fc7380d30615b2e70fcf9c7b69997b6

void enqueue(node_t **head, product_t *node_value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->product = node_value;
    new_node->next = *head;
    number_of_products++;
    *head = new_node;
}

product_t *dequeue(node_t **head) {
    node_t *current, *prev = NULL;
    int retval = -1;

    if (*head == NULL) return -1;

    current = *head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->product;
    free(current);
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    number_of_products--;
    return retval;
}

void check_status(int idx) {
  int wait_sems = semaphores_to_wait[idx];
  bool ok = true;
  for (i = 0; i < N; i++) {
    if (i != idx && state[i] != ON_HOLD) {
      ok = false;
      break;
    }
  }

  if (!ok) return;

  if (idx < 3) state[idx] = PRODUCING;
  if (idx > 2) state[idx] = CONSUMING;
  sem_post(&semaphores[idx]);
}
