#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER 1024

/*
queue to store messages.
condition variables :buffer  (empty or full).
Contains a mutex for functions to lock on before modifying the array,
*/
typedef struct {
  pthread_mutex_t *mutex;
  pthread_cond_t *notFull, *notEmpty;
  int head, tail;
  int full, empty;
  char *buffer[MAX_BUFFER];
} chat_queue;

/*
mainly for server use (chatroom data).
list of client sockets , and mutex list
message queue
socket for new connections
*/
typedef struct {
  pthread_mutex_t *clientListMutex;
  fd_set serverReadFds;

  int socketFd;
  int clientSockets[MAX_BUFFER];
  int numClients;

  chat_queue *queue;
} Chat_datastruct;

/*
this is used only in client handler thread
store 1. Chat_datastruct 2. new socket_fd
*/
typedef struct {
  Chat_datastruct *data;
  int clientSocketFd;
} clientHandlerVars;