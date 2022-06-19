/*run&compile instructions*/
/*compile this file : $gcc client.c -std=c99 -lpthread -Wall -o client
  connect to server (run) : telnet localhost [port]
                            --> the port should be consistent with the server
  [port]*/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// read write buffer maxsize
#define MAX_BUFFER 1024

static int socket_fd;

// chatroom mainloop
void chat_room(char *name, int socket_fd) {
  fd_set clientFds;
  char chatMsg[MAX_BUFFER];
  char chatBuffer[MAX_BUFFER], msgBuffer[MAX_BUFFER];

  /*this while loop waiting for available fd*/
  while (1) {
    /*use select function*/
    // Reset the fd_set each time when modified by select
    FD_ZERO(&clientFds);
    FD_SET(socket_fd, &clientFds);
    FD_SET(0, &clientFds);
    if (select(FD_SETSIZE, &clientFds, NULL, NULL, NULL) !=
        -1)  // use select function to check whether "fd" is available
    {
      for (int fd = 0; fd < FD_SETSIZE; fd++) {
        if (FD_ISSET(fd, &clientFds)) {
          if (fd == socket_fd)  // receive data from server
          {
            int numBytesRead = read(socket_fd, msgBuffer, MAX_BUFFER - 1);
            msgBuffer[numBytesRead] = '\0';
            printf("%s", msgBuffer);
            memset(&msgBuffer, 0, sizeof(msgBuffer));
          } else if (fd == 0)  // read from keyboard (stdin) and send to server
          {
            fgets(chatBuffer, MAX_BUFFER - 1, stdin);
            if (strcmp(chatBuffer, "/exit\n") == 0)
              interruptHandler(-1);  // Reuse the interruptHandler function to
                                     // disconnect the client
            else {
              buildMessage(chatMsg, name, chatBuffer);
              if (write(socket_fd, chatMsg, MAX_BUFFER - 1) == -1)
                perror("write failed: ");
              // printf("%s", chatMsg);
              memset(&chatBuffer, 0, sizeof(chatBuffer));
            }
          }
        }
      }
    }
  }
}

// format every message before sending it (this function will be used in both
// client and server)
void buildMessage(char *result, char *name, char *msg) {
  memset(result, 0, MAX_BUFFER);
  strcpy(result, name);
  strcat(result, ": ");
  strcat(result, msg);
}

// Sets up the socket and connects
void socket_connection(struct sockaddr_in *serverAddr, struct hostent *host,
                       int socket_fd, long port) {
  memset(serverAddr, 0, sizeof(serverAddr));
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_addr = *((struct in_addr *)host->h_addr_list[0]);
  serverAddr->sin_port = htons(port);
  if (connect(socket_fd, (struct sockaddr *)serverAddr,
              sizeof(struct sockaddr)) < 0) {
    perror("Couldn't connect to server");
    exit(1);
  }
}

// Sets the fd to nonblocking
void setNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  if (flags < 0) perror("fcntl failed");

  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

// Notify the server when the client exits by sending "/exit"
void interruptHandler(int sig_unused) {
  if (write(socket_fd, "/exit\n", MAX_BUFFER - 1) == -1)
    perror("write failed: ");

  close(socket_fd);
  exit(1);
}

int main(int argc, char *argv[]) {
  char *name;
  struct sockaddr_in serverAddr;
  struct hostent *host;
  long port;

  if (argc != 4) {
    fprintf(stderr, "./client [username] [host] [port]\n");
    exit(1);
  }
  name = argv[1];
  if ((host = gethostbyname(argv[2])) == NULL) {
    fprintf(stderr, "Couldn't get host name\n");
    // fprintf("your error hostname : " , argv[1]);
    exit(1);
  }
  port = strtol(argv[3], NULL, 0);
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "Couldn't create socket\n");
    exit(1);
  }

  socket_connection(&serverAddr, host, socket_fd, port);
  setNonBlock(socket_fd);
  setNonBlock(0);

  // Set a handler for the interrupt signal
  signal(SIGINT, interruptHandler);

  chat_room(name, socket_fd);
}