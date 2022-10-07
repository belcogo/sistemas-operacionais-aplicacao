#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>
#include <signal.h>
#include "Queue.h"

// config
#define ECHOMAX 250

// styling
#define DEFAULT_SEPARATOR "________________________________"
#define DETAIL_SEPARATOR "--------------------------------"
#define ESPECIAL_SEPARATOR "********************************"

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

// product attributes structure
typedef struct product {
  char type;
  short int price_range;
  short int quality;
  int produce_time;
  int consume_time;
} product_t;

// def queue
Queue products;

// def product to be used in peek and dequeue
product_t prd_current;

// controllers for production and consuming numbers
int controller[3][3][3] = {0};
int consumed_items_count = 0;
int products_count = 0;
int production_time = 0;

// array of types to be used by the producers and consumers
char types[N] = {A, B, C, A, B};

// array of prices to be used by the producers and consumers
int prices[3] = {GOLD, SILVER, BRONZE};

// array of qualities to be used by the producers and consumers
int qualities[3] = {HIGH, MEDIUM, LOW};

// def semaphores
sem_t mutex;

// def array of states for the production line control
int state[N];

// threads CONTROL
pthread_t threads[N];
int result;
void *thread_result;

// functions signatures
void *producer(int);
void *consumer(int);
int generate_random_number(int);
int check_status(int);
void show_statistics();
void print_product(product_t *product);
void end_production_line();

