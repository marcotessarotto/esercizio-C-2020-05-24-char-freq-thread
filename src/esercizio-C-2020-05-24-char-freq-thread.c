/*
 * esercizio-C-2020-05-24-char-freq-thread.c
 *
 *è data la string di caratteri content:

char * content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";

il programma utilizza due thread aggiuntivi, il primo thread analizza i caratteri nelle
posizioni "pari" della stringa (content[0], content[2], content[4]...) mentre il secondo
considera i caratteri nelle posizioni "dispari" della stringa (content[1], content[3],
content[5]...)

definire un array condiviso:

int counter[256];

ciascun thread incrementa il contatore (counter) corrispondente al valore di ogni carattere
incontrato:

counter[content[i]]++;

usare un solo semaforo per risolvere il problema (ci sono più modi di risolverlo...)

il thread principale aspetta il termine dei due thread e poi scrive su stdout la frequenza
di ogni carattere.

"frequenza %d = %d\n"
 *  Created on: May 22, 2020
 *      Author: marco
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t * thread_sem;

int shared_counter; // variabile condivisa tra i due thread
int counter[256]; // variabile condivisa tra i due thread

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }

void* thread_pari(void* arg){
	char* str = (char*) arg;
	int i = 0;

	while(*(str+i) != '\0'){
		// 3.4.2 Mutual exclusion solution, pag. 19
		if (sem_wait(thread_sem) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		counter[(int)*(str+i)]++;
		i = i+2;

		if (sem_post(thread_sem) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}

	return NULL;
}

void* thread_dispari(void* arg){
	char* str = (char*) arg;
	int i = 1;

	while(*(str+i) != '\0'){
		// 3.4.2 Mutual exclusion solution, pag. 19
		if (sem_wait(thread_sem) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		counter[(int)*(str+i)]++;
		i = i+2;

		if (sem_post(thread_sem) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}

	return NULL;
}

int main() {

	int s;
	void * res;
	char * content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";

	pthread_t t1;
	pthread_t t2;

	thread_sem = malloc(sizeof(sem_t));
	s = sem_init(thread_sem,
					0, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo (se mettiamo 0 che succede?)
				  );

	CHECK_ERR(s,"sem_init")



	s = pthread_create(&t1, NULL, thread_pari, content);

	if (s != 0) {
		perror("pthread_create vocali");
		exit(EXIT_FAILURE);
	}

	s = pthread_create(&t2, NULL, thread_dispari, content);

	if (s != 0) {
		perror("pthread_create consonanti");
		exit(EXIT_FAILURE);
	}


	s = pthread_join(t1, &res);

	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	s = pthread_join(t2, &res);

	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	// STAMPA SU STDOUT
	int total = 0;
	for (int i = 0; i < 256; i++){
		// "frequenza %d = %d\n"
		printf("frequenza %d = %d\n",i, counter[i]);
		total = total +counter[i];
	}
	printf("total number of characters %d \n: ", total);
	printf("strlen %ld\n", strlen(content));
	printf("bye!\n");
	return 0;
}
