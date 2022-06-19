#include <netdb.h>       // struct hostent
#include <netinet/in.h>  // struct sockaddr_in

// read write buffer maxsize
#define MAX_BUFFER 1024

// chatroom mainloop
void chat_room(char *name, int socket_fd);

// format every message before sending it (this function will be used in both
// client and server)
void buildMessage(char *result, char *name, char *msg);

// Sets up the socket and connects
void socket_connection(struct sockaddr_in *serverAddr, struct hostent *host,
                       int socket_fd, long port);

// Sets the fd to nonblocking
void setNonBlock(int fd);

// Notify the server when the client exits by sending "/exit"
void interruptHandler(int sig_unused);