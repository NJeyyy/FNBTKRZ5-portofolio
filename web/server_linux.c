#include <stdio.h> //other thing, i think
#include <sys/socket.h> //socket
#include <string.h> //for string modification
#include <fcntl.h> //for file accessing
#include <sys/sendfile.h> //to send the file data
#include <unistd.h> //for the close thingy
#include <netinet/in.h> //for sockaddr_in
#include <sys/types.h> //it is wise, they say.. :)
#include <arpa/inet.h> //for htons(), its usually already being included in netinet/in.h, but for some. So.. good practice this if for :D

#define BUFFER 1024 //1kb, is the standard

const char *get_mime_type(const char *filename) {
  if (strstr(filename, ".html")) return "text/html";
  if (strstr(filename, ".css")) return "text/css";
  if (strstr(filename, ".js")) return "application/javascript";
  if (strstr(filename, ".png")) return "image/png";
  if (strstr(filename, ".jpg") || strstr(filename, ".jpeg")) return "image/jpeg";
  return "application/octet-stream";  // Default
}

int main() {
  //creating socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    perror("Error when creating socket..");
    return 1;
  }
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = INADDR_ANY;
  //bind
  if (bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Error when binding!");
    return 1;
  }
  //listen
  listen(serverSocket, 5);

  while (1) { //loop to keep getting requests from the client!
    //accept
    int ClientResp = accept(serverSocket, 0, 0);
		if (ClientResp < 0) {
			perror("Accept failed.");
      continue;  //if accept fails, continue to the next iteration of the loop.
		}
    char buffer[BUFFER] = {0};
    int recv_bytes = recv(ClientResp, buffer, BUFFER, 0);
		if (recv_bytes <= 0) {
      perror("Error when receiving request..");
      close(ClientResp);
      continue;
    }
    if (recv_bytes > BUFFER - 1) recv_bytes = BUFFER - 1;
    buffer[recv_bytes] = '\0'; //make sure the buffer is null terminated?

    //GET / [FILE]
    char* rt = buffer + 5; //"GET / " have 5 indexs! so imma skipping them
    *strchr(rt, ' ') = 0; //search for the " " (space) and change the space into null terminator

		//ROUTE MANAGEMENT!!=======================================
    const char *mfile = rt;
		if (strcmp(rt, "/") == 0) {
			mfile = "/index.html"; //if the route is "/", then serve the index.html file by default!
      /* mfile means main file, which what it gonna send to the server. (?) i am confusing myself */
		}

    int opend_f = open(mfile + 1, O_RDONLY); //+1 to skip the "/" :/
		if (opend_f == -1) { // 404
			const char *http_404 = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Type: text/html\r\n" //text/html which is what this gonna send.
                             "Connection: close\r\n" //close means no request will be given again for this.
                             "\r\n" //separate (using \r\n\r\n) between headers and body
                             "<html><body><h1>404<br>Not Found</h1></body></html>";
  	send(ClientResp, http_404, strlen(http_404), 0);
		} else { //if success
      const char *http_header = "HTTP/1.1 200 OK\r\n"
                                "Connection: close\r\n"
                                "Content-type: ";
      const char *mime_type = get_mime_type(mfile);
      //send header
      char header[512];
      snprintf(header, sizeof(header), "%s%s\r\n\r\n", http_header, mime_type);
      send(ClientResp, header, strlen(header), 0);
      //send file data
      sendfile(ClientResp, opend_f, 0, BUFFER);
      close(opend_f);
    }
    close(ClientResp);
  }
	close(serverSocket);
	return 0;
}