#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define BIGNUM 1000

int mysleepsort(int n_proc, int* num)
{
	int status = 0;
	for (int i = 0; i < n_proc; ++i)
	{
		pid_t pid = fork();
		if(pid < 0)
		{
			perror("Child process didn't create");
		}
		else if(pid == 0)
		{
			usleep(2000*num[i]);
			printf("%d ", num[i]);
			return 0;
		}
	}

	for (int i = 0; i < n_proc; ++i)
	{
		wait(&status);
	}
	printf("\n");
	return 0;
}

int main(int argc, char const *argv[])
{
	if(argc < 2)
	{
		write(2, "Too few arguments", strlen("Too few arguments"));
	}
	int num[BIGNUM];
	for (int i = 0; i < argc - 1; ++i)
	{
		num[i] = strtol(argv[i + 1], (char**)NULL, 10);
	}

	mysleepsort(argc - 1, num);
	return 0;
}