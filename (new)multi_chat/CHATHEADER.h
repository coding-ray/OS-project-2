#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_BUFFER 1024
/*
Queue implementation using a char array.
Contains a mutex for functions to lock on before modifying the array,
and condition variables for when it's not empty or full.
*/
typedef struct
{
    pthread_mutex_t *mutex;
    pthread_cond_t *notFull, *notEmpty;
    int head, tail;
    int full, empty;
    char *buffer[MAX_BUFFER];
} chat_queue;

/*
Struct containing important data for the server to work.
Namely the list of client sockets, that list's mutex,
the server's socket for new connections, and the message queue
*/
typedef struct
{
    pthread_mutex_t *clientListMutex;
    fd_set serverReadFds;

    int socketFd;
    int clientSockets[MAX_BUFFER];
    int numClients;

    chat_queue *queue;
} Chat_datastruct;

/*
Simple struct to hold the Chat_datastruct and the new client's socket fd.
Used only in the client handler thread.
*/
typedef struct
{
    Chat_datastruct *data;
    int clientSocketFd;
} clientHandlerVars;