int main() {
	struct sigaction act;
	act.sa_handler = &end_production_line;
	sigaction(SIGINT, &act, NULL);
	srand(time(NULL));

	// initializing queue
	queueInit(&products, sizeof(int));

	// initializing semaphores
	result = sem_init(&mutex, 0, 1);
	if (result != 0) {
		perror("init mutex");
		exit(EXIT_FAILURE);
	}

  // initializing states
	for (int i = 0; i < N; i++) {
		state[i] = ON_HOLD;
	}

	int loop_control = 0;

  printf("Abrindo a fábrica...\nCaso queira encerrar o expediente pressione ctrl+c\n\nIniciando produção...\n\n");

  // create threads
	for (int i = 0; i < N; i++) {
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

  // join threads
	for (int i = 0; i < N; i++) {
		result = pthread_join(threads[i], &thread_result);

		if (result != 0) {
			perror("Something happened during thread join");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}


void *producer(int i) {
	// define producer type
	char type = types[i];

	while (1) {
		if (getQueueSize(&products) > 0) {
			sleep(1);
		}

		int loop_control = 0;

		// define number of products to be produced
		int number_of_products = generate_random_number(NULL);

		while (loop_control < number_of_products) {
      // get random option for price and quality
			int price_idx = generate_random_number(3);
			int quality_idx = generate_random_number(3);

      // create product object
			product_t new_product;
			new_product.type = type;
			new_product.price_range = prices[price_idx];
			new_product.quality = qualities[quality_idx];
			new_product.produce_time = generate_random_number(NULL);
			new_product.consume_time = new_product.produce_time * 0.8;

      production_time+= new_product.produce_time;
      // production_time+= new_product.consume_time;

			// verify if there is any producer or consumer already active
			int result = check_status(i);

			// keep in wait loop until there is no producer/consumer active
			while (result >= 0) {
				// wait post from active producer/consumer
				sem_wait(&mutex);
				result = check_status(i);
			}

			// wait time defined for product production
			sleep(new_product.produce_time);

			// add to queue
			enqueue(&products, &new_product);

			// post semaphore to free critical section
  			sem_post(&mutex);

			printf("\n\nPRODUTO PRODUZIDO:\n");
			print_product((product_t *)&new_product);

			// add one unity to specific type - quality - price range
			controller[i][quality_idx][price_idx] += 1;

			// set state to on hold to signal that is not active anymore
			state[i] = ON_HOLD;
			loop_control++;	
		}
	}
}

void *consumer(int i) {
	// define consumer type
	char type = types[i];

	while(1) {
		// wait liberation for critical section
		sem_wait(&mutex);
		
		// verify if there is any producer or consumer already active
		int result = check_status(i);

		// keep in wait loop until there is no producer/consumer active
		while (result >= 0) {
			// wait post from active producer/consumer
			sem_wait(&mutex);
			result = check_status(i);
		}

		// keep consuming while there is products in queue
		while (getQueueSize(&products) > 0) {
			queuePeek(&products, &prd_current);

			// if type from first product in queue is not equal to consumer type breaks loop
			if ((prd_current.type == A && type != A) || (prd_current.type != A && type == A)) break;

			// wait defined consuming type for product
			sleep(prd_current.consume_time);

			// remove from queue
			dequeue(&products, &prd_current);

			printf("\nPRODUTO CONSUMIDO:\n");
			print_product((product_t *)&prd_current);
		}

		state[i] = ON_HOLD;

		// post semaphore to free critical section
		sem_post(&mutex);
	}
}

int generate_random_number(int factor) {
	if (factor == NULL) factor = 10;

  int time_to_wait;
  time_to_wait = random() % factor;

  return time_to_wait;
}

int check_status(int idx) {
  int ok = -1;

	// verify if there is any producer/consumer active
  for (int j = 0; j < N; j++) {
    if (j != idx && state[j] != ON_HOLD) {
      ok = j;
      break;
    }
  }

	// if no other producer/consumer is active update current producer/consumer's state
  if (ok == -1) {
    if (idx < 3) state[idx] = PRODUCING;
    if (idx > 2) state[idx] = CONSUMING;
  }
  
  return ok;
}

float calc_media(int value) {
	return (float)(100 * value) / products_count;
}

void show_statistics() {
	int sum_a = 0;
	int sum_b = 0;
	int sum_c = 0;

	int sum_high = 0;
	int sum_medium = 0;
	int sum_low = 0;

	int sum_gold = 0;
	int sum_silver = 0;
	int sum_bronze = 0;

  // iterate over types
	for (int type = 0; type < 3; type++) {
    // iterate over quality options
    for (int quality = 0; quality < 3; quality++) {
      // iterate over price range options
      for (int price = 0; price < 3; price++) {
				int current_count = controller[type][quality][price];

				char type_value = types[type];
				int quality_value = qualities[quality];
				int price_value = prices[price];

        // calculates total for each type
				switch(type_value) {
					case A: sum_a += current_count; break;
					case B: sum_b += current_count; break;
					case C: sum_c += current_count; break;
				}

        // calculates total for each quality option
				switch(quality_value) {
					case HIGH: sum_high += current_count; break;
					case MEDIUM: sum_medium += current_count; break;
					case LOW: sum_low += current_count; break;
				}

        // calculates total for each price range option
				switch(price_value) {
					case GOLD: sum_gold += current_count; break;
					case SILVER: sum_silver += current_count; break;
					case BRONZE: sum_bronze += current_count; break;
				}
        
        // update total items count
				products_count += current_count;
      }
    }
  }

  // get consumed items count
	consumed_items_count = products_count - getQueueSize(&products);
  float media_production_time = production_time / products_count;
  printf("\n\n%s%s\n", DEFAULT_SEPARATOR, DEFAULT_SEPARATOR);
  printf("%s%s\n", ESPECIAL_SEPARATOR, ESPECIAL_SEPARATOR);
  printf("%-20s%-30s\n", "TOTAL PRODUZIDO", "TEMPO MÉDIO PRODUCAO (seg)");
  printf("%-20d%-30.2f\n", products_count, media_production_time);
  printf("%s%s\n", DETAIL_SEPARATOR, DETAIL_SEPARATOR);
  printf("%-20s%-20s%-20s\n", "TIPO A(%)", "TIPO B(%)", "TIPO C(%)");
  printf("%-20d%-20.2f%-20.2f\n", calc_media(sum_a), calc_media(sum_b), calc_media(sum_c));
  printf("%s%s\n", DETAIL_SEPARATOR, DETAIL_SEPARATOR);
  printf("%-20s%-20s%-20s\n", "QUALI. ALTA(%)", "QUALI. MEDIA(%)", "QUALI. BAIXA(%)");
  printf("%-20d%-20.2f%-20.2f\n", calc_media(sum_high), calc_media(sum_medium), calc_media(sum_low));
  printf("%s%s\n", DETAIL_SEPARATOR, DETAIL_SEPARATOR);
  printf("%-20s%-20s%-20s\n", "GOLD(%)", "SILVER(%)", "BRONZE(%)");
  printf("%-20d%-20.2f%-20.2f\n", calc_media(sum_gold), calc_media(sum_silver), calc_media(sum_bronze));
  printf("%s%s\n", DEFAULT_SEPARATOR, DEFAULT_SEPARATOR);
}



void end_production_line() {
	printf("\nFechando fábrica...\n");

	for(int i = 0; i < 3; i++) {
			pthread_cancel(threads[i]);
	}

	if (getQueueSize(&products) > 0) {
		state[0] = ON_HOLD;
		state[1] = ON_HOLD;
		state[2] = ON_HOLD;
	}

	while(getQueueSize(&products) > 0) {
		sem_post(&mutex);
	}

  sem_wait(&mutex);

	for(int i = 3; i < N; i++) {
			pthread_cancel(threads[i]);
	}

	show_statistics();

	exit(0);
}

void print_product(product_t *product) {	
  char value_quality[ECHOMAX];
  char value_price[ECHOMAX];

  switch(product->quality) {
    case HIGH: strcpy(value_quality, "Alta"); break;
    case MEDIUM: strcpy(value_quality, "Média"); break;
    case LOW: strcpy(value_quality, "Alta"); break;
  }

  switch(product->price_range) {
    case GOLD: strcpy(value_price, "R$ Gold"); break;
    case SILVER: strcpy(value_price, "R$ Silver"); break;
    case BRONZE: strcpy(value_price, "R$ Bronze"); break;
  }
  printf("%s\n", DEFAULT_SEPARATOR);
  printf("%-6s%-12s%-14s\n", "TIPO", "QUALIDADE", "FAIXA DE PREÇO"); 
  printf("%s\n", DETAIL_SEPARATOR);
  printf("%-6c%-12s%-14s\n", product->type, value_quality, value_price);
  printf("%s\n", DEFAULT_SEPARATOR);
}
