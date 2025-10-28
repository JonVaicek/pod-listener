#include <iostream>
#include <cstring> 
#include <cstdlib>
#include <fstream>
// socket listener
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024
#define PORT 10501


bool isWSL() {
    std::ifstream file("/proc/version");
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        file.close();
        return line.find("Microsoft") != std::string::npos || 
               line.find("WSL") != std::string::npos;
    }
    return false;
}


int tcp_server(void) {
    int sockfd, newsockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Allow immediate reuse of port
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Bind to localhost:10501
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "TCP Server listening on 127.0.0.1:" << PORT << std::endl;

    socklen_t len = sizeof(cliaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    if (newsockfd < 0) {
        perror("accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected." << std::endl;

    // Read data
    int n = read(newsockfd, buffer, MAXLINE - 1);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "Client: " << buffer << std::endl;
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}

int udp_server(void){
    int sockfd; 
    char buffer[MAXLINE]; 
    const char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    //Enable broadcast option
    int broadcast = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        perror("setsockopt SO_BROADCAST failed");
        exit(EXIT_FAILURE);
    }
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    socklen_t len;
  int n; 
  
    len = sizeof(cliaddr);  //len is value/result 
  
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                0, ( struct sockaddr *) &cliaddr, 
                &len); 
    buffer[n] = '\0'; 
    printf("Client : %s\n", buffer); 
    sendto(sockfd, (const char *)hello, strlen(hello),  
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
            len); 
    std::cout<<"Hello message sent."<<std::endl;  
      
    return 0; 
}


int main(void){

    std::cout << "Hello World\n";
    if(isWSL()){
        std::cout << "Running on WSL\n";
        tcp_server();
    }
    else{
        std::cout << "Running on Linux\n";
        udp_server();
    }
        
    //
    return 0;
}