#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>
#include <regex.h>
#define BUFFER_SIZE 1024

struct HttpServer{
    int server_fd;
    struct sockaddr_in address;
    socklen_t addrlen;
};

struct HttpServer startHttpServer(){
    struct HttpServer sr = {};
    sr.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if ((sr.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        panic("Erro ao iniciar socket\n");
    } else {
        printf("Socket sucefully created\n");
    }
    int opt = 1;
    if (setsockopt(sr.server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        panic("Erro ao set socket options\n");
    } else {
        printf("Set options to socket\n");
    }
    sr.address.sin_family = AF_INET;
    sr.address.sin_addr.s_addr = INADDR_ANY;
    memset(sr.address.sin_zero, '\0', sizeof(sr.address.sin_zero));
    sr.address.sin_port = htons(8080);
    sr.addrlen = sizeof(sr.address);
    
    
    if ((bind(sr.server_fd, (struct sockaddr*) &sr.address, sr.addrlen)) == -1){
        panic("Erro ao bind socket");
    } else {
        printf("Socket binded to 8080\n");
    }
    return sr;
}

char * getFilepath(char *buffer, regmatch_t *match){
    int start = match->rm_so;
    int end = match->rm_eo;
    int length = end - 5 - start;
    char * filepath;
    filepath = (char *)malloc(length + 1);
    strncpy(filepath, &buffer[start + 5], length);
    filepath[length] = '\0';
    return filepath;
}

void *handleClient(void *arg){
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read == -1) {
        panic("Erro ao ler do cliente");
    }
    buffer[bytes_read] = '\0';
    // TODO -  Check if request is GET usign
    regex_t regex;
    regmatch_t match[2];
    regcomp(&regex, "GET /[A-Za-z]+\\.html", REG_EXTENDED | REG_ICASE);
    regexec(&regex, buffer, 1, match, 0);
    if (regexec(&regex, buffer, 2, match, 0) == 0) {
        // TODO - Extract filename from request and decode URL
        if (match[0].rm_so != -1) {
            // Guardando request dentro da variável request
            char * filepath = getFilepath(buffer, &match[0]);
            free(filepath);
        }
    } else {
        // TODO - Enviar resposta 404
        printf("Invalid request\n");
    } 
    // TODO - Get file extension 
    // TODO - Build HTPP response 
    // TODO - Send HTTP response to client
    close(client_fd); 
    regfree(&regex);
    free(buffer);
    free(arg);  
    return NULL;
}

int listenToRequestsAndAccept(struct HttpServer *server) {
    if (listen(server->server_fd, 3) == -1) {
        panic("Erro ao listen Socket\n");
    } else {
        printf("Listening to incoming requests...\n");
    }

    while (1){
        int *client_fd = malloc(sizeof(int));
        struct sockaddr_in client_address;
        socklen_t client_addrlen = sizeof(client_address);
        
        if ((*client_fd = accept(server->server_fd, (struct sockaddr *)&client_address, &client_addrlen)) == -1) {
            panic("Não foi possivel aceitar o cliente\n");
        }

        pthread_t thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if(pthread_create(&thread, &attr, (void *)handleClient, (void *)client_fd) != 0){
            panic("Erro ao criar thread\n");
        }
        pthread_attr_destroy(&attr);
        
    }
    
    return 0;
}

int stopServer(struct HttpServer *server){
    close(server->server_fd);
    return 0;
}

int main(int argc, char const* argv[]){
    printf("Starting Server...\n");
    struct HttpServer server = startHttpServer();
    printf("Server started sucefully!\n");
    listenToRequestsAndAccept(&server);
    stopServer(&server);
    printf("Server stoped\n");
    pthread_exit(NULL);
}