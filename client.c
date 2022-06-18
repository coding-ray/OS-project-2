// Mainly referenced from
// http://zake7749.github.io/2015/03/17/SocketProgramming/
#include <arpa/inet.h>  // inet_addr
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <username>", argv[0]);
    printf("Example: %s ray", argv[0]);
    return -1;
  }

  // socket的建立
  int sockfd = 0;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd == -1) {
    printf("Fail to create a socket.");
    return -1;
  }

  // socket的連線

  struct sockaddr_in info;
  bzero(&info, sizeof(info));
  info.sin_family = PF_INET;

  // localhost test
  info.sin_addr.s_addr = (unsigned)inet_addr("127.0.0.1");
  info.sin_port = htons(8700);

  int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
  if (err == -1) {
    printf("Connection error");
    return -1;
  }

  // Send a message to server
  char *message = malloc(sizeof(char) * 50);
  memcpy(
      message, argv[1],
      (sizeof(message) < sizeof(argv[1])) ? sizeof(message) : sizeof(argv[1]));

  char receiveMessage[100] = {};
  send(sockfd, message, sizeof(message), 0);
  recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);

  printf("%s", receiveMessage);
  printf("close Socket\n");
  close(sockfd);
  return 0;
}