#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h> // trim

#define PORT 9090           // port
#define BFSZ 1024           // receive data size
#define CLNM 0003           // number of allowed clients

int prepare();
int communicate(int client);

int online = 0;
int main(void) {
  int server_fd, new_socket, valread;

  server_fd = prepare();

  // listening
  if (listen(server_fd, CLNM) < 0){
    perror("Listen Failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_info;
  socklen_t len = sizeof(client_info);
  int pid = 1;
  char client_ip[50];
  if (pid > 0) {
    printf("Listening  127.0.0.1:%d\n", PORT);
    while(pid > 0){
      new_socket = accept(server_fd, (struct sockaddr *)&client_info, &len);
      if(new_socket < 0){
        perror("Client Failed");
        exit(EXIT_FAILURE);
      }

      inet_ntop(AF_INET, &(client_info.sin_addr), client_ip, 50);
      printf("%s is connected.\n", client_ip);

      send(new_socket, "Connection Successful", 100, 0);
      online++;
      printf("%d Kisi Cevrimici", online);
      pid = fork();

    }

  }
  if (pid == 0) {
    // new client
    while(1){
      if(communicate(new_socket) == 0)
        break;
    }

  }
  else {
    exit(EXIT_FAILURE);
  }

  close(new_socket);
  close(server_fd);

  return 0;
}

int cmdon = 0;
char buffer[BFSZ];
int valread;
int communicate(int client){
  memset(buffer, 0, BFSZ);
  valread = recv(client, buffer, BFSZ, 0);
  strtok(buffer, "\n");
  if (valread <= 0){
    perror("Connection is closed");
    online--;
    printf("%d Kisi Cevrimici\n", online);
    return 0;
  }

  if (strcmp(buffer, "cmdon") == 0){ // if the message is "cmdon"
    cmdon = 1;
    printf("Terminal mode on\n");
    return 1;
  }
  else if (strcmp(buffer, "cmdoff") == 0){
    cmdon = 0;
    printf("Terminal mode off\n");
    return 1;
  }

  if(cmdon == 1){
    FILE *cmd = popen(buffer, "r");
    char output[BFSZ];
    fgets(output, sizeof(char) * BFSZ, cmd);
    strtok(output, "\n");
    send(client, output, BFSZ, 0);
  }
  else printf("%s\n", buffer);
  return 1;
}



// TODO programa arguman olarak ip ve port vermek
int prepare(){
  int server_fd;
  struct sockaddr_in address;
  int opt = 1;

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
//printf("Connected to %s:%d\n", HOST, PORT);

  return server_fd;
}
