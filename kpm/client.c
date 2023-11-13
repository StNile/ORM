/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close
#include <stdlib.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015
#define SOCKETERROR -1

int check(int exp, const char* msg) {
    if(exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
}
int main(int argc , char *argv[])
{
    int sock, read_size;
    struct sockaddr_in server;
    char server_message[DEFAULT_BUFLEN];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    check(connect(sock , (struct sockaddr *)&server , sizeof(server)),
        "Failed to connect to server");


    puts("Connected\n");

    //RECIVING START MSG


   check(read_size =recv(sock , server_message , DEFAULT_BUFLEN , 0),
        "Failed to recieve server message");
   
   puts("Server message:");
   puts(server_message);
    
    //odabir ruke
    printf("Client:\n");
    char client_message;
    printf("Izaberite kamen-papir-makaze (k-p-m): ");
    scanf("%c",&client_message);
    if(client_message != 'k' && client_message != 'p'&& client_message != 'm')
    {
        printf("nepostojeci izbor\n");
        return 1;
    }
    if(client_message == 'k') {printf("Izabrali ste kamen\n");}
    else if(client_message == 'm') {printf("Izabrali ste makaze\n");}
    else {printf("Izabrali ste papir\n");}

    check(send(sock , &client_message , sizeof(char) ,0),
          "failed to send msg");

    check(read_size =recv(sock , server_message , DEFAULT_BUFLEN , 0),
        "Failed to recieve server message");

    puts("Server message:");
    puts(server_message);

    close(sock);

    return 0;
}

