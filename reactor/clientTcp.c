#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("%s %s\n", argv[1], argv[2]);
        exit(0);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket");
        exit(0);
    }

    const char *ip = argv[1];

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(2480);
    int len = sizeof(struct sockaddr_in);
    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0){
        perror("connect");
        exit(0);
    }

    char buffer[128] = {0};
    sprintf(buffer, "%s", "send --------->");

    char outbuffer[128] = {0};
    while (1){

        int len = send(sockfd, buffer, 128, 0);

        len = recv(sockfd, outbuffer, 128, 0);

        sleep(2);
      
    }
    
    close(sockfd);

    return 0;
}