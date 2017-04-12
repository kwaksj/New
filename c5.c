#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 127
void error_handling(char *message);

int main(int argc, char **argv )
{
	struct sockaddr_in servaddr;
	int sock;
	char buf[BUF_SIZE];
	char buf2[BUF_SIZE];
	char filename[20];
	int filesize, fp, filenamesize, fp2;
	int sread, total=0;
	
	if(argc != 3)
	{
		printf("Usage: %s ip_address port ", argv[0]);
		exit(0);		
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);

	if (sock == -1)
		error_handling("socket() error");

	memset((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);                        	
	servaddr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
		error_handling("connect() error");
	else	
		printf("select file to send : ");

	if ( fgets(filename, sizeof(filename), stdin) == NULL )	
		exit(0);

	filenamesize = strlen(filename);	
	filename[filenamesize-1] = 0;

	if((fp = open( filename, O_RDONLY )) == -1 )
		error_handling("open() error");

	send(sock, filename, sizeof(filename), 0 );
	filesize = lseek( fp, 0, SEEK_END );
	send(sock, &filesize, sizeof(filesize), 0 );
	lseek(fp, 0, SEEK_SET );

	printf( "file is sending now.. \n" );
	
	while( total != filesize )
	{
		sread = read( fp, buf, 100 );
		total += sread;
		buf[sread] = 0;
		send(sock, buf, sread, 0 );		
	}

	printf( "file translating is completed\n " );	
	printf( "filesize : %d, sending : %d ", filesize, total );
	
	close(fp);

	memset(filename, 0, 20);
	recv(sock, filename, sizeof(filename), 0);	
	read(sock, &filesize, sizeof(filesize));
	
	fp2 = open(filename, O_WRONLY | O_CREAT | O_TRUNC);

	printf("file is receiving now...\n");
	
	total = 0;
	
	while (total != filesize)
	{
		sread = recv(sock, buf2, BUF_SIZE, 0);
		total += sread;
		buf2[sread] = 0;
		write(fp2, buf2, sread);		
	}
	
	printf("file translating is completed\n");
	
	close(fp2);
	close(sock);
	
	return 0;
}

void s_get(char fname[])
{
/*	int s, nbyte;
	char buf[BUF_SIZE];
	int filesize, fp, filenamesize ;	
	int sread, total=0;

	read(1, &filesize, sizeof(filesize) );
	printf( "%d ", filesize );	
	fp = open( fname, O_WRONLY | O_CREAT | O_TRUNC);

	while( total != filesize )
	{
		sread = recv(1, buf, 100, 0 );
		printf( "file is receiving now.. \n" );
		total += sread;
		buf[sread] = 0;
		write( fp, buf, sread );
		bzero( buf, sizeof(buf) );
		printf( "processing : %4.2f%%\n ", total*100 / (float)filesize );
		usleep(1000);	
	}
	
	printf( "file traslating is completed\n " );	
	printf( "filesize : %d, received : %d\n ", filesize, total );
*/
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);	
	exit(1);
}
 
