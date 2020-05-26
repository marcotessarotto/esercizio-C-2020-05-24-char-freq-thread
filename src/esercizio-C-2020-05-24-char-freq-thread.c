#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define N	2

int counter[256];
sem_t sem;

void err_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void *analyze_even(void *arg)
{
	char *str = (char *) arg;
	for (int i = 0; i < strlen(arg); i += 2) {
		if (sem_wait(&sem))
			err_exit("sem_wait() error");
		counter[(int) str[i]]++;
		if (sem_post(&sem))
			err_exit("sem_post() error");
	}
	return NULL;
}

void *analyze_odd(void *arg)
{
	char *str = (char *) arg;
	for (int i = 1; i < strlen(arg); i += 2) {
		if (sem_wait(&sem))
			err_exit("sem_wait() error");
		counter[(int) str[i]]++;
		if (sem_post(&sem))
			err_exit("sem_post() error");
	}
	return NULL;
}

int main()
{
	char *content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
			"Mattis rhoncus urna neque viverra justo nec ultrices. Pretium quam vulputate dignissim suspendisse in est ante. "
			"Vitae congue mauris rhoncus aenean. Blandit cursus risus at ultrices mi. Ut lectus arcu bibendum at varius vel pharetra vel. "
			"Etiam non quam lacus suspendisse faucibus interdum posuere. Eget sit amet tellus cras adipiscing enim eu turpis egestas. "
			"Lectus magna fringilla urna porttitor rhoncus dolor purus non. Sit amet consectetur adipiscing elit duis tristique sollicitudin nibh. "
			"Nec tincidunt praesent semper feugiat nibh. Sapien pellentesque habitant morbi tristique senectus et netus et malesuada.";

	if (sem_init(&sem, 0, 1))
		err_exit("sem_open() error");

	pthread_t threads[N];
	void *(*thread_funcs[N])(void *arg);
	thread_funcs[0] = analyze_even;
	thread_funcs[1] = analyze_odd;
	for (int i = 0; i < N; i++) {
		if (pthread_create(&threads[i], NULL, &(*thread_funcs[i]), content))
			err_exit("pthread_create() error");
	}

	for (int i = 0; i < N; i++) {
		if (pthread_join(threads[i], NULL))
			err_exit("pthread_join() error");
	}

	for (int i = 0; i < 256; i++)
		printf("frequenza di %d = %d \n", i, counter[i]);
	return 0;
}
