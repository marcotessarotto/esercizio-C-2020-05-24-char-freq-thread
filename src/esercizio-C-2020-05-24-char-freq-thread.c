/*
 è data la string di caratteri content:

char * content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";

il programma utilizza due thread aggiuntivi, il primo thread analizza i caratteri nelle posizioni "pari" della stringa (content[0], content[2], content[4]...) mentre il secondo considera i caratteri nelle posizioni "dispari" della stringa (content[1], content[3], content[5]...)

definire un array condiviso:

int counter[256];

ciascun thread incrementa il contatore (counter) corrispondente al valore di ogni carattere incontrato:

counter[content[i]]++;

usare un solo semaforo per risolvere il problema (ci sono più modi di risolverlo...)

il thread principale aspetta il termine dei due thread e poi scrive su stdout la frequenza di ogni carattere.

"frequenza %d = %d\n"
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
#include <semaphore.h>
#include <pthread.h>

sem_t * process_semaphore;
int counter[256]; // variabile condivisa tra i due thread

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }
#define NUMBER_OF_CYCLES 10000000

void * thread_function_1(void * arg) {

	char * text = (char *) arg;
	//thread1


	for (int i = 0; text[i] != '\0'; i++) {

		if(i%2==0)
		{
			if (sem_wait(process_semaphore) == -1) {
						perror("sem_wait");
						exit(EXIT_FAILURE);
					}

			counter[(int)text[i]]++;

			if (sem_post(process_semaphore) == -1) {
						perror("sem_post");
						exit(EXIT_FAILURE);
					}

		}

	}

	return NULL;
}


void * thread_function_2(void * arg) {

	char * text = (char *) arg;
	//thread2


	for (int i = 0; text[i] != '\0'; i++) {

		if(i%2!=0)
		{

			if (sem_wait(process_semaphore) == -1) {
									perror("sem_wait");
									exit(EXIT_FAILURE);
								}

						counter[(int)text[i]]++;

						if (sem_post(process_semaphore) == -1) {
									perror("sem_post");
									exit(EXIT_FAILURE);
								}



		}


		// 3.4.2 Mutual exclusion solution, pag. 19
		if (sem_wait(process_semaphore) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}


		if (sem_post(process_semaphore) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}

	}

	return NULL;
}

int main(int argc, char * argv[]) {

	pthread_t t1;
	pthread_t t2;

//	char * content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";
 char * content ="abcdefghilmnopqrstuvz123!";

	void * res;

	int s;
	process_semaphore = malloc(sizeof(sem_t));

	s = sem_init(process_semaphore,
					0, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo (se mettiamo 0 che succede?)
				  );

	CHECK_ERR(s,"sem_init")

	s = pthread_create(&t1, NULL, thread_function_1, content);

	if (s != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	s = pthread_create(&t2, NULL, thread_function_2, content);

	if (s != 0) {
		perror("pthread_create");
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


	printf("The shared arrays has the values:\n");
	for(int i=0; i<256; i++)
	printf("frequenza %d = %d\n",i,counter[i]);

	// il semaforo senza nome va distrutto solo quando non ci sono processi bloccati su di esso
	s = sem_destroy(process_semaphore);
	CHECK_ERR(s,"sem_destroy")

	printf("bye\n");

	return 0;
}

