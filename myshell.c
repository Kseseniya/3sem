#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wait.h>
#include <pwd.h>
#include <ctype.h>
#define BUF_SIZE 1000


void display()
{
	uid_t uid = getuid();
	struct passwd* pwd = getpwuid(uid);
	printf("\x1b[1;32m");
	printf("%s:",pwd->pw_name);
	
	char cwd[BUF_SIZE];
	getcwd(cwd, BUF_SIZE);
	printf("\x1b[34m");
	printf("%s", cwd);
	printf("\x1b[0m");

	printf("$ ");

}

int counter(char* line)
{
	char* ptr = line;
	int count = 1;
	while(ptr = strchr(ptr, '|'))
	{
		ptr++;
		count++;
	}
	return count;
}

char** separ(char* line)
{
	int position = 0;
	char* cmd;
	char* ptr = line;

	char** cmds = (char**)malloc(sizeof(char*) * BUF_SIZE);
	cmd = strtok(ptr, "|");
	while(cmd != NULL)
	{
		cmds[position++] = cmd;
		cmd = strtok(NULL, "|");
	}

	cmds[position] = NULL;
	return cmds;
}

char** part(char* cmd)
{
	int position = 0;
	char* part;
	char* ptr = cmd;

	char** parts = (char**)malloc(sizeof(char*) * BUF_SIZE);
	part = strtok(ptr, " ");
	while(part != NULL)
	{
		parts[position++] = part;
		part = strtok(NULL, " ");
	}

	parts[position] = NULL;
	return parts;
}

char* read_line()
{
	char* line = NULL;
	size_t size = 0;
	ssize_t str_len;

	if((str_len = getline(&line, &size, stdin)) == -1)
	{
		perror("Couldn't read from stdin");
		free(line);
		printf("\n");
		exit(1);
	}

	if(line[str_len - 1] == '\n')
	{
		line[str_len - 1] = '\0';
	}

	if(!strcmp(line,"exit"))
	{
		printf("There are stopped jobs.\n");
		free(line);
		exit(0);
	}
	return line;
}



int main(int argc, char const *argv[])
{
	
	while(1)
	{
		display();
		char* line = read_line();
		int num_of_cmd = counter(line);

		char** cmds = separ(line);
		int fd_input = 0;

		for (int i = 0; i < num_of_cmd; ++i)
		{
			int fd[2];
			pipe(fd);

			int status = 0;

			pid_t pid = fork();

	
			if(pid < 0)
			{
				perror("No child process is created");
				return -1;
			}
			else if(pid == 0)
			{
				char** parts = part(cmds[i]);
				dup2(fd_input, 0);
				if(i != num_of_cmd - 1)
				{
					dup2(fd[1], 1);
				}
				close(fd[0]);
				execvp(parts[0], parts);
				perror("Exec");
				return -1;
			}
			else
			{
				wait(&status);
				fd_input = fd[0];
				close(fd[1]);
			}
		}
		free(cmds);
		free(line);
	}
		
	return 0;
}