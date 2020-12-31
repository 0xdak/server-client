#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main(void){
  int sock, valread = 0;
  int PORT;
  char HOST[50] = "";

  printf("IP Address: ");
  scanf("%s",HOST);
  printf("Port      : ");
  scanf("%d",&PORT);

  struct sockaddr_in serv_addr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0){
    perror("Socket Creation Failed");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  inet_pton(AF_INET, HOST, &serv_addr.sin_addr);
  serv_addr.sin_port   = htons(PORT);

  system("clear");
  printf("Connected to %s:%d\n", HOST, PORT);

  int conn = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  if (conn < 0){
    perror("Connection Failed");
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  while(1){
    printf("Mesaj: ");
    scanf("%s", buffer);
    if (send(sock, buffer, strlen(buffer), 0) < 0){
      perror("Send Failed");
      exit(EXIT_FAILURE);
    }
  }

  close(sock);


  return 0;
}
