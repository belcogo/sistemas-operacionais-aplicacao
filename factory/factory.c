#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// array size default
#define BUFFER_SIZE 30
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

typedef struct node {
    product_t *node_product;
    struct node *next;
} node_t;

// mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// mutex condition
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
// product queue
node_t *head = NULL;

// int *produce(char type, short int price_range, short int quality);
// int *consume(product_t *product_to_consume);
void *producer();
void *consumer();
useconds_t generate_random_time();
// void *update_inventory(product_t *product_to_consume);
void enqueue(node_t **head, product_t node_value);
product_t dequeue(node_t **head);

int main() {
  pthread_t threads[N];
  int result;
  void *thread_result;

  for (int i = 0; i < N; i++) {
    if (i >= 3) {
      result = pthread_create(&threads[i], NULL, &consumer, NULL);
    } else {
      result = pthread_create(&threads[i], NULL, &producer, NULL);
    }

    if (result != 0) {
      perror("Something happened during thread creation");
      exit(EXIT_FAILURE);
    }
  }

  // for (int i = 0; i < N; i++) {
  //   result = pthread_join(threads[i], &thread_result);

  //   if (result != 0) {
  //     perror("Something happened during thread join");
  //     exit(EXIT_FAILURE);
  //   }
  // }
}

// int *produce(char type, short int price_range, short int quality) {

// }

// int *consume(product_t *product_to_consume) {

// }

void *producer() {
  product_t *new_product;
  useconds_t times[2] = {0, 0};

  new_product.type = 'A';
  new_product.price_range = GOLD;
  new_product.quality = HIGH;

  while(1) {
    if (times[0] == times[1]) {
      times[0] = generate_random_time();
      times[1] = generate_random_time();
    } else {
      if (times[0] > times[1]) {
        new_product.produce_time = times[0];
        new_product.consume_time = times[1];
      } else {
        new_product.produce_time = times[1];
        new_product.consume_time = times[0];
      }
      break;
    }
  }

  pthread_mutex_lock(&mutex);
  usleep(new_product->produce_time);
  enqueue(&head, new_product);
  pthread_mutex_unlock(&mutex);

  return 0;
}

void *consumer() {
  while(1) {
    
  }
}

void generate_random_time() {
  useconds_t time_to_wait;
  time_to_wait = random() % 5000;
  return time_to_wait;
}

// void *update_inventory(product_t *product_to_consume) {

// }


// queue source code: https://gist.github.com/kroggen/5fc7380d30615b2e70fcf9c7b69997b6

void enqueue(node_t **head, product_t *node_value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->node_product = node_value;
    new_node->next = *head;

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

    retval = current->node_product;
    free(current);
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retval;
}
