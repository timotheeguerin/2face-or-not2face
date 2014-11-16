#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include "helper.h"

#define PORTNUM 2345
#define MAXRCVLEN 500

void processCommand(std::string command) {
    std::cout << "Recevied command" << std::endl;
    std::vector<double> v;
    split(" ", v);
    double* argv = &v[0];
    int argc = v.size();
}

void listenSocket() {
    char msg[] = "Hello World !\n";
    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mysocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons(PORTNUM);           /* set the server port number */

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    /* bind serv information to mysocket */
    if (bind(mysocket, (struct sockaddr *) &serv, sizeof(struct sockaddr)) == -1) {
        std::cerr << "Error binding\n";
    }

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(mysocket, 1);
    int consocket = accept(mysocket, (struct sockaddr *) &dest, &socksize);
    int len;
    std::string message = "";
    char buffer[MAXRCVLEN + 1];
    while (consocket) {
        printf("Incoming connection from %s - sending welcome\n", inet_ntoa(dest.sin_addr));
        do {
            len = recv(consocket, buffer, MAXRCVLEN, 0);
            buffer[len] = '\0';
            message += buffer;
        } while (len >= MAXRCVLEN);

        processCommand(message);

        printf("Received %s (%d bytes).\n", buffer, len);
        close(consocket);
        consocket = accept(mysocket, (struct sockaddr *) &dest, &socksize);
    }

    close(mysocket);
}

void sendMessage(std::string message) {
    char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
    int len, mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest, 0, sizeof(dest));                /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
    dest.sin_port = htons(PORTNUM);                /* set destination port number */

    if (::connect(mysocket, (struct sockaddr *) &dest, sizeof(struct sockaddr)) == -1) {
        std::cerr << "Error connecting to server";
    }
    send(mysocket, message.c_str(), strlen(message.c_str()), 0);
    len = recv(mysocket, buffer, MAXRCVLEN, 0);

    /* We have to null terminate the received data ourselves */
    buffer[len] = '\0';

    printf("Received %s (%d bytes).\n", buffer, len);

    close(mysocket);
}