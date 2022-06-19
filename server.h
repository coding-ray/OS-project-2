#include <netinet/in.h>  // sockaddr_in
#include <pthread.h>     // pthread_mutex_t, pthread_cond_t
#include <sys/select.h>  // fd_set
#include <sys/socket.h>  // fd_set
#include <sys/time.h>    // fd_set
#include <sys/types.h>   // fd_set
#include <unistd.h>      // fd_set

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

void startChat(int socketFd);
void buildMessage(char *result, char *name, char *msg);
void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port);
void removeClient(Chat_datastruct *data, int clientSocketFd);

void *newClientHandler(void *data);
void *clientHandler(void *chv);
void *messageHandler(void *data);

void queueDestroy(chat_queue *q);
chat_queue *queueInit(void);
void queuePush(chat_queue *q, char *msg);
char *queuePop(chat_queue *q);