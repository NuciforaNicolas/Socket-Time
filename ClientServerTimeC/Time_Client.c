//-----------------------------------------------//
//  Esercizio Sistemi distribuiti 14/10/2019     //
//              Time_Client.c                    //
//             Nicolas Nucifora                  //
//-----------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 3233
#define BUFF 1024

int main(int argc, char** args){

    //gli argomenti devono essere 3: nome programma, inidirzzo ip host remoto, messaggio
    if(argc != 3){
        printf("Usage: server-address msg\n");
        exit(1);
    }

    //variabili socket
    int sock, retcode = 0;
    struct sockaddr_in remoteHost;
    char* msg = args[2];
    char answer[BUFF];

    //creazione socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()\n");
        exit(-1);
    }

    //inizializzazione host remoto
    memset(&remoteHost, 0, sizeof(remoteHost));
    remoteHost.sin_family = AF_INET;
    remoteHost.sin_addr.s_addr = inet_addr(args[1]);
    remoteHost.sin_port = htons(PORT);

    //inizializzazione connessione con l'host remoto
    if((connect(sock, (struct sockaddr *)&remoteHost, sizeof(remoteHost))) < 0){
        perror("connect()\n");
        exit(-1);
    }

    printf("---Connection established---\n");

    //scrittura dati nella socket
    if((retcode = write(sock, msg, strlen(msg))) < 0){
        perror("write()\n");
        exit(-1);
    }

    printf("---Message sent---\n");

    //lettura dati dalla socket
    if((retcode = read(sock, answer, BUFF-1)) < 0){
        perror("read()\n");
        exit(-1);
    }

    answer[retcode] = '\0';

    printf("Received message: %s\n", answer);
    close(sock);
}