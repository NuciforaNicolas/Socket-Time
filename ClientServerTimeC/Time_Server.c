//-----------------------------------------------//
//  Esercizio Sistemi distribuiti 21/10/2019     //
//              Time_Server.c                    //
//             Nicolas Nucifora                  //
//-----------------------------------------------//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#define PORT 3233
#define BUFFER 2048

struct in_addr bannedClients[10]; //contiene gli indirizzi ip dei clienti bannati
int bannedIndex = 0; //n° attuale di clienti bannati


//controlla se il client attualmente connesso abbia gia' effettuato richieste precedentemente
int checkClient(struct in_addr client){
    struct in_addr c = client; //conterra' l'indirizzo ip del client da controllare
    
    //nessun cliente è stato bannato
    if(bannedIndex == 0){
        bannedClients[bannedIndex++] = c; //inserisco il client tra quelli che non potranno piu' fare richieste
        return 0;
    }
    else if(bannedIndex == 10){
        printf("Max banned clients reached\n");
        return(-1);
    }

    for(int i = 0; i < bannedIndex; i++){
        if(c.s_addr == bannedClients[i].s_addr);
            return 1; //cliente bannato
    }

    //cliente non bannato
    bannedClients[bannedIndex++] = c;
    return 0;
}

//manda un ban al client
void sendBan(int sock){
    char* answer = "BANNED";
    int retcode = 0;
    if((retcode = write(sock, answer, strlen(answer))) < 0){ //spedisce il messaggio "BANNED" al cliente bannato
        perror("write()\n");
        exit(1);
    }
    close(sock);
}


int main(int argc, char** argv){

    //variabili socket
    int welcomeSock, connectionSock, retcode, used = 1;
    struct sockaddr_in server, client;
    socklen_t clientLen;
    char msg[BUFFER];

    //inizializzazione socket di benvenuto
    if((welcomeSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()\n");
        exit(-1);
    }


    //inizializzazione server
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //controlla se è gia stato utilizzato l'indirizzo ip
    if(setsockopt(welcomeSock, SOL_SOCKET, SO_REUSEADDR, &used, sizeof(int)) < 0){
        perror("setscokop()\n");
        exit(-1);
    }

    //lega la socket all'host
    if((bind(welcomeSock, (struct sockaddr*)&server, sizeof(server))) < 0){
        perror("bind()\n");
        exit(-1);
    }

    printf("---Server listening---\n");
    listen(welcomeSock, 2); //resta in ascolto di connessioni

    clientLen = sizeof(client);

    printf("size before accept: %d\n", clientLen);

    while(1){

        //accetta una richiesta di connessione ed inizializza il client
        if((connectionSock = accept(welcomeSock, (struct sockaddr *)&client, &clientLen)) < 0){
            perror("Accept()\n");
            exit(-1);
        }

        printf("size after accept: %d\n", clientLen);

        if(checkClient(client.sin_addr) == 1){ //controlla se il cliente è bannato o meno
                sendBan(connectionSock); //spedisce il ban
                continue;
            }

        
        printf("Connection established with: %s %d\n", inet_ntoa(client.sin_addr), PORT);

        //legge dati dalla socket
        if((retcode = read(connectionSock, msg, BUFFER-1)) > 0){
            msg[retcode] = '\0';

            if((strncmp(msg, "TIME", strlen("TIME"))) == 0){ //se il messaggio ricevuto e' TIME...
                time_t t = time(NULL); //creo una variabile time_t settato con l'orario corrente
                char timeAnswer[100]; //conterra' la data in formato stringa
                strftime(timeAnswer, sizeof(timeAnswer), "%F %H:%M:%S", localtime(&t)); //permette di convertire un oggetto di tipo time_t in stringa

                if((retcode = write(connectionSock, timeAnswer, strlen(timeAnswer))) < 0){ //spedisco l'orario
                  perror("write()\n");
                  exit(2);
                }
            }
            else{
                if((retcode = write(connectionSock, "N/A", strlen("N/A"))) < 0){ //mesaggio ricevuto errato, manda N/A
                    perror("write()\n");
                    exit(3);
                }
            }

            printf("---Message sent---\n");
        }
        
        //il cliente ha chiuso la connessionei  
        if(retcode == 0)
            printf("Client closed connection\n");

        close(connectionSock);
    }
}