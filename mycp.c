#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#define BIGNUM 1000
#define DIR 1
#define FILE 0

int chek_file(char const* file_name)
{
	int fd_inp = 0,
		fd_out = 1,
		fd_err = 2;
	int fd = open(file_name, O_RDONLY);
	if(fd < 0)
		return 0;

	return 1;
}

int is_dir(char const* file_name)
{

	if(file_name[strlen(file_name) - 1] == '/')
		return DIR;
	else
		return FILE;
}
char* file_name(char const* file_path)
{
	char* file_name = strrchr(file_path, '/');
	return file_name + 1;
}

void copy_file(char const* file_1, char const* file_2)
{
	char* buf[BIGNUM];

	if(chek_file(file_1))
	{
		int fd1 = open(file_1, O_RDONLY);
		int s1 = read(fd1, buf, BIGNUM);
		int fd2 = open(file_2, O_WRONLY|O_CREAT|O_TRUNC);
		write(fd2, buf, s1);
	}
	else
	{
		write(2, "No such file\n", strlen("No such file\n"));
	}
	return;
}

void file_to_dir(char const* file, char const* dir)
{
	int dir_offset = strlen(dir);
	char* direc = dir;
	direc = strcat(dir, file);
	copy_file(file, direc);
	direc[dir_offset] = '\0';
	return;
}



int main(int argc, char const *argv[])
{
	int fd_inp = 0,
		fd_out = 1,
		fd_err = 2;

	char* buffer[BIGNUM];

	if (argc <= 2)
	{
		write(fd_err, "Too few arguments\n", strlen("Too few arguments\n"));
	}
	if (argc == 3)
	{
		if(!is_dir(argv[2]))
		{
			copy_file(argv[1], argv[2]);
		}
		else
		{
			file_to_dir(argv[1], argv[2]);
		}
	}
	if (argc > 3)
	{
		for (int i = 1; i <= argc - 2; ++i)
		{
			file_to_dir(argv[i], argv[argc - 1]);
		}
	}
	return 0;
}
