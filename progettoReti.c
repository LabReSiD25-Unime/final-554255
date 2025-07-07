// server_http.c
// Server HTTP concorrente - Federico Celi, MAT 554255

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void *gestisci_connessione(void *arg);
void invia_risposta(int client_socket, const char *metodo);

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address; //dichiarata in libreria syssocket
    socklen_t addr_len = sizeof(address); //dimensione struttura chiamata address(variabile di tipo socklen_t)

    server_fd = socket(AF_INET, SOCK_STREAM, 0); //variabili globali(af inet comunicazione tramite ip,sock stream tipo di socket basato su tcp)
    if (server_fd == 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }
//dove deve aprire la request
    address.sin_family = AF_INET;  //
    address.sin_addr.s_addr = INADDR_ANY; //
    address.sin_port = htons(PORT); //

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { //viene utilizzata principalmente per associare un indirizzo di socket a un socket creato
        perror("Errore nel bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) { //il server va in ascolto
        perror("Errore nella listen");
        exit(EXIT_FAILURE);
    }

    printf("Server HTTP in ascolto sulla porta %d...\n", PORT); //server in ascolto 

    while (1) { //loop infinito la condizione è sempre vera
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len); //accept: per accettare il server TCP bindato con il socket
        if (client_socket < 0) {
            perror("Errore nella accept");
            continue;
        }
        //gestisce la connessione tramite thread
        pthread_t tid;
        pthread_create(&tid, NULL, gestisci_connessione, (void *)&client_socket);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
//legge la richiesta
void *gestisci_connessione(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0}; //
    read(client_socket, buffer, BUFFER_SIZE);

    printf("1.Richiesta ricevuta:\n%s\n", buffer);

    // Estrazione metodo
    char metodo[8];
    sscanf(buffer, "%s", metodo);
    invia_risposta(client_socket, metodo);
    close(client_socket);
    pthread_exit(NULL);
}

void invia_risposta(int client_socket, const char *metodo) {
    
    char risposta[BUFFER_SIZE];
    
//visualizza risposta
    if (strcmp(metodo, "GET") == 0) {
        sprintf(risposta,
                "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRisposta GET ricevuta.");
    } else if (strcmp(metodo, "POST") == 0) {
        sprintf(risposta,
                "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\n\r\nRisorsa creata con POST.");
    } else if (strcmp(metodo, "PUT") == 0) {
        sprintf(risposta,
                "HTTP/1.1 201 No Content\r\n\r\nRisorsa aggiornata con PUT.");
    } else if (strcmp(metodo, "DELETE") == 0) {
        sprintf(risposta,
                "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRisorsa eliminata.");
    } else {
        sprintf(risposta,
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMetodo non supportato.");
    }

    send(client_socket, risposta, strlen(risposta), 0);
}
