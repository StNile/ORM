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

#define KAMEN 'k'
#define PAPIR 'p'
#define MAKAZE 'm'

static int clients = 0;
static char sign1;
static char sign2;
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

    char client_message;
    int read_size;

    //Receive a message from client
    check(read_size = recv(client_sock , &client_message , sizeof(char) , 0), 
    "failed to receive");

    pthread_mutex_lock(&lock);
    clients++;
    pthread_mutex_unlock(&lock);
    if(clients == 1) {sign1 = client_message;}
    else if(clients == 2) {sign2 = client_message;}
    else {printf("Los upis");}
    
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

    check(client_sock2 = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&addr_size),
             "Acept failed");

    char* start_msg = "START!\n";
    check(send(client_sock1 , start_msg , sizeof(start_msg),0),
          "failed to send msg");
    check(send(client_sock2 , start_msg , sizeof(start_msg),0),
          "failed to send msg");


    //function used to recv and send back the same message
    pthread_t t1;
    int *pclient1 = malloc(sizeof(int));
    *pclient1 = client_sock1;
    pthread_create(&t1, NULL, handle_connection, pclient1);
    pthread_join(t1, NULL);

    //thread for client 2
    pthread_t t2;
    int *pclient2 = malloc(sizeof(int));
    *pclient2 = client_sock2;
    pthread_create(&t2, NULL, handle_connection, pclient2);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock);

    int size;
    char msg1[DEFAULT_BUFLEN] = "DOSLO JE DO GRESKE!", msg2[DEFAULT_BUFLEN] = "DOSLO JE DO GRESKE!";
     printf("client(1):%c\n",sign1);
     printf("client(2):%c\n",sign2);


    if(sign1 == sign2)
    {
        printf("Nereseno je, oba klijenta su izbarala isti ruku\n");
        sprintf(msg1,"Nereseno je, oba klijenta su izbarala isti ruku\n");
        sprintf(msg2 ,"Nereseno je, oba klijenta su izbarala isti ruku\n");
    }
    if(sign1 == KAMEN)
    {
        if(sign2 == PAPIR)
        {
        printf("igrac1 je izgubio od igraca2\n");
        sprintf(msg1,"Izgubili ste drugi igrac je izabrao papir\n");
        sprintf(msg2 ,"Pobedili ste drugi igrac je izabrao kamen\n");
        }
        if(sign2 == MAKAZE)
        {
        printf("igrac2 je izgubio od igraca1\n");
        sprintf(msg1,"Pobedili ste drugi igrac je izabrao makaze\n");
        sprintf(msg2 ,"Izgubili ste drugi igrac je izabrao kamen\n")  ;          
        }
    }
    if(sign1 == PAPIR)
    {
        if(sign2 == KAMEN)
        {
        printf("igrac1 je izgubio od igraca2\n");
        sprintf(msg1,"Pobedili ste drugi igrac je izabrao kamen\n");
        sprintf(msg2 ,"Izgubili ste drugi igrac je izabrao papir\n");
        }
        if(sign2 == MAKAZE)
        {
        printf("igrac2 je izgubio od igraca1\n");
        sprintf(msg1,"Izgubili ste drugi igrac je izabrao makaze\n");
        sprintf(msg2 ,"Pobedili ste drugi igrac je izabrao papir\n")  ;          
        }
    }
    if(sign1 == MAKAZE)
    {
        if(sign2 == PAPIR)
        {
        printf("igrac1 je izgubio od igraca2\n");
        sprintf(msg1,"Pobedili ste drugi igrac je izabrao papir\n");
        sprintf(msg2 ,"Izgubili ste drugi igrac je izabrao makaze\n");
        }
        if(sign2 == KAMEN)
        {
        printf("igrac2 je izgubio od igraca1\n");
        sprintf(msg1,"Izgubili ste drugi igrac je izabrao kamen\n");
        sprintf(msg2 ,"Pobedili ste drugi igrac je izabrao makaze\n")  ;          
        } 
    }
      

    check(send(client_sock1 , msg1 , strlen(msg1)+1, 0),
               "Send Failed");
    check(send(client_sock2 , msg2 , strlen(msg1)+1, 0),
               "Send Failed");
    
    

    close(client_sock1);
    close(client_sock2);
    close(socket_desc);
    
    return 0;
}