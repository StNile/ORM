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
#include <time.h>
#include <stdlib.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char client1_message[DEFAULT_BUFLEN] , server_message[DEFAULT_BUFLEN];

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
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    
    int read_size;

    if((read_size =recv(sock, server_message, DEFAULT_BUFLEN , 0)) > 0)
    {
      puts("Server message:");
      puts(server_message);
    }

    int izbor;
    int buffer;
    int brojevi[7];
    srandom(time(0));
    
    printf("samostalan odabir (1.) ili randomizovani(2.) ==> ");
    scanf("%d", &izbor);

    if(izbor == 1) 
    {
      printf("Ispiste vasu kombinaciju brojeva od (1 - 39): \n");
      for(int i = 0 ; i < 7 ; i++) 
        {
          printf("broj(%d):",i+1);
          scanf("%d", &buffer);
          if(buffer > 0 && buffer < 40)
          {
                brojevi[i] = buffer;
          }
          else
          {
              printf("greska pri upisu!\n");
              i--;
          }
        }
    }
    else if(izbor == 2) {
      for(int i = 0; i < 7 ; i++)
        brojevi[i] = rand()%39 + 1;
    }
    else {
      printf("Izabrali ste nepostojucu opciju");
      return 1;
    }

    printf("Vasa kombinacija brojeva je: %d , %d , %d, %d, %d ,%d ,%d\n",brojevi[0],brojevi[1],brojevi[2],brojevi[3],brojevi[4],brojevi[5],brojevi[6]);

    if( send(sock, brojevi, 7*sizeof(int),0) < 0)
      puts("Message failed");

    
	  memset(server_message, '\0', DEFAULT_BUFLEN);
    if((read_size =recv(sock, server_message, DEFAULT_BUFLEN , 0)) > 0)
    {
      puts("Server message:");
      puts(server_message);
    }

    close(sock);

    return 0;
}

