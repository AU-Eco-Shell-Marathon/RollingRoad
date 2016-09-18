//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <lib.h>

#include <fcntl.h>

using namespace std;


int    sockfd, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buf;
int myfile;


int portno = 5000;
string ip = "192.168.11.2";
const long buffer_size = 1000;


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	server = gethostbyname(ip.c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	// Nu er TCP forbindelsen etableret

	writeTextTCP(argv[1], sockfd); //Sender tekst/string med stignavn og filnavn
	
	myfile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 0755);
	
	while(buf!='#')
	{
		n=read(sockfd, &buf, 1);
		if(n<0) error("ERROR reading to socket");
		if(buf!='#')
		write(myfile, &buf, n);
	}
	close(myfile);
	close(sockfd);
	return 0;
}
