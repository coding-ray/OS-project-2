#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  struct sockaddr_in serverAddr;
  // Set default port 6666
  long port = 6666;
  int socketFd;

  if (argc == 2) port = strtol(argv[1], NULL, 0);

  if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket creation failed");
    exit(1);
  }

  bindSocket(&serverAddr, socketFd, port);
  if (listen(socketFd, 1) == -1) {
    perror("listen failed: ");
    exit(1);
  }

  startChat(socketFd);

  close(socketFd);
}

// Spawns the new client handler thread and message consumer thread
void startChat(int socketFd) {
  Chat_datastruct data;
  data.numClients = 0;
  data.socketFd = socketFd;
  data.queue = queueInit();
  data.clientListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(data.clientListMutex, NULL);

  // Start thread to handle new client connections
  pthread_t connectionThread;
  if ((pthread_create(&connectionThread, NULL, (void *)&newClientHandler,
                      (void *)&data)) == 0) {
    fprintf(stderr,
            "*******************NCKU OS_ChatRoom started*******************\n");
  }

  FD_ZERO(&(data.serverReadFds));
  FD_SET(socketFd, &(data.serverReadFds));

  // Start thread to handle messages received
  pthread_t messagesThread;
  if ((pthread_create(&messagesThread, NULL, (void *)&messageHandler,
                      (void *)&data)) == 0) {
    fprintf(stderr, "allow clients now..........\n");
  }

  // threads done
  pthread_join(connectionThread, NULL);
  pthread_join(messagesThread, NULL);

  queueDestroy(data.queue);

  // !!!!remember to free mutex
  pthread_mutex_destroy(data.clientListMutex);
  free(data.clientListMutex);
}

