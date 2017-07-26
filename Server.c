#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>

#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		256	/* buffer length */

int echod(int);
void reaper(int);

int main(int argc, char **argv)
{
	int 	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client; //contains the internet address

	switch(argc){
	case 1:
		port = SERVER_TCP_PORT;
		break;
	case 2:
		port = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "Usage: %d [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/
//below socket fuction will take:
// address domain of the socket (AF_INET)
// secong argumanet is type of socket (SOCK_STREAM) which in this case is TCP
// third arugment is 0, which tells OS to choose most appropriate protocol, in this case TCP for stream sockets (SOCK_STREAM)
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //if the return of socket function is -1, then return the error
		fprintf(stderr, "Can't create a socket\n");
		exit(1);
	}

	/* Bind an address to the socket	*/
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);//will contain the server address
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}

	/* queue up to 5 connect requests  */
	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

	while(1) {
	  client_len = sizeof(client);
	  new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
	  if(new_sd < 0){
	    fprintf(stderr, "Can't accept client \n");
	    exit(1);
	  }
	  switch (fork()){
	  case 0:		/* child */
		(void) close(sd);
		exit(echod(new_sd));
	  default:		/* parent */
		(void) close(new_sd);
		break;
	  case -1:
		fprintf(stderr, "fork: error\n");
	  }
	}
}

/*	echod program	*/
int echod(int sd)
{
	char	*bp, buf[BUFLEN];
	int 	n, bytes_to_read;
	FILE *fp;
	fp = fopen("file.txt","r");
	if(fp==NULL){
		printf("There is an issue with file");
		return(1);
	}
	//buf=fgets(fp);

	//while(n = read(sd, buf, BUFLEN)) 
	//	write(sd, buf, n);
	printf("\nBefore sending buffer\n");
	while(fgets(buf, BUFLEN,fp)!=NULL){
		printf("\nshow stuff in file\n");		
		printf("%s",buf);
		write(sd, buf, BUFLEN);
		printf("sent stuff\n");
	}
	close(sd);
	fclose(fp);
	return(0);
}

/*	reaper		*/
void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}
