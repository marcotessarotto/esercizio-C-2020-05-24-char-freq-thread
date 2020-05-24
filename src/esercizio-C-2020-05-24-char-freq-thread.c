#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

void * thread1(char * content);
void * thread2(char * content);

sem_t * semaphore;
int counter[256] = {0};

int main() {

	char * content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";

	pthread_t threads[2];

	semaphore = malloc(sizeof(sem_t));
	if(semaphore == NULL){
		perror("malloc()\n");
		exit(1);
	}


	int res = sem_init(semaphore,
					1, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo (se mettiamo 0 che succede?)
				  );
	if (res != 0){
		perror("sem_init()\n");
		exit(1);
	}


	if(pthread_create(&threads[0], NULL, thread1, content) != 0){
		perror("pthread_create()\n");
		exit(1);
	}
	if(pthread_create(&threads[1], NULL, thread2, content) != 0){
		perror("pthread_create()\n");
		exit(1);
	}

	for(int i=0 ; i<2 ; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("pthread_join()\n");
			exit(1);
		}
	}

	for(int i=0 ; i<255 ; i++){
		printf("frequenza %d = %d\n", i, counter[i]);
	}


	exit(0);
}


void * thread1(char * content){

	for(int i=0 ; i<strlen(content) ; i=i+2){
		if (sem_wait(semaphore) == -1) {
			perror("sem_wait()\n");
			exit(EXIT_FAILURE);
		}

		counter[content[i]]++;

		if (sem_post(semaphore) == -1) {
			perror("sem_post()\n");
			exit(EXIT_FAILURE);
		}

	}

	return NULL;
}

void * thread2(char * content){
	for(int i=1 ; i<strlen(content) ; i=i+2){
		if (sem_wait(semaphore) == -1) {
			perror("sem_wait()\n");
			exit(EXIT_FAILURE);
		}

		counter[content[i]]++;

		if (sem_post(semaphore) == -1) {
			perror("sem_post()\n");
			exit(EXIT_FAILURE);
		}

	}

	return NULL;
}
