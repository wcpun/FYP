#ifndef __TCP_H__
#define __TCP_H__

#include <unistd.h> /* close, write, read */
#include <sys/types.h> 
#include <sys/socket.h> /* socket, bind, listen, accept, connect, socklen_t */
#include <netinet/in.h> /* sockadd_in, inet_ntop */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa */
#include <netdb.h>
#include <errno.h>
#include "utils.h"

#define SERVER 0
#define CLIENT 1

#define VERIFY (-1)
#define SENDMETA (-2)
#define SENDDATA (-3)
#define SENDCODE (-7)
#define SENDCHUNK (-8)
#define UPLOAD (-4)
#define DOWNLOAD (-5)
#define END (-6)

#define DEBUG(var, type) DEBUG_##type(var)
#define DEBUG_int(var) printf("Line:%d " #var ": %d\n", __LINE__, var)
#define DEBUG_str(var) printf("Line:%d " #var ": %s\n", __LINE__, var)


void error(const char* msg);

/* Create a socket descriptor for a server. */
int CreateServer(int servPort);

/* Accept a connetion from client and Return a file descriptor. */
int AcceptConnection(int servSock);

/* Connect to the server and Return a file descriptor. */
int ConnectServer(int servPort, const char* address);

int GetPeerPort(struct sockaddr_in* address);

char* GetPeerIP(struct sockaddr_in* address);

void VerifyConnection(int fd, int flag);

int SendInt(int fd, int raw);

int RecvInt(int fd);

int SendMeta(int fd, Meta_t meta);

Meta_t RecvMeta(int fd);

int SendCode(int fd, Code_t code);

Code_t RecvCode(int fd);

int SendInput(int fd, Input_t input);

Input_t RecvInput(int fd);

int SendChunk(int fd, char* ciper, int ciperSize, unsigned char* key,
				unsigned char* iv);

Chunk_t RecvChunk(int fd);


#endif
