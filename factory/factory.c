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
  int produce_time;
  int consume_time;
} product_t;

// struct for queue
typedef struct node {
    product_t *prd;
    struct node *next;
} node_t;

// iterator
int i;

// types array
char types[N] = {A, B, C, A, B};

// semaphores
sem_t mutex;
sem_t semaphores[N];

// state array
int state[N];

// product queue
node_t *head = NULL;
// product number
int number_of_products = 0;

void *producer(int);
void *consumer(int);
int generate_random_number();
void enqueue(node_t **head, product_t *node_value);
product_t *dequeue(node_t **head);
int check_status(int idx);

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
    printf("%d", i);
    state[i] = ON_HOLD;
    result = sem_init(&semaphores[i], 0, 0);
    if (result != 0) {
      perror("init sem");
      exit(EXIT_FAILURE);
    }
  }
  int a = 0;

  while (a < 1) {
    for (i = 0; i < N; i++) {
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

    for (i = 0; i < N; i++) {
      result = pthread_join(threads[i], &thread_result);

      if (result != 0) {
        perror("Something happened during thread join");
        exit(EXIT_FAILURE);
      }
    }
  }

  return 0;
}

void *producer(int i) {
  sem_wait(&mutex);
  int result = check_status(i);
  while (result >= 0) {
    printf("waiting %d\n", result);
    sem_wait(&semaphores[result]);
    result = check_status(i);
  }
  char type = types[i];
  printf("INÍCIO PRODUTOR - TIPO %c\n", type);
  int aaa = 0;
  int number_of_products = 1;

  while (aaa < number_of_products) {
    printf("produto nro %d\n", aaa);
    int times[2] = {0, 0};
    product_t new_product;
    new_product.type = type;
    new_product.price_range = GOLD;
    new_product.quality = HIGH;
    while(1) {
      if (times[0] == times[1]) {
        times[0] = generate_random_number();
        times[1] = generate_random_number();
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
    sleep(new_product.produce_time);
    enqueue(&head, (product_t *)&new_product);
    state[i] = ON_HOLD;
    if (number_of_products == 1) {
      sem_post(&semaphores[i]);
    }
    aaa++;
  }
  int times[2] = {0, 0};
  product_t new_product;
  new_product.type = type;
  new_product.price_range = GOLD;
  new_product.quality = HIGH;
  while(1) {
    if (times[0] == times[1]) {
      times[0] = generate_random_number();
      times[1] = generate_random_number();
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
  sleep(new_product.produce_time);
  enqueue(&head, (product_t *)&new_product);
  state[i] = ON_HOLD;
  if (number_of_products == 1) {
    sem_post(&semaphores[i]);
  }
  printf("FIM PRODUTOR - TIPO %c\n", type);
  sem_post(&mutex);
}

void *consumer(int i) {
  sem_wait(&mutex);
  char type = types[i];
  int result = check_status(i);
  while (result >= 0) {
    printf("consumidor tipo %c - waiting %d\n", type, result);
    sem_wait(&semaphores[result]);
    result = check_status(i);
  }
  printf("INÍCIO CONSUMIDOR - TIPO %c\n", type);

  char current_product_type = head->prd->type;
  printf("type C %c - type P %c\n", type, current_product_type);
  if ((type == A && current_product_type == A) || (type != A && (current_product_type == B || current_product_type == C))) {
    sleep(head->prd->consume_time);
    product_t *dequeued_product = dequeue(&head);
    printf("%s - %d", dequeued_product->type, dequeued_product->price_range);
  }
  state[i] = ON_HOLD;
  sem_post(&semaphores[i]);
  sem_post(&mutex);
  printf("FIM CONSUMIDOR - TIPO %c\n", type);
}

int generate_random_number() {
  int time_to_wait;
  time_to_wait = random() % 10;
  return time_to_wait;
}

// queue source code: https://gist.github.com/kroggen/5fc7380d30615b2e70fcf9c7b69997b6

void enqueue(node_t **head, product_t *node_value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->prd = node_value;
    new_node->next = *head;
    number_of_products++;
    *head = new_node;
}

product_t *dequeue(node_t **head) {
    node_t *current, *prev = NULL;
    product_t *retval;

    if (*head == NULL) return NULL;

    current = *head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->prd;
    free(current);
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    number_of_products--;
    return retval;
}

int check_status(int idx) {
  int ok = -1;
  for (int j = 0; j < N; j++) {
    if (j != idx && state[j] != ON_HOLD) {
      ok = j;
      break;
    }
  }

  if (ok == -1) {
    if (idx < 3) state[idx] = PRODUCING;
    if (idx > 2) state[idx] = CONSUMING;
  }

  
  return ok;
}
