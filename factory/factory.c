#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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
    product_t *product;
    struct node *next;
} node_t;

// mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// mutex conditions
pthread_cond_t consume_type_A = PTHREAD_COND_INITIALIZER;
pthread_cond_t consume_others = PTHREAD_COND_INITIALIZER;
// product queue
node_t *head = NULL;
// product number
int number_of_products = 0;

void *producer(char type);
void *consumer(char type);
useconds_t generate_random_time();
// void *update_inventory(product_t *product_to_consume);
void enqueue(node_t **head, product_t *node_value);
product_t *dequeue(node_t **head);

int main() {
  pthread_t threads[N];
  int result;
  void *thread_result;
  char types[3] = {'A', 'B', 'C'};

  for (int i = 0; i < N; i++) {
    printf("%d - %s\n", i, types[i]);
    if (i >= 3) {
      result = pthread_create(&threads[i], NULL, &consumer, NULL);
    } else {
      result = pthread_create(&threads[i], NULL, &producer, &types[i]);
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

  return 0;
}

void *producer(char type) {
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

  pthread_mutex_lock(&mutex);
  usleep(new_product->produce_time);
  enqueue(&head, new_product);
  if (number_of_products == 1) {
    if (type == A) {
      pthread_cond_signal(&consume_type_A);
    } else {
      pthread_cond_signal(&consume_others);
    }
  }
  pthread_mutex_unlock(&mutex);

  return 0;
}

void *consumer(char type) {
  pthread_mutex_lock(&mutex);
  while(1) {
    if (number_of_products == 0) {
      if (type == A) {
        pthread_cond_wait(&consume_type_A, &mutex);
      } else {
        pthread_cond_wait(&consume_others, &mutex);
      }
    }

    char current_product_type = head->product->type;
    if (current_product_type == type) {
      usleep(head->product->consume_time);
      product_t *dequeued_product = dequeue(&head);
      printf("%s - %d", dequeued_product->type, dequeued_product->price_range);
    }
  }
  pthread_mutex_unlock(&mutex);
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
