#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wait.h>

void print_time(struct timeval start, struct timeval end)
{
	double sec = end.tv_sec - start.tv_sec;
	double usec = end.tv_usec - start.tv_usec;

	double time = (sec*1000 + usec/1000) / 1000;
	printf("Time is : %f\n", time);
}

int MyExec(char ** argv)
{
	int status = 0;

	struct timeval start, end;
	start.tv_sec = 0;
	start.tv_usec = 0;
	end.tv_sec = 0;
	end.tv_usec = 0;

	pid_t pid = fork();

	if(gettimeofday(&start, NULL) != 0)
		perror("Error in gettimeofday(start)");

	if(pid < 0)
	{
		printf("Error: No child process is created");
	}
	else if(pid == 0)
	{
		execvp(argv[1], argv + 1);
	}
	else
	{
		wait(&status);
	}


	if(gettimeofday(&end, NULL) != 0)
		perror("Error in gettimeofday(end)");

	print_time(start, end);
}

int main(int argc, char *argv[])
{
	MyExec(argv);
	return 0;
}
