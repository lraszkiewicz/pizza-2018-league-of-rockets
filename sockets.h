#ifndef SOCKETS_H
#define SOCKETS_H

#include <arpa/inet.h>  //inet_addr
#include <cstdio>       //printf
#include <cstdlib>
#include <cstring>  //strlen
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/socket.h>  //socket
#include <unistd.h>

struct Socket {
   bool initialized;
   int sockfd;
   FILE *input, *output;

   Socket() {
      initialized = false;
      sockfd      = -1;
   }

   ~Socket() {
      if (initialized) {
         fclose(input);
         fclose(output);
      }
   }

   void init(const char *ip, int port) {

      printf("%s:%d\n", ip, port);
      sockaddr_in server;

      // Create socket
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd == -1) {
         perror("Could not create socket: ");
         exit(1);
      }
      fputs("Socket created\n", stderr);

      int flag = 1;
      if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag)) == -1) {
         perror("Disabling Nagle failed");
         exit(1);
      }

      server.sin_addr.s_addr = inet_addr(ip);
      server.sin_family      = AF_INET;
      server.sin_port        = htons(port);
      // Connect to remote server
      if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
         perror("connect failed. Error");
         exit(1);
      }
      fputs("Connected\n", stderr);

      input = fdopen(sockfd, "r");
      if (input == NULL) {
         fprintf(stderr, "%s: fdopen(s,'r')\n", strerror(errno));
         exit(1);
      }
      output = fdopen(dup(sockfd), "w");
      if (output == NULL) {
         fprintf(stderr, "%s: fdopen(s,'w')\n", strerror(errno));
         exit(1);
      }
      setlinebuf(input);
      setlinebuf(output);
      initialized = true;
   }

   void send(const char *message) {
      ::send(sockfd, message, strlen(message), 0);
   }
};

Socket default_socket;
FILE *socket_input, *socket_output;

#define qprintf(...) fprintf(socket_output, __VA_ARGS__)?:0;
#define qscanf(...) fscanf(socket_input, __VA_ARGS__) ?: 0;
#define qgetline(...) getline(__VA_ARGS__, socket_input) ?: 0;
#define qgetdelim(...) getdelim(__VA_ARGS__, socket_input) ?: 0;

void init(const char *ip, int port) {
   default_socket.init(ip, port);
   socket_input  = default_socket.input;
   socket_output = default_socket.output;
}

struct RequestStatus {
   int code;
   size_t descriptionSize;
   char *description;

   RequestStatus() {
      code            = -1;
      descriptionSize = -1;
      description     = NULL;
   };

   ~RequestStatus() {
      if (description)
         delete[] description;
   }
};

RequestStatus getRequestStatus() {
   RequestStatus result;
   char status[100];
   qscanf("%s", status);
   if (!strcmp(status, "OK")) {
      result.code        = 0;
      result.description = (char *)malloc(strlen(status) + 1);
      strcpy(result.description, status);
      result.descriptionSize = strlen(status);
   } else if (!strcmp(status, "ERR")) {
      qscanf("%d ", &result.code);
      // qgetdelim(&result.description, &result.descriptionSize, '.');
      qgetline(&result.description, &result.descriptionSize);
   } else {
      // printf("ERROR WTF, STATUS: %s\n", status);
      const char *wtf    = "WTF";
      result.code        = -2;
      result.description = (char *)malloc(strlen(wtf) + 1);
      strcpy(result.description, wtf);
      result.descriptionSize = strlen(wtf);
      exit(42);
   }
   return result;
}

int checkOk() {
   RequestStatus rs = getRequestStatus();
   if (rs.code != 0) {
      fprintf(stderr, "ERROR %d: %s\n", rs.code, rs.description);
      // exit(1);
      return rs.code;
   }
   return 0;
}

void wait() {
   qprintf("WAIT\n");
   checkOk();
   checkOk();
}

void login(const char *login, const char *password) {
   char tmp[100];
   qscanf("%s", tmp);
   qprintf("%s\n", login);
   qscanf("%s", tmp);
   qprintf("%s\n", password);
   checkOk();
}

void loginFromFile() {
   char username[50], password[50];
   FILE *file = fopen("../login.txt", "r");
   fscanf(file, " %s %s", username, password)?:0;
   fclose(file);
   login(username, password);
}

#endif  // SOCKETS_H
