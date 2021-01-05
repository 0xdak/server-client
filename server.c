#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define HOST "localhost"                            // host
#define PORT 9090                                   // port
#define BFSZ 1024                                   // receive data size
#define CLNM 0003                                   // number of allowed clients
#define OUT  1

#define d_printf OUT && printf                      // controlling printf

int prepare();
int communicate(int client_socket, char* client_ip);

int main(void) {
  int server_fd, new_client;
  int pid = 1;
  char client_ip[50];

  server_fd = prepare();

  // listening
  if (listen(server_fd, CLNM) < 0){
    d_printf("Listen Failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);

  if (pid > 0) {
    d_printf("Listening %s:%d\n",HOST, PORT);
    while(pid > 0){
      new_client = accept(server_fd, (struct sockaddr *)&client_addr, &len);
      if(new_client < 0){
        d_printf("Client Failed");
        exit(EXIT_FAILURE);
      }
      inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, 50);
      d_printf("%s is connected.\n", client_ip);

      send(new_client, "Connection Successful\n", 50, 0);

      pid = fork();

    }

  }
  if (pid == 0){
    // new client
    while (1){
        if(communicate(new_client, client_ip) == 0)
        break;
    }
  }
  else {
    exit(EXIT_FAILURE);
  }

  close(new_client);
  close(server_fd);

  return 0;
}

int cmdon = 0;
int valread;
char buffer[BFSZ];
char output[BFSZ];
char line[BFSZ];
int communicate(int client_socket, char* client_ip){
  memset(buffer, 0, BFSZ);
  memset(line, 0, BFSZ);
  memset(output, 0, BFSZ);
  valread = recv(client_socket, buffer, BFSZ, 0);
  strtok(buffer, "\r");
  strtok(buffer, "\n");
  if (valread <= 0){
    d_printf("%s : Connection is closed\n", client_ip);
    return 0;
  }

  if (strcmp(buffer, "cmdon") == 0){                // if the message is "cmdon"
    cmdon = 1;
    d_printf("%s : ", client_ip);
    d_printf("Terminal mode on\n");
    return 1;
  }
  else if (strcmp(buffer, "cmdoff") == 0){
    cmdon = 0;
    d_printf("%s : ", client_ip);
    d_printf("Terminal mode off\n");
    return 1;
  }

  if(cmdon == 1){
    FILE *cmd = popen(buffer, "r");
    while ( fgets( line, sizeof(char) * BFSZ, cmd) != NULL ) {
      strcat(output, line);
    }
  //strtok(output, "\n");
    send(client_socket, output, BFSZ, 0);
  }
  else {
    d_printf("%s : ", client_ip);
    d_printf("%s\n", buffer);
  }
  return 1;
}


int prepare(){
  int server_fd;
  struct sockaddr_in server_addr;
  int opt = 1;

  // creating socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0){
    d_printf("Socket Failed");
    exit(EXIT_FAILURE);
  }


  // Forcefully attaching socket to the port
  // For "Already in use" mistake
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                               &opt, sizeof(opt))){
    d_printf("setsockopt");
    exit(EXIT_FAILURE);
  }

  //strcat(HOST, (char*)&ip);
  //strcat(HOST, "192.168.1.105");

  server_addr.sin_family      = AF_INET;
  inet_pton(AF_INET, HOST, &server_addr.sin_addr);
//server_addr.sin_addr.s_addr = INADDR_ANY;             //inet_addr(HOST);
  server_addr.sin_port        = htons(PORT);

  // forcefully attaching socket to the port 9090
  int bind_rs = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_rs < 0){
    d_printf("Bind Failed");
    exit(EXIT_FAILURE);
  }
//printf("Connected to %s:%d\n", HOST, PORT);

  return server_fd;
}
