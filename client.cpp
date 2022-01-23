#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>

#define LENGTH 4096

// Global variables
int sockfd = 0;
char name[32];
bool endGame = false;

void str_overwrite_stdout() {
  fflush(stdout);
}

void str_trim_lf(char* arr, int length) {
  for (int i = 0; i < length; i++) { 
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void send_msg_handler() {
  char message[LENGTH] = {};
  char buffer[LENGTH + 32] = {};
  fgets(message, LENGTH, stdin);
  str_trim_lf(message, LENGTH);

  sprintf(buffer, "%s: %s\n", name, message);
  send(sockfd, buffer, strlen(buffer), 0);
  bzero(message, LENGTH);
  bzero(buffer, LENGTH + 32);
}

bool isCheckMate(std::string str) {
  std::string check = "******************CHECKMATE*****************";
  if (str.find(check) != std::string::npos) {
    return true;
  }
  return false;
}

void* recv_msg_handler(void*) {
  char message[LENGTH] = {};
  int step = 0;
  int condition = true;
  while (condition) {
    if (recv(sockfd, message, LENGTH, 0) > 0) {
      if (step % 2 == 0) {
        system("clear");
      }
      if (isCheckMate(std::string(message))) {
          condition = false;
          endGame = true;
          std::string red("\033[0;31m"); //color ^_^ aww
          std::cout << red <<  message << std::endl;
          std::string white("\033[0;37m");
          std::cout << white;
      } else {
      std::cout << message << std::endl;
        }
      ++step;
    } 
    memset(message, 0, sizeof(message));
  }
  return nullptr;
}

int main(int argc, char **argv){
  if(argc != 2){
    printf("Usage: %s <port>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *ip = "127.0.0.1";
  int port = atoi(argv[1]);
  
  //signal(SIGINT, catch_ctrl_c_and_exit);
  printf("Please enter your name: ");
  fgets(name, 32, stdin);
  str_trim_lf(name, strlen(name));
  if (strlen(name) > 32 || strlen(name) < 2){
    printf("Name must be less than 30 and more than 2 characters.\n");
    return EXIT_FAILURE;
  }
  struct sockaddr_in server_addr;
  /* Socket settings */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  // Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == -1) {
    std::cout << "ERROR: connect\n";
    return EXIT_FAILURE;
  }

  send(sockfd, name, 32, 0);
  std::cout << "=== WELCOME  ===\n";
  pthread_t recv_msg_thread;
  if (pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0){
    std::cout << "ERROR: pthread\n";
    return EXIT_FAILURE;
  }

  while (1) {
    if (endGame) break;
    send_msg_handler();
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
