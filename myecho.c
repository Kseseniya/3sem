#include <stdio.h>
#include <string.h>

int main(int argc, char*argv[])
{

	if (argc > 1)
	{
		int IsTransf = 0;
		int nargv = 2;

		if(strcmp(argv[1], "-n") != 0)
		{
			IsTransf = 1;
			nargv = 1;
		}
		
		for (nargv; nargv < argc; ++nargv)
		{
			printf("%s", argv[nargv]);
			if(nargv != argc - 1)
			{
				printf(" ");
			}
		}

		if(IsTransf == 1)
		{
			printf("\n");
		}

	}
	else
	{
		printf("\n");
	}

	return 0; 
}