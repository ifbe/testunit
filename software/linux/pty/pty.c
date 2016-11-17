# include <stdio.h> 
# include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <linux/limits.h> 
#include <pty.h> 
void do_passwd( int pty, const char *old_passwd, const char *new_passwd ) 
{ 
	char buffer[80]; 
	int ret; 
	fd_set reads; 
	int input_old_passwd = 0; 
	while(1)
	{ 
		FD_ZERO( &reads ); 
		FD_SET( pty, &reads ); 
		ret = select( pty+1, &reads, NULL, NULL, NULL ); 
		if ( ret == -1 ){ 
			perror( "select" ); 
			break; 
		} 

		ret = read( pty, buffer, sizeof(buffer) ); 
		if ( ret <= 0 )break; 

		write( fileno(stdout), buffer, ret ); 
		fflush( stdout ); 
		if ( buffer[ret-1] == '\n' )continue; 

		if ( !input_old_passwd )
		{ 
			write( pty, old_passwd, strlen(old_passwd) ); 
			write( pty, "\n", 1 ); 
			write( fileno(stdout), old_passwd, strlen(old_passwd) ); 
			input_old_passwd = 1; 
		} 
		else
		{ 
			write( pty, new_passwd, strlen(new_passwd) ); 
			write( pty, "\n", 1 ); 
			write( fileno(stdout), new_passwd, strlen(new_passwd) ); 
		} 
	} 
	return; 
} 
int main( int argc, char *argv[] ) 
{ 
	int pty, slave; 
	char pty_name[PATH_MAX]; 
	int ret; 
	pid_t child; 
	if ( argc < 3 ){ 
		fprintf( stderr, "Usage: %s <old password> <new password>\n", argv[0] ); 
		exit( EXIT_FAILURE ); 
	} 

	ret = openpty( &pty, &slave, pty_name, NULL, NULL ); 
	if ( ret == -1 ){ 
		perror( "openpty" ); 
		exit( EXIT_FAILURE ); 
	} 

	child = fork(); 
	if ( child == -1 ){ 
		perror( "fork" ); 
		exit( EXIT_FAILURE ); 
	} 
	else if ( child == 0 ){ 
		close( pty ); 
		login_tty( slave ); 
		execl( "/usr/bin/passwd", "passwd", NULL ); 
	} 

	close( slave ); 
	printf( "pty name: %s\n", pty_name ); 
	do_passwd( pty, argv[1], argv[2] ); 
	exit( EXIT_SUCCESS ); 
} 
