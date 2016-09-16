/* THEIR INCLUDES*/
#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* OUR INCLUDES*/

#ifdef DOS
#include <sys/types.h>
#include <windows.h>
#include <mysock.h>
#include <sys/timeb.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <dmedia/audio.h>
#include <signal.h>

#endif

int pnum=5113;


#include <fcntl.h>

static fd_set readers;
static struct  timeval time_out;
static int sock;
/**********************************************************************/
/*This is the  master*/

#ifdef DOS
/**********************************************************************/



void open_socket()
{
  WSADATA WSAData;
  int err, cnt;
  
  struct sockaddr_in sin;
  
  if (err = WSAStartup( 0x0101, &WSAData))  // register task with
    {                                         // winsock tcp/ip API
      printf("Startup error\n");
      exit(0);
    }
  
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("Can't open socket Err=%i", h_errno);
      exit(0);
    }
  
  
  /* This ,makes eth socket non-blocking hopefully*/
/*
  
  if (fcntl(sock, F_SETFL, FNDELAY) < 0)
  {
  perror("fcntl F_SETFL, FNDELAY");
  exit(1);
  }
  */
  
  strcpy((char *) &sin,"                ");
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr=inet_addr("144.82.46.105");
  sin.sin_port = htons(pnum);
  
  /* Connect to the server. */
  if (connect(sock, (PSOCKADDR) &sin,sizeof(sin)) < 0)
    {
      printf("Cannot connect Err=%i", h_errno);
      closesocket(sock);
      exit(0);
    }
  printf("Connection established PC\n");
  
  FD_ZERO(&readers); 
  FD_SET(sock, &readers);
  time_out.tv_sec =0;
  time_out.tv_usec =0;
}
/**********************************************************************/
void close_socket()
{
  closesocket(sock);
}
/**********************************************************************/

/*This is the  slave*/
#else 
void open_socket()
{
  int length, msgsock, cnt;
  struct sockaddr_in sin;
  
  /* Create a socket */
  
  if ((sock = socket (AF_INET,SOCK_STREAM,0)) < 0)
    {
      perror("opening stream socket");
      exit(1);
    }

  /* This ,makes eth socket non-blocking hopefully*/
/*  if (fcntl(sock, F_SETFL, FNDELAY) < 0)
    {
      perror("fcntl F_SETFL, FNDELAY");
      exit(1);
    }
    */
  /* Initialize the socket's address structure */
  
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(pnum);
  
  /* Assign an address to this socket */
  
  if (bind (sock,&sin,sizeof(sin)) < 0)
    {
      close(sock);
      perror("binding stream socket");
      exit(1);
    }
  
  /* Prepare the socket queue for connection requests */
  
  listen(sock,5);
  
  length = sizeof(sin);

  msgsock = accept(sock, &sin, &length);
  
  if (msgsock < 0)
    {
      perror("accept");
      exit(1);
    }
  
  printf("Connection from host %s, port %u\n",
	 inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
  fflush(stdout);
  close(sock);
  
  FD_ZERO(&readers); 
  FD_SET(msgsock, &readers);
  time_out.tv_sec =0;
  time_out.tv_usec =0;
  
  sock= msgsock;
}


/**********************************************************************/

void close_socket()
{
  close(sock);
}

#endif



/**********************************************************************/
int check_socket()
{
  FD_SET(sock, &readers);
  return select(FD_SETSIZE,&readers,0,0,&time_out);
}
/**********************************************************************/
int read_socket(char *line)
{
  int cnt;
  int  j=-1;
  do
    {
      j++;
      cnt = recv(sock, &line[j], 1,0);
    }
  while(line[j]!='\n');
  line[j+1]='\0';
  return j+1;
}
/**********************************************************************/
int write_socket(char *line)
{
  return send(sock,line,strlen(line),0);
}
/**********************************************************************/







