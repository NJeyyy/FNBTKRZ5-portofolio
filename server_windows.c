#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>

#define PORT 8080
#define BUFFER 256

int main() {
  //creating socket
  WSADATA wsadata;
  WSAStartup(MAKEWORD(2,2), &wsadata);
  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

  //bind
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = 0;
  addr.sin_port = htons(PORT);
  bind(s, &addr, sizeof(addr));

  //listen to the created server
  listen(s, 10);

  //accept aka defining client
  SOCKET clnt = accept(s, 0, 0);
  
  //receive data
  char req[BUFFER] = {0};
  recv(clnt, req, 256, 0);

  // GET /[file]
  if (memcpm(req, "GET / ", 6) == 0) {
    FILE* f = fopen("index.html", "r");
    char buffer[BUFFER] = {0};
    fread(buffer, 1, BUFFER, f); //"buffer" is just a variable, it's defined here

    //send retrieved data (from the file) to the client
    send(clnt, buffer, BUFFER, 0);
  }
}