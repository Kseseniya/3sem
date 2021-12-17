#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int childcreat(int n_proc)
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
			printf("Parent id: %d, child id: %d\n", (int)getppid(), (int)getpid());
			return 0;	
		}
		else
		{
			wait(&status);
		}
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	if(argc < 2)
	{
		write(2, "Too few arguments\n", strlen("Too few arguments\n"));
		return -1;
	}
	if(argc > 2)
	{
		write(2, "Too many arguments\n", strlen("Too many arguments\n"));
		return -1;
	}

	int n_proc = strtol(argv[1], (char**)NULL, 10);
	childcreat(n_proc);

	return 0;
}
