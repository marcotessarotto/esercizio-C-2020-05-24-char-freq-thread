#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }
#define CHECK_ERR2(a,msg) {if ((a) != 0) { perror((msg)); exit(EXIT_FAILURE); } }


void * pari();
void * dispari();

char * content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";
int counter[256];

sem_t mutex;

int main() {
	int s;
	pthread_t thread_pari;
	pthread_t thread_dispari;

	s = sem_init(&mutex,
			0, // 1 => il semaforo è condiviso tra processi,
			   // 0 => il semaforo è condiviso tra threads del processo
			1 // valore iniziale del semaforo
	  );
	CHECK_ERR(s,"sem_init")



	s = pthread_create(&thread_pari, NULL, pari, NULL);
		CHECK_ERR2(s,"pthread_create")

	s = pthread_create(&thread_dispari, NULL, dispari, NULL);
		CHECK_ERR2(s,"pthread_create")

	s = pthread_join(thread_pari, NULL);
			CHECK_ERR2(s,"pthread_join")

	s = pthread_join(thread_dispari, NULL);
			CHECK_ERR2(s,"pthread_join")

	for(int i=0; i<256; i++){
		printf("frequenza %d = %d\n", i, counter[i]);
	}
	return 0;
}

void * pari(){

	int carattere;

	int length = strlen(content);


	for(int i=0; i<length; i=i+2){

		carattere = content[i];


		if (sem_wait(&mutex) == -1) {
					perror("sem_wait");
					exit(EXIT_FAILURE);
		}

		(counter[carattere])++;

		if (sem_post(&mutex) == -1) {
				perror("sem_post");
				exit(EXIT_FAILURE);
		}

	}

	return NULL;
}

void * dispari(){

	int carattere;
	int length = strlen(content);

	for(int i=1; i<length; i=i+2){

		carattere = content[i];

		if (sem_wait(&mutex) == -1) {
					perror("sem_wait");
					exit(EXIT_FAILURE);
				}

		(counter[carattere])++;

		if (sem_post(&mutex) == -1) {
				perror("sem_post");
				exit(EXIT_FAILURE);
			}

	}

	return NULL;
}
