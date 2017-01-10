#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct bot_info{
    int id;
    float x;
    float y;
    float vx;
    float vy;
    float t;
};


int main(int argc, char *argv[])
{
    int sockfd, portno, n,num,*nb,i=1;
    nb=(int *)malloc(sizeof(int));
    sockaddr_in serv_addr;
    hostent *server;

    bot_info *p1;
    p1=(bot_info *)malloc(sizeof(bot_info));

    if (argc < 3)
     {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    n = inet_pton (AF_INET, "127.0.0.1", &serv_addr.sin_addr);
      if (n <= 0)
    {
        perror ("Client Presentation to network address conversion.\n");
        exit (1);
    }

    if (connect(sockfd,(sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    memset(p1,0,sizeof(bot_info));
    printf("Enter initial coordinates\n");
    scanf("%f%f",&(p1->x),&(p1->y));
    while(1)
    {
        printf("Enter velocity components\n");
        scanf("%f%f",&(p1->vx),&(p1->vy));
        printf("Enter time elapsed\n");
        scanf("%f",&(p1->t));
        n = write(sockfd,p1,sizeof(bot_info));
        if (n < 0)
            error("ERROR writing to socket\n");
        else printf("Written to server\n");
        memset(p1,0,sizeof(bot_info));
        n = read(sockfd, nb, sizeof(int));
        num=*nb;
        for(int i=0;i<num;i++)
        {
            n = read(sockfd, p1, sizeof(bot_info));
            if (n < 0)
             error("ERROR reading from socket");
            printf("Updated coordinates of Bot %d @ time %f: %f, %f\n",i,(float)p1->t,(float)p1->x,(float)p1->y);
        }
    }
    close(sockfd);
    return 0;
}
