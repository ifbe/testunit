#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void main(int argc, char *argv[ ], char **env)
{
	int fd[2];
	int fd1[2];
	pid_t pid;
	pipe(fd);
	pipe(fd1);
	pid =fork();
	if(0==pid)
	{
		dup2(*read_fd,0);
		dup2(*write_fd1,1);
	}
	else
	{
		write(*write_fd,scan,5);
		write(*write_fd,disconnect,15);
		while(1)
		{
			a=read(*read_fd1,readbuf,sizeof(readbuf));
		}	
	}
}









