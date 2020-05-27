#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>





int counter[256];

sem_t * gettone;





void* thread_dispari(void *arg) {

	char *str = (char*) arg;

	int length = strlen(str);

	for (int i = 0; i < length-1; i=i+2) {

		int pos = str[i];

		if (sem_wait(gettone) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		counter[pos]++;

		if (sem_post(gettone) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}

	}

	return NULL;

}

void* thread_pari(void *arg) {

	char *str = (char*) arg;

	int length = strlen(str);

	for (int i = 1; i < length; i=i+2) {

		int pos = str[i];

		if (sem_wait(gettone) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		counter[pos]++;

		if (sem_post(gettone) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}

	}

	return NULL;

}

int main() {

	char *content =
			"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";

	pthread_t t1;
	pthread_t t2;

	void *res_t1;
	void *res_t2;

	int s;

	gettone = malloc(sizeof(sem_t));

	s = sem_init(gettone,
					0, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo (se mettiamo 0 che succede?)
				  );

	if (s != 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	s = pthread_create(&t1, NULL, thread_pari, content);

	if (s != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	s = pthread_create(&t2, NULL, thread_dispari, content);

	if (s != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	s = pthread_join(t1, &res_t1);

	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	printf("Join con thread_pari avvenuta con successo\n");

	s = pthread_join(t2, &res_t2);

	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	printf("Join con thread_dispari avvenuta con successo\n");

	int tot=0;

	for (int i = 0; i < 256; i++) {
		printf("frequenza %3d = %d\n", i, counter[i]);
		tot=tot+counter[i];
	}

	printf("in totale ho contato %d volte\n", tot);

	if ( (sem_destroy(gettone)) != 0 ) {
		perror("sem_destroy");
		exit(EXIT_FAILURE);
	}

	printf("bye\n");
//free(res_t1);

	return 0;
}
