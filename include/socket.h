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
#include "docopt.h"
#include "face_matcher.h"

#define PORTNUM 2352
#define MAXRCVLEN 500

int server_socket;

class Server {
private:
    FaceMatcher matcher;
public:

    Server(std::string gallery_folder) {
        std::cout << "Launching server..." << std::endl;
        if (gallery_folder == "") {
            std::cout << "Loading save" << std::endl;
            matcher.load();
        }
        else {
            matcher.train(gallery_folder);
            matcher.save();
        }
    }

    std::string processCommand(std::string command) {
        int argc;
        char **argv;
        stringToArgcArgv(command, &argc, &argv);
        std::map<std::string, docopt::value> args = get_docopt_args(argc, argv);
        if (args["<picture>"]) {
            std::string filename = args["<picture>"].asString();
            if (Helper::fileExists(filename)) {
                return std::to_string(matcher.predict(args["<picture>"].asString()));
            } else {
                return "Error file doesn't exist!";
            }
        }
        return "";
    }

    static void onQuit() {
        std::cout << "Closing socket!" << std::endl;
        close(server_socket);
    }

    void listenSocket() {
        char msg[] = "Hello World !\n";
        struct sockaddr_in dest; /* socket info about the machine connecting to us */
        struct sockaddr_in serv; /* socket info about our server */
        socklen_t socksize = sizeof(struct sockaddr_in);

        memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
        serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
        serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
        serv.sin_port = htons(PORTNUM);           /* set the server port number */

        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        /* bind serv information to mysocket */
        if (bind(server_socket, (struct sockaddr *) &serv, sizeof(struct sockaddr)) == -1) {
            std::cerr << "Error binding\n";
            return;
        }
        atexit(onQuit);

        /* start listening, allowing a queue of up to 1 pending connection */
        listen(server_socket, 1);
        printf("Server listening to port %d\n", PORTNUM);
        int consocket = accept(server_socket, (struct sockaddr *) &dest, &socksize);
        int len;
        std::string message = "";
        char buffer[MAXRCVLEN + 1];
        while (consocket) {
            printf("Incoming connection from %s - sending welcome\n", inet_ntoa(dest.sin_addr));
            do {
                if (len < MAXRCVLEN) {
                    message = "";
                }
                len = (int) recv(consocket, buffer, MAXRCVLEN, 0);
                buffer[len] = '\0';
                message += buffer;
            } while (len >= MAXRCVLEN);

            std::string output = processCommand(message);
            send(consocket, output.c_str(), strlen(output.c_str()), 0);

            close(consocket);
            consocket = accept(server_socket, (struct sockaddr *) &dest, &socksize);
        }

        close(server_socket);
    }


};

std::string sendMessage(std::string message) {
    char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
    int len, mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest, 0, sizeof(dest));                /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
    dest.sin_port = htons(PORTNUM);                /* set destination port number */

    if (::connect(mysocket, (struct sockaddr *) &dest, sizeof(struct sockaddr)) == -1) {
        std::cout << "The server seems not to be started(It might have crashed). Please use facerecognition -s to start it!\n";
        std::cerr << "";
        Server server("");
        std::cout << "------------------------------------------------" << std::endl;
        return server.processCommand(message);
    }
    send(mysocket, message.c_str(), strlen(message.c_str()), 0);
    len = recv(mysocket, buffer, MAXRCVLEN, 0);
    /* We have to null terminate the received data ourselves */
    buffer[len] = '\0';

    close(mysocket);

    return std::string(buffer);
}