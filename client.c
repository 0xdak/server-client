#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BFSZ 1024

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
  printf("Trying %s:%d\n", HOST, PORT);

  int conn = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  if (conn < 0){
    perror("Connection Failed");
    exit(EXIT_FAILURE);
  }

  char buffer[BFSZ];
  int pid = fork();
  if(pid > 0){
    while(1){
      memset(buffer, 0, BFSZ);
      valread = recv(sock, buffer, BFSZ, 0);
      if(valread <= 0) {
        printf("Connection is closed by %s\n", HOST);
        break;
      }
      printf(">>>%s", buffer);
    }
  }
  if(pid == 0){
    while(1){
      memset(buffer, 0, BFSZ);
      fgets(buffer, BFSZ, stdin);
      strtok(buffer, "\n");
      if (send(sock, buffer, strlen(buffer), 0) < 0){
        perror("Send Failed");
        exit(EXIT_FAILURE);
      }
    }
  }


  close(sock);


  return 0;
}
