#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BIGNUM 1000

int main(int argc, char const *argv[])
{
	int fd_inp  = 0,
		fd_outp = 1,
		fd_err  = 2;

	char *buffer[BIGNUM];
	

	if(argc == 1)
	{
		while(1) 
		{
			int s = read(fd_inp, buffer, BIGNUM);
			write(fd_outp, buffer, s);
		}
	}

	if(argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			int fd = open(argv[i], O_RDONLY);
//			printf("%d\n", fd);
			if(fd < 0)
			{
				printf("NO SUCH FILE\n");
			}
			int s1 = read(fd, buffer, BIGNUM);
			write(fd_outp, buffer, s1); 
		}
	}
	return 0;
}
