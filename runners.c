#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#define COME (nrun+2)
#define END (nrun+3)

struct msgbuf
{
	long type;
};

double print_time(struct timeval start, struct timeval end)
{
	double sec = end.tv_sec - start.tv_sec;
	double usec = end.tv_usec - start.tv_usec;

	double time = (sec*1000 + usec/1000) / 1000;
	return time;
}

void receive(int key, int type, struct msgbuf msg)
{
	if(msgrcv(key, &msg, sizeof(msg), type, 0) < 0)
	{
		perror("Error in msgrcv");
		exit(1);
	}
}

void send(int key, struct msgbuf msg)
{
	if(msgsnd(key, &msg, sizeof(msg), 0) < 0)
	{
		perror("Error in msgsnd");
		exit(1);
	}
}

void judge(int key, int nrun)
{
	struct msgbuf msg;
	int judge = nrun + 1;

	printf("Judge come\n");
	for(int i = 1; i <= nrun; i++)
	{
		receive(key, COME, msg);
	}

	struct timeval start, end;
	start.tv_sec = 0;
	start.tv_usec = 0;
	end.tv_sec = 0;
	end.tv_usec = 0;

	printf("Start of run\n");
	msg.type = 1;
	send(key, msg);

	if(gettimeofday(&start, NULL) != 0)
	{
		perror("Error in gettimeofday(start)");
	}

	printf("Judge wait for the end of the run\n");
	receive(key, judge, msg);

	if(gettimeofday(&end, NULL) != 0)
	{
		perror("Error in gettimeofday(end)");
	}
	printf("Race is over in %f\n", print_time(start, end));

	for(int i = 1; i <= nrun; i++)
	{
		msg.type = END;
		send(key, msg);
	}
	printf("Judge go home\n");

}

void runner(int key, int nrun, int runner)
{
	struct msgbuf msg;

	msg.type = COME;
	printf("Runner %d came to stadium\n", runner);
	send(key, msg);

	receive(key, runner, msg);

	printf("Start of runner %d\n", runner);

	msg.type = runner + 1;
	send(key, msg);
	printf("Runner %d end the run\n", runner);

	receive(key, END, msg);
	printf("Runner %d go home\n", runner);
}

int main(int argc, char const *argv[])
{
	if(argc < 2)
	{
		write(2, "Too few arguments\n", strlen("Too few arguments\n"));
		return -1;
	}

	int nrun = atoi(argv[1]);

	int key = msgget(IPC_PRIVATE, 0777);
	if(key < 0)
	{
		perror("Error in getting key");
		return -1;
	}

	int status = 0;
	pid_t j_pid = fork();
	if(j_pid < 0)
	{
		perror("Error: No child process is created");
		return -1;
	}
	if(j_pid == 0)
	{
		judge(key, nrun);
		return 0;
	}

	for (int i = 1; i <= nrun; ++i)
	{
		pid_t r_pid = fork();
		if(r_pid < 0)
		{
			perror("Error: No child process is created");
			return -1;
		}
		else if(r_pid == 0)
		{
			runner(key, nrun, i);
			return 0;
		}
	}

	for (int i = 1; i <= nrun; ++i)
	{
		wait(&status);
	}
	msgctl(key, IPC_RMID, NULL);
	
	return 0;
}