#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>

#include <sys/wait.h>
#include <wait.h>
#include <ctype.h>
#define BIGNUM 1000

struct counts
{
	int bytes;
	int words;
	int lines;
};

void print_time(struct timeval start, struct timeval end)
{
	double sec = end.tv_sec - start.tv_sec;
	double usec = end.tv_usec - start.tv_usec;

	double time = (sec*1000 + usec/1000) / 1000;
	printf("Time is : %f\n", time);
}

int counterlines(char* buf, int nsymbols)
{
	int counter = 0;
	for (int i = 0; i < nsymbols; ++i)
	{
		if(buf[i] == '\n')
		{
			counter += 1;
		}
	}
	return counter;
}

int counterwords(char* buf, int nsymbols)
{
	int counter = 0, status = 0;
	for (int i = 0; i < nsymbols; ++i)
	{
		if(!isspace(buf[i]) && !status)
		{
			counter++;
			status = 1;
		}
		else if(isspace(buf[i]))
		{
			status = 0;
		}
	}
	return counter;
}

int mywrite(int fd, char* buf, int nsymbols)
{
	int output_sym = 0;
	do{
		int wsym = write(fd, buf, nsymbols - output_sym);
		if(wsym < 0)
		{
			perror("Error in write");
			return -1;
		}
		buf += wsym;
		output_sym += wsym;
	}while(nsymbols - output_sym != 0);
	return 0;
}

int counter(int fd)
{
	struct counts counts;
	counts.bytes = 0;
	counts.words = 0;
	counts.lines = 0;

	int nsymbols = 0;

	do{
		char buf[BIGNUM];
		nsymbols = read(fd, buf, BIGNUM);
		mywrite(1, buf, nsymbols);
		counts.bytes += nsymbols;
		counts.words += counterwords(buf, nsymbols);
		counts.lines += counterlines(buf, nsymbols);
	}while(nsymbols > 0);

	fprintf(stderr, "Num of bytes = %d\n", counts.bytes);
	fprintf(stderr, "Num of lines = %d\n", counts.lines);
	fprintf(stderr, "Num of words = %d\n", counts.words);
	return 0;
}

int MyExec(char ** argv)
{
	int fd[2];
	pipe(fd);

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
		dup2(fd[1], 1);
		execvp(argv[1], argv + 1);
	}
	else
	{
		close(fd[1]);
		counter(fd[0]);
		close(fd[0]);
		wait(&status);
	}

	if(gettimeofday(&end, NULL) != 0)
		perror("Error in gettimeofday(end)");

	print_time(start, end);
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		write(2, "Too few arguments", strlen("Too few arguments"));
	}
	MyExec(argv);
	return 0;
}
