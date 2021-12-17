#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

enum sems
{
	STAIR_DOWN = 0,
    START_OF_TRIP,
    NUM_ON_BOAT,
    NUM_ON_STAIR,
    END_OF_TRIP,
    NUM_ON_COAST,
    LAST_TRIP
};

const int nsem = 7;

void Z(int sem_key, enum sems num_of_sem)
{
	struct sembuf sem_op = {num_of_sem, 0, 0};
	if(semop(sem_key, &sem_op, 1) < 0)
	{
		perror("Error in Z");
		exit(-1);
	}
}

void V(int sem, enum sems num_of_sem, int n)
{
	struct sembuf sem_op = {num_of_sem, n, 0};
	if(semop(sem, &sem_op, 1) < 0)
	{
		perror("Error in V");
		exit(-1);
	}
}

void P(int sem, enum sems num_of_sem, int n)
{
	struct sembuf sem_op = {num_of_sem, -n, 0};
	if(semop(sem, &sem_op, 1) < 0)
	{
		perror("Error in P");
		exit(-1);
	}
}

void captain(int sem_key, int ntrips, int nstair, int nseat)
{
	P(sem_key, STAIR_DOWN, 1);
	printf("Stair is down\n");
	for (int i = 0; i < ntrips; i++)
	{
		Z(sem_key, NUM_ON_BOAT);
		V(sem_key, NUM_ON_BOAT, nseat);

		V(sem_key, STAIR_DOWN, 1);

		P(sem_key, START_OF_TRIP, 1);
		printf("\x1b[33m");
		printf("Start of the trip\n");
		printf("\x1b[0m");

		sleep(5);

		printf("\x1b[33m");
		printf("End of the trip\n");
		printf("\x1b[0m");
		V(sem_key, START_OF_TRIP, 1);
		P(sem_key, END_OF_TRIP, 1);

		P(sem_key, STAIR_DOWN, 1);
		Z(sem_key, NUM_ON_BOAT);
		V(sem_key, NUM_ON_BOAT, nseat);
		V(sem_key, NUM_ON_COAST, nseat);
		if(i == ntrips - 1)
		{
			V(sem_key, LAST_TRIP, 1);
			break;
		}

		printf("\x1b[35m");
		printf("Another group can come to the boat\n");
		printf("\x1b[0m");
		V(sem_key, END_OF_TRIP, 1);
	}
	V(sem_key, NUM_ON_COAST, nseat);
	printf("\x1b[32m");
	printf("Captain go home\n");
	printf("\x1b[0m");
}

void passanger(int sem_key, int num_of_pass)
{
	Z(sem_key, STAIR_DOWN);
	printf("Passanger %d - ready\n", num_of_pass);

	P(sem_key, NUM_ON_COAST, 1);

	if(semctl(sem_key, LAST_TRIP, GETVAL))
	{
		V(sem_key, NUM_ON_COAST, 1);
		exit(0);
	}

	P(sem_key, NUM_ON_STAIR, 1);
	printf("Passanger %d on the stairs go to the boat\n", num_of_pass);
	sleep(2);
	printf("Passanger %d on the boat\n", num_of_pass);
	V(sem_key, NUM_ON_STAIR, 1);

	P(sem_key, NUM_ON_BOAT, 1);

	Z(sem_key, START_OF_TRIP);
	Z(sem_key, END_OF_TRIP);
	Z(sem_key, STAIR_DOWN);

	P(sem_key, NUM_ON_STAIR, 1);
	printf("Passanger %d on the stairs go to the coast\n", num_of_pass);
	sleep(2);
	printf("Passanger %d on the coast\n", num_of_pass);
	V(sem_key, NUM_ON_STAIR, 1);

	P(sem_key, NUM_ON_BOAT, 1);
}

int main(int argc, char const *argv[])
{
	if(argc < 5)
	{
		write(2, "Too few arguments\n", strlen("Too few arguments\n"));
		return -1;
	}
	int npass = atoi(argv[1]),
		ntrips = atoi(argv[2]),
		nseat = atoi(argv[3]),
		nstair = atoi(argv[4]);

	int sem_key = semget(IPC_PRIVATE, nsem, 0777);
	if(sem_key < 0)
	{
		perror("Error in semop");
		exit(-1);
	}

	V(sem_key, STAIR_DOWN, 1);
	V(sem_key, END_OF_TRIP, 1);
	V(sem_key, START_OF_TRIP, 1);
	V(sem_key, NUM_ON_STAIR, nstair);

	int nproc;

	if(nseat < npass)
	{
		nproc = nseat;	
	}
	else
	{
		nproc = npass;
	}

	V(sem_key, NUM_ON_BOAT, nproc);
	V(sem_key, NUM_ON_COAST, nproc);

	int status = 0;

	pid_t cap_pid = fork();
	if(cap_pid < 0)
	{
		perror("Error: No child process is created");
		exit(-1);
	}
	else if(cap_pid == 0)
	{
		captain(sem_key, ntrips, nstair, nproc);
		return 0;
	}

	for(int i = 1; i <= npass; i++)
	{
		pid_t pas_pid = fork();
		if(cap_pid < 0)
		{
			perror("Error: No child process is created");
			exit(-1);
		}
		else if(pas_pid == 0)
		{
			passanger(sem_key, i);
			return 0;
		}
	}

	for (int i = 0; i < npass; i++)
	{
		wait(&status);
	}

	return 0;
}