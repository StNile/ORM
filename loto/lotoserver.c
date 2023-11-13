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
static int players = 0;
static int klijent1brojevi[7];
static int klijent2brojevi[7];

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
    int client_message[7];
    int read_size, rand_num;
    players++;

    //Receive a message from client
    if((read_size = recv(client_sock , client_message , 7*sizeof(int) , 0)) > 0 )
    {
        printf("Client(%d) msg: %d %d %d %d %d %d %d\n",players ,client_message[0],client_message[1],client_message[2],client_message[3],client_message[4],client_message[5],client_message[6]);
    }

    if(players == 1)
    {
      for(int i = 0 ; i<7 ; i++)
      {
        klijent1brojevi[i] = client_message[i];
      }
    }
    else if(players == 2)
    {
      for(int i = 0 ; i<7 ; i++)
      {
        klijent2brojevi[i] = client_message[i];
      }
    }
    else
    {
      printf("Ne primamo vise od 2 igraca!\n");
    }

}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock1, client_sock2 , addr_size;
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
    
    //SLANJE STARTA
    char* server_message = "Start";
    check(send(client_sock1 , server_message , sizeof(server_message), 0),
             "Send Failed");
    check(send(client_sock2 , server_message , sizeof(server_message), 0),
             "Send Failed");

    //function used to recv and send back the same message
    pthread_t t1;
    int *pclient1 = malloc(sizeof(int));
    *pclient1 = client_sock1;
    pthread_create(&t1, NULL, handle_connection, pclient1);
    pthread_join(t1, NULL);

    //obrada izabranih brojeva drugog klijenta
    pthread_t t2;
    int *pclient2 = malloc(sizeof(int));
    *pclient2 = client_sock2;
    pthread_create(&t2, NULL, handle_connection, pclient2);
    pthread_join(t2, NULL);

    //DOBITNI BROJEVI
    int Result[7];
    srand(time(0));
    for(int i = 0 ; i < 7 ; i++) {Result[i] = rand()%39 + 1; }
    printf("Izabrani brojevi su: %d %d %d %d %d %d %d\n",Result[0],Result[1],Result[2],Result[3],Result[4],Result[5],Result[6]);
    
    //PROVERA BROJEVA
    int dobitak1 = 0, dobitak2 = 0;
    for(int i = 0 ; i<7 ; i++)
    {for(int j = 0 ; j <7 ; j++)
      {if(Result[j] == klijent1brojevi[i]) {dobitak1++;}
       if(Result[j] == klijent2brojevi[i]) {dobitak2++;}}}

    //FORMATIRANJE PRVOM PORUKE
    int size = snprintf(NULL, 0,"client(2) je imao %d. pogodaka\n",dobitak1);
    char* msg = (char*)malloc(size+1);
    snprintf(msg, size+1,"client(1) je imao %d. pogodaka\n",dobitak1);

    //SALJE PRVOM KLIJENTU
    printf("client(1) je imao %d. pogodaka\n",dobitak1);
    check(send(client_sock1 , msg , size+1, 0),
               "Send Failed");

    //FORMATIRANJE DRUGOM
    printf("client(2) je imao %d. pogodaka\n",dobitak2);
    snprintf(msg, size+1,"client(2) je imao %d. pogodaka\n",dobitak2);

    //SLANJE DRUGOM
    check(send(client_sock2 , msg , size+1, 0),
               "Send Failed");

    close(client_sock1);
    close(client_sock2);
    close(socket_desc);
    return 0;
}