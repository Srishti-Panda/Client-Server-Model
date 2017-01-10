/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <string.h>
#include "netproto.pb.h"
#include <arpa/inet.h> // For inet_pton (), inet_ntop ().
using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(1);
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
    vector<bot_info *>p;
    int num,i,j,k,*nb;
    bot_info *temp;
    cout<<"Enter number of bots\n";
    cin>>num;
    nb=(int *)malloc(sizeof(int));
    *nb=num;
    for(i=0;i<num;i++)
    {
        temp=(bot_info *)malloc(sizeof(bot_info));
        p.push_back(temp);
    }
    int sockfd, newsockfd[num], portno;
    socklen_t clilen;
    sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    error("ERROR opening socket 1");

    memset((char *) &serv_addr,0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    n = inet_pton (AF_INET, "127.0.0.1", &serv_addr.sin_addr); /* To * type conversion of the pointer here. */
    if (n <= 0)
    {
        perror ("Server Presentation to network address conversion.\n");
        exit (1);
    }

    if (bind(sockfd, (sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    else printf("Bound\n");
    listen(sockfd,1);
    clilen = sizeof(cli_addr);

    for(i=0;i<num;i++)
    {
        newsockfd[i] = accept(sockfd, (sockaddr *) &cli_addr, &clilen);
        if (newsockfd[i] < 0)
        error("ERROR on accept");
        else printf("Accepted client sock %d\n",i);
    }

    fd_set rd, wr, er;

    struct timeval tm;
    tm.tv_sec = 1000;
    tm.tv_usec = 0;

    while(1)
    {
        FD_ZERO(&rd);
        FD_ZERO(&wr);
        FD_ZERO(&er);
        for(i=0;i<num;i++)
        {
            FD_SET(newsockfd[i],&rd);
        }
        printf ("Select block starts:...\n");
        n = select (100, &rd, &wr, &er, &tm);
        if (n == -1)
        {
            perror ("Select failed: ");
            exit (1);
        }
        printf ("Select block ends:...\n");
        printf ("Number of set fds obtained by select() = %d\n", n);

        for(i=0;i<num;i++)
        {
            if (FD_ISSET (newsockfd[i], &rd))
            {
                memset(p[i], 0, sizeof(bot_info));
                n = read(newsockfd[i],p[i], sizeof(bot_info));
                if(n < 0)
                {
                    perror("Server read failed\n");
                    exit(1);
                }
                else printf("Data received with size: %d\n", (int)sizeof(*p[i]));
                p[i]->x=p[i]->x+(p[i]->vx)*(p[i]->t);
                p[i]->y=p[i]->y+(p[i]->vy)*(p[i]->t);
                n=write(newsockfd[i],nb,sizeof(int));
                for(j=0;j<num;j++)
                {
                    n = write (newsockfd[i], p[j], sizeof(bot_info));
                    if (n == -1)
                    {
                        perror ("Server write failed");
                        exit (1);
                    }
                    else printf("Data sent with size: %d\n", (int)sizeof(*p[i]));
                    printf ("Message obtained from child with fd = %d\n", newsockfd[i]);
                }
            }
        }
    }
    printf ("Final sleep\n");
    sleep (10);
    printf ("Server exiting\n");
    for(i=0;i<num;i++)
    {
        close(newsockfd[i]);
    }
    close(sockfd);
    return 0;
}
