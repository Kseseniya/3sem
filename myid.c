#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#define BIGNUM 1000

int main(int argc, char** argv)
{
	uid_t uid;
	gid_t gid;
	int size = 0, ngroups;
	gid_t list[BIGNUM];


	if(argc == 1)
	{
		uid = getuid();
		struct passwd* pwd = getpwuid(uid);

		printf("uid=%d(%s) gid=%d(%s) ", pwd->pw_uid, pwd->pw_name, 
										 pwd->pw_gid, pwd->pw_name);
		ngroups = getgroups(BIGNUM, list);

		printf("groups=%d(%s)", list[ngroups - 1], getgrgid(list[ngroups - 1])->gr_name);
		for (int i = 0; i < ngroups - 1; ++i)
		{
			printf(",%d(%s)", list[i], getgrgid(list[i])->gr_name);
		}
		printf("\n");
	}
	else
	{
		struct passwd* pwd = getpwnam(argv[1]);
		printf("uid=%d(%s) gid=%d(%s) ", pwd->pw_uid, pwd->pw_name, 
										 pwd->pw_gid, pwd->pw_name);
		ngroups = 1000;//?????????????
		ngroups = getgrouplist(pwd->pw_name, pwd->pw_gid, list, &ngroups);

		printf("groups=%d(%s)", list[ngroups - 1], getgrgid(list[ngroups - 1])->gr_name);
		for (int i = 0; i < ngroups - 1; ++i)
		{
			printf(",%d(%s)", list[i], getgrgid(list[i])->gr_name);
		}
		printf("\n");
	}

	return 0;
}