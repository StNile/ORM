/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdlib.h> //exit
#include<pthread.h> // threads

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015
#define SOCKETERROR (-1)

int check(int exp, const char* msg) {
    if(exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
}

void* handle_connection(void* p_client_sock) {
    int client_sock = *((int*)p_client_sock);
    free(p_client_sock);
    char client_message[DEFAULT_BUFLEN];
    char* server_message = "makaze";
    int read_size, rand_num;
    
    printf("Socket number: %d\n", client_sock);

    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", read_size);
        printf("Client msg: %s\n",client_message);
            
        check(send(client_sock , server_message , sizeof(client_message), 0),
                "Send Failed");
    }

}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , addr_size;
    struct sockaddr_in server , client;
   
    //Create socket
    check(socket_desc = socket(AF_INET , SOCK_STREAM , 0),
            "Failed to create socket");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    check( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)),
            "Bind failed !");

    //Listen
    listen(socket_desc , 3);

    while (1)
    {
        printf("Waiting for connections...\n");
        //wait for , and eventually accept incoming connection
        addr_size = sizeof(struct sockaddr_in);
        check(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&addr_size),
                "Acept failed");
        printf("Connected!\n");

        //function used to recv and send back the same message
        pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = client_sock;
        pthread_create(&t, NULL, handle_connection, pclient);
    }

    return 0;
}