// Initializes queue
chat_queue *queueInit(void) {
  chat_queue *q = (chat_queue *)malloc(sizeof(chat_queue));
  if (q == NULL) {
    perror("memory is full!");
    exit(EXIT_FAILURE);
  }
  // queue init
  q->empty = 1;
  q->full = q->head = q->tail = 0;
  q->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  if (q->mutex == NULL) {
    perror("memory is full!");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_init(q->mutex, NULL);

  q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  if (q->notFull == NULL) {
    perror("memory is full!");
    exit(EXIT_FAILURE);
  }

  pthread_cond_init(q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  if (q->notEmpty == NULL) {
    perror("Couldn't allocate anymore memory!");
    exit(EXIT_FAILURE);
  }
  pthread_cond_init(q->notEmpty, NULL);

  return q;
}

// Frees a queue
void queueDestroy(chat_queue *q) {
  pthread_mutex_destroy(q->mutex);
  pthread_cond_destroy(q->notFull);
  pthread_cond_destroy(q->notEmpty);
  free(q->mutex);
  free(q->notFull);
  free(q->notEmpty);
  free(q);
}

// Push to end of queue
void queuePush(chat_queue *q, char *msg) {
  q->buffer[q->tail] = msg;
  q->tail++;
  if (q->tail == MAX_BUFFER) q->tail = 0;
  if (q->tail == q->head) q->full = 1;
  q->empty = 0;
}

// Pop front of queue
char *queuePop(chat_queue *q) {
  char *msg = q->buffer[q->head];
  q->head++;
  if (q->head == MAX_BUFFER) q->head = 0;
  if (q->head == q->tail) q->empty = 1;
  q->full = 0;

  return msg;
}

// Sets up and socket then bind
void bindSocket(struct sockaddr_in *serverAddr, int socketFd, long port) {
  memset(serverAddr, 0, sizeof(*serverAddr));
  // these are required
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr->sin_port = htons(port);

  if (bind(socketFd, (struct sockaddr *)serverAddr,
           sizeof(struct sockaddr_in)) == -1) {
    perror("Socket bind failed: ");
    exit(1);
  }
}

// Removes the socket from the list of active client sockets and closes it
void removeClient(Chat_datastruct *data, int clientSocketFd) {
  pthread_mutex_lock(data->clientListMutex);
  for (int i = 0; i < MAX_BUFFER; i++) {
    if (data->clientSockets[i] == clientSocketFd) {
      data->clientSockets[i] = 0;
      close(clientSocketFd);
      data->numClients--;
      i = MAX_BUFFER;
    }
  }
  pthread_mutex_unlock(data->clientListMutex);
}

// Thread to handle new connections. Adds client's fd to list of client fds and
// spawns a new clientHandler thread for it
void *newClientHandler(void *data) {
  Chat_datastruct *chatData = (Chat_datastruct *)data;
  while (1) {
    int clientSocketFd = accept(chatData->socketFd, NULL, NULL);
    if (clientSocketFd > 0) {
      fprintf(stderr, "new client trying to join. It's Socket_fd : %d\n",
              clientSocketFd - 4);

      // Obtain lock on clients list and add new client in
      pthread_mutex_lock(chatData->clientListMutex);
      if (chatData->numClients < MAX_BUFFER) {
        // Add new client to list
        for (int i = 0; i < MAX_BUFFER; i++) {
          if (!FD_ISSET(chatData->clientSockets[i],
                        &(chatData->serverReadFds))) {
            chatData->clientSockets[i] = clientSocketFd;
            i = MAX_BUFFER;
          }
        }

        FD_SET(clientSocketFd, &(chatData->serverReadFds));

        // Spawn new thread to handle client's messages
        clientHandlerVars chv;
        chv.clientSocketFd = clientSocketFd;
        chv.data = chatData;

        pthread_t clientThread;
        if ((pthread_create(&clientThread, NULL, (void *)&clientHandler,
                            (void *)&chv)) == 0) {
          chatData->numClients++;
          fprintf(stderr,
                  "new client is joined successfully!! It's Socket_fd : %d\n",
                  clientSocketFd - 4);
        } else
          close(clientSocketFd);
      }
      pthread_mutex_unlock(chatData->clientListMutex);
    }
  }
}

/*this is "producer"*/
// listens for messages from client
// add messages to message queue
void *clientHandler(void *chv) {
  clientHandlerVars *vars = (clientHandlerVars *)chv;
  Chat_datastruct *data = (Chat_datastruct *)vars->data;

  chat_queue *q = data->queue;
  int clientSocketFd = vars->clientSocketFd;

  char msgBuffer[MAX_BUFFER];
  while (1) {
    int numBytesRead = read(clientSocketFd, msgBuffer, MAX_BUFFER - 1);
    msgBuffer[numBytesRead] = '\0';

    // If the client sent /exit\n, remove them from the client list and close
    // their socket
    if (strcmp(msgBuffer, "/exit\n") == 0) {
      // clientSocketFd
      fprintf(stderr, "Client on socket %d has disconnected.\n",
              clientSocketFd - 4);
      removeClient(data, clientSocketFd);
      return NULL;
    } else {
      // Wait for queue to not be full before pushing message
      while (q->full) {
        pthread_cond_wait(q->notFull, q->mutex);
      }

      // Obtain lock, push message to queue, unlock, set condition variable
      pthread_mutex_lock(q->mutex);
      fprintf(stderr, "message is pushed to queue (from client-> %s) \n",
              msgBuffer);
      queuePush(q, msgBuffer);
      pthread_mutex_unlock(q->mutex);
      pthread_cond_signal(q->notEmpty);
    }
  }
}

/*this is "consumer" */
// waits for any new messages in the queue
// broad cast them to every cliens
void *messageHandler(void *data) {
  Chat_datastruct *chatData = (Chat_datastruct *)data;
  chat_queue *q = chatData->queue;
  int *clientSockets = chatData->clientSockets;

  while (1) {
    // Obtain lock and pop message from queue when not empty
    pthread_mutex_lock(q->mutex);
    while (q->empty) {
      pthread_cond_wait(q->notEmpty, q->mutex);
    }
    char *msg = queuePop(q);
    pthread_mutex_unlock(q->mutex);
    pthread_cond_signal(q->notFull);

    // Broadcast message to all connected clients
    fprintf(stderr, "Broadcasting to every clients ->(from user %s) \n", msg);
    for (int i = 0; i < chatData->numClients; i++) {
      int socket = clientSockets[i];
      if (socket != 0 && write(socket, msg, MAX_BUFFER - 1) == -1)
        perror("Socket failed (write): ");
    }
  }
}