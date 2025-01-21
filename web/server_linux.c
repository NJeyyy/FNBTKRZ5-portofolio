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
#define MAX_CLIENTS 50

// Helper function to determine MIME type based on file extension
const char* get_mime_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream"; // Default MIME type
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    return "application/octet-stream";
}

int main() {
  /*if (chdir("./web") != 0) {
    perror("Error when changing the working directory.");
    return 1;
  }*/

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

    //get methods and route
    char method[8], rt[256], mfile[256];
    sscanf(buffer, "%7s %255s", method, rt);

		//ROUTE MANAGEMENT!!=======================================
		if (strcmp(method, "GET", 3) == 0) {
			if (strcmp(rt, "/") == 0) {
				mfile = "/index.html"; //if the route is "/", then serve the index.html file by default!
      	/* mfile means main file, which what it gonna send to the server. (?) i am confusing myself */
			}
			printf("Method: %s, Route: %s\n", method, route);
		} else {
			printf("Unsupported HTTP method: %s\n", method);
			continue;
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