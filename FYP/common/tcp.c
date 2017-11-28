#include "tcp.h"
#include <stdio.h>
#include <stdlib.h> /* atoi, exit */
#include <string.h> /* memset, bzero, bcopy */
#include <fcntl.h> /* open, O_RDONLY, O_WRONLY, O_CREAT */

void error(const char* msg)
{
	perror(msg);
	exit(0);
}

/* Create a socket descriptor for a server. */
int CreateServer(int servPort)
{
	int sd;
	struct sockaddr_in serv_addr;

	/* Create a tcp socket */
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sd < 0)
		error("ERROR: Cannot open socket.");

	/* Init the address of the current host and port
	   number on which the server will run */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(servPort);

    /* Bind a socket to an address*/
    if (bind(sd, (struct sockaddr *) &serv_addr, 
    	     sizeof(serv_addr)) < 0) 
    	error("ERROR on binding");

    /* The server is waiting for connection. */
    listen(sd, 5);
    printf("Lisening on port %d.\n", servPort);

    return sd;
}

/* Accept a connetion from client and  Return a file descriptor. */
int AcceptConnection(int servSock)
{
	struct sockaddr_in chi_addr;
	socklen_t clilen = sizeof(chi_addr);

	int fd = accept(servSock, (struct sockaddr *) &chi_addr,
		              &clilen);
	if (fd < 0)
		error("ERROR: Cannot accept connection.");

	/* Convert numberic IP to readable format for display */
	printf("Client connected from %s:%d.\n", GetPeerIP(&chi_addr),
		        GetPeerPort(&chi_addr));

	return fd;
}

/* Connect to the server and Return a file descriptor. */
int ConnectServer(int servPort, const char* address)
{
	int sd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	/* Create a TCP socket */
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sd < 0)
		error("ERROR: Cannot create a socket.");

	/* Get host by IP address */
	server = gethostbyname(address);
	if (server == NULL)
		error("ERROR: No such host.");

	/* init the address of the current host and port
	   number on which the server will run */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(servPort);

    /* Establish a connection to the server */
    if (connect(sd,(struct sockaddr *) &serv_addr,
    	           sizeof(serv_addr)) < 0)
        error("ERROR: Cannot connect to the server.");

    printf("Connected to:%s:%d.\n", address, servPort);

    return sd;
}

char* GetPeerIP(struct sockaddr_in* address)
{
	char ip[50];
	inet_ntop(AF_INET, (struct in_addr*) &(address->sin_addr.s_addr),
		        ip, sizeof(ip)-1);
	char* addr = ip;
	return addr;
}

int GetPeerPort(struct sockaddr_in* address)
{
	return ntohs(address->sin_port);
}

void VerifyConnection(int fd, int flag)
{
	char buf[256];
	if (flag){ // Client side
		SendInt(fd, VERIFY);
		char sent_msg[12] = "Hello World";
		if (send(fd,sent_msg,11,0) < 0)
			error("ERROR: Cannot Write Msg to Socket.");
		bzero(buf,255);
		if (recv(fd,buf,255,0) < 0)
			error("ERROR: Cannot Read Msg from Socket.");
    	printf("From Server: %s\n",buf);		
	}
	else{ // Server side
		char recv_msg[19] = "I got your message";
		bzero(buf,255);
		if (recv(fd,buf,255,0) < 0)
			error("ERROR: Cannot Read Msg from Socket.");
		printf("Here is the message: %s\n",buf);
		if (send(fd,recv_msg,18,0) < 0)
			error("ERROR: Cannot Write Msg to Socket.");
	}
}

int SendInt(int fd, int raw)
{
	int n;
	int size = sizeof(int);
	char buf[2*size];
	memcpy(buf,&raw,size);
	if ((n = send(fd,buf,size,0)) < 0)
		error("ERROR: Cannot Send Int.");
	return n;
}

int RecvInt(int fd)
{
	int raw;
	int size = sizeof(int);
	char buf[2*size];
	if (recv(fd,buf,size,0) < 0)
		error("ERROR: Cannot Recv Ints.");
	raw = *(int*)buf;
	return raw;
}

int SendMeta(int fd, Meta_t meta)
{
	int n;
	SendInt(fd,meta.fileNameSize);
	SendInt(fd,meta.fileSize);
	if ((n = send(fd,meta.fileName,meta.fileNameSize,0))<0)
		error("ERROR: Cannot Send FileName.");
	return n;
}

Meta_t RecvMeta(int fd)
{
	Meta_t meta;
	meta.fileNameSize=RecvInt(fd);
	meta.fileSize=RecvInt(fd);
	meta.fileName=(char*)malloc(sizeof(char)*meta.fileNameSize);
	if (recv(fd,meta.fileName,meta.fileNameSize,0)<0)
		error("ERROR: Cannot Recv FileName.");
	// DEBUG(meta.fileName,str);
	return meta;
}

int SendCode(int fd, Code_t code)
{
	int n;
	if ((n = send(fd,code.key,16,0))<0)
		error("ERROR: Cannot Send Key.");
	if ((n = send(fd,code.iv,16,0))<0)
		error("ERROR: Cannot Send IV.");
	SendInt(fd,code.chunkID);
	return n;
}

Code_t RecvCode(int fd)
{
	int n;
	Code_t code;
 	if ((n = recv(fd, code.key, 16, 0))<0)
 		error("ERROR: Cannot Recv Key.");
 	if ((n = recv(fd, code.iv, 16, 0))<0)
 		error("ERROR: Cannot Recv IV.");
 	code.chunkID=RecvInt(fd);
 	return code;
}

int SendInput(int fd, Input_t input)
{
	int n;
	SendInt(fd,input.chunkID);
	SendInt(fd,input.chunkSize);
	SendInt(fd,input.ciperSize);
	if ((n = send(fd,input.ciper,input.ciperSize,0))<0)
		error("ERROR: Cannot Send Ciper.");
	return n;
}

Input_t RecvInput(int fd)
{
	Input_t input;
	input.chunkID=RecvInt(fd);
	input.chunkSize=RecvInt(fd);
	input.ciperSize=RecvInt(fd);
	input.ciper=(unsigned char*)malloc(sizeof(char)*(input.ciperSize));
	if (recv(fd,input.ciper,input.ciperSize,0)<0)
		error("ERROR: Cannot Recv Ciper.");
	return input;
}

int SendChunk(int fd, char* ciper, int ciperSize, unsigned char* key,
				unsigned char* iv)
{
	int n;
	SendInt(fd,SENDCHUNK);
	SendInt(fd,ciperSize);
	if ((n = send(fd,ciper,ciperSize,0))<0)
		error("ERROR: Cannot Send Ciper.");
	if ((n = send(fd,key,16,0))<0)
		error("ERROR: Cannot Send Key.");
	if ((n = send(fd,iv,16,0))<0)
		error("ERROR: Cannot Send IV.");
	return n;
}
Chunk_t RecvChunk(int fd)
{
	Chunk_t chunk;
	chunk.ciperSize = RecvInt(fd);
	chunk.ciper=(unsigned char*)malloc(sizeof(char)*(chunk.ciperSize));
	if (recv(fd,chunk.ciper,chunk.ciperSize,0)<0)
		error("ERROR: Cannot Recv Ciper.");
	if (recv(fd,chunk.key,16,0)<0)
		error("ERROR: Cannot Recv Key.");
	if (recv(fd,chunk.iv,16,0)<0)
		error("ERROR: Cannot Recv IV.");
	return chunk;
}
