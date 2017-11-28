#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server.h"
#include "dedup.h"

void Sigint_handler(int sig)
{
	printf("\nServer is terminated ...\n");
	exit(0);
}

void *ThreadVerify(void *args)
{
	int fd;
	int flag;
	/* Guarantees that thread resources are deallocated upon return */
	pthread_detach(pthread_self());

	/* Extract socket file descriptor from argument */
	fd = ((VerifyArgs*) args) -> fd;
	flag = ((VerifyArgs*) args) -> flag;
	free(args); /* Deallocate memory for argument */

    VerifyConnection(fd, flag);

 	close(fd); // close connection btw server and client
	pthread_exit(NULL);
}

void* ThreadUpload(void* args)
{
	int fd;
	dedupObj_t *dedupObj;
	pthread_detach(pthread_self());
	fd = ((UploadArgs*) args) -> fd;
	dedupObj = ((UploadArgs*) args) -> dedupObj;
	free(args);
	Meta_t meta;
	Code_t code;
	Input_t input;
	while(1){
		int indicator = RecvInt(fd);
		DEBUG(indicator,int);
		if (indicator == SENDMETA){
			meta=RecvMeta(fd);
			// DEBUG(meta.fileName,str);
			// DEBUG(meta.fileNameSize,int);
			// DEBUG(meta.fileSize,int);
		}
		if (indicator == SENDCODE){
			code=RecvCode(fd);
			// displayMD5(code.key);
		}
		if (indicator == SENDDATA){
			input=RecvInput(fd);
			// displayCiper(input.ciper,input.ciperSize);
		}
		if (indicator == END){
			break;
		}
	}
	// begin dedup
	dedup(dedupObj,meta,code,input);

	close(fd);
	pthread_exit(NULL);
}

void* ThreadDownload(void* args)
{
	int fd;
	dedupObj_t *dedupObj;
	pthread_detach(pthread_self());
	fd = ((UploadArgs*) args) -> fd;
	dedupObj = ((UploadArgs*) args) -> dedupObj;
	free(args);

	Meta_t meta;
	meta=RecvMeta(fd);
	restore(fd, *dedupObj, meta.fileName);
	SendInt(fd,END);
	close(fd);
	pthread_exit(NULL);
}

void RunServer(int lsd)
{
	int fd_cli;
	int indicator;
	dedupObj_t dedupObj = initDedupObj();
	pthread_t threadID;

	while (1){
		printf("Waiting for connection ...\n");
		fd_cli = AcceptConnection(lsd);
		// displayDedupObj(dedupObj);
		// recv indicator first
		indicator = RecvInt(fd_cli);
		printf("Received indicator: %d\n", indicator);

		switch (indicator){

			case UPLOAD:{
				UploadArgs *args = (UploadArgs*)malloc(sizeof(UploadArgs));
				args->fd = fd_cli;
				args->dedupObj = &dedupObj;
				if (pthread_create(&threadID,NULL,ThreadUpload,
						(void*) args) != 0)
					error("ERROR: Thread Cannot be Created.");
			}break;

			case DOWNLOAD:{
				DownloadArgs *args = (DownloadArgs*)malloc(sizeof(DownloadArgs));
				args->fd = fd_cli;
				args->dedupObj = &dedupObj;
				if (pthread_create(&threadID,NULL,ThreadDownload,
						(void*) args) != 0)
					error("ERROR: Thread Cannot be Created.");
			}break;

			case VERIFY:{
				VerifyArgs *args = (VerifyArgs*)malloc(sizeof(VerifyArgs));
				args -> fd = fd_cli;
				args -> flag = SERVER;
				if (pthread_create(&threadID,NULL,ThreadVerify,
						(void*) args) != 0)
					error("ERROR: Thread Cannot be Created.");
			}break;

			default:
				break;
		}
	}
}
