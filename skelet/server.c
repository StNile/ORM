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
#include<stdlib.h>
#include<unistd.h>


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015
#define SOCKETERROR (-1)

static int clients = 0;
static pthread_mutex_t lock;

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
    int read_size;

    //Receive a message from client
    if(read_size = recv(client_sock , &client_message , DEFAULT_BUFLEN , 0) > 0)
    {
      puts("Client message:");
      puts(client_message);
    } 

    

    pthread_mutex_lock(&lock);
    clients++;
    pthread_mutex_unlock(&lock);

    //neko formatiranje verovatno
    
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock1 ,client_sock2 , addr_size;
    struct sockaddr_in server , client;

   
    //Create socket
    check(socket_desc = socket(AF_INET , SOCK_STREAM , 0),
            "Failed to create socket");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //za spamovanje ./server-a
    const int enable = 1;
	  setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const void*) &enable, sizeof(enable));
    //Bind
    check(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)),
            "Bind failed !");

    //Listen
    listen(socket_desc , 3);

    printf("Waiting for connections...\n");
    //wait for , and eventually accept incoming connection
    addr_size = sizeof(struct sockaddr_in);
    check(client_sock1 = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&addr_size),
            "Acept failed");

    //SLANJE IP ADRESE
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client(1) IP address: %s\n", client_ip);
    //printf("Connected!\n");

    check(client_sock2 = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&addr_size),
             "Acept failed");

    //SLANJE IP ADRESE
    inet_ntop(AF_INET, &(client.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client(2) IP address: %s\n", client_ip);
        
    printf("Accpeted both clients\n");

    //start message for clients
    char* start_msg = "START!\n";
    check(send(client_sock1 , start_msg , sizeof(start_msg),0),
          "failed to send msg");
    check(send(client_sock2 , start_msg , sizeof(start_msg),0),
          "failed to send msg");


    //thread for formmating first client arugments
    pthread_t t1;
    int *pclient1 = malloc(sizeof(int));
    *pclient1 = client_sock1;
    pthread_create(&t1, NULL, handle_connection, pclient1);
    pthread_join(t1, NULL);

    //thread for formmating second client arugments
    pthread_t t2;
    int *pclient2 = malloc(sizeof(int));
    *pclient2 = client_sock2;
    pthread_create(&t2, NULL, handle_connection, pclient2);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock);


    //variables for server msg
    int size;
    char msg1[DEFAULT_BUFLEN] = "DOSLO JE DO GRESKE!", msg2[DEFAULT_BUFLEN] = "DOSLO JE DO GRESKE!";
    char z = 'z';

    //check something


    //sprintf koriscenje
    sprintf(msg1,"Ovako se koristi sprintf %c\n",z);
    sprintf(msg2 ,"Jesi skontao idiote\n");
      
    //send server solution
    check(send(client_sock1 , msg1 , strlen(msg1)+1, 0),
               "Send Failed");
    check(send(client_sock2 , msg2 , strlen(msg1)+1, 0),
               "Send Failed");
    
    close(client_sock1);
    close(client_sock2);
    close(socket_desc);
    
    return 0;
}