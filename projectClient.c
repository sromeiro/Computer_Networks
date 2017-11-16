//=============================================== file = projectClient.c =======
//=  A UDP based protocol that provides fast and reliable transfer            =
//=  between two hosts on the internet.
//=============================================================================
//=  Notes:                    NEED TO UPDATE                                 =
//=    1) This program conditionally compiles for Winsock and BSD sockets.    =
//=       Set the initial #define to WIN or BSD as appropriate.               =
//=    2) This program needs projectServer to be running on another host.     =
//=       Program udpServer must be started first.                            =
//=    3) This program assumes that the IP address of the host running        =
//=       udpServer is defined in "#define IP_ADDR"                           =
//=    4) The steps #'s correspond to lecture topics.                         =
//=---------------------------------------------------------------------------=
//=  Example execution:       NEED TO UPDATE                                  =
//=  (udpServer and udpClient running on host 127.0.0.1)                      =
//=    Received from server: This is a reply message from SERVER to CLIENT    =
//=---------------------------------------------------------------------------=
//=  Build:                                                                   =
//=    Windows (WIN):  Borland: bcc32 projectClient.c                         =
//=                    MinGW: gcc projectClient.c -lws2_32 -o projectClient   =
//=                    Visual C: cl projectClient.c wsock32.lib               =
//=    Unix/Mac (BSD): gcc projectClient.c -lnsl -o projectClient             =
//=---------------------------------------------------------------------------=
//=  Execute:                                                                 =
//=---------------------------------------------------------------------------=
//=  Author: Esthevan Romeiro & My Nyugen                                     =
//=          University of South Florida                                      =
//=          Email: eromeiro@mail.usf.edu ; mynguyen@mail.usf.edu             =
//=---------------------------------------------------------------------------=
//=  History: (10/26/17) - Genesis (from udpClient.c)                         =
//=---------------------------------------------------------------------------=
//=  Sources: Code snippets and functionality taken from the following:       =
//=  udpClient.c        provided by Dr. Christensen                           =
//=  udpServer.c        provided by Dr. Christensen                           =
//=  tcpClient.c        provided by Dr. Christensen                           =
//=  tcpServer.c        provided by Dr. Christensen                           =
//=  tcpFileSend.c      provided by Dr. Christensen                           =
//=  tcpFileRecv.c      provided by Dr. Christensen                           =
//=  https://stackoverflow.com/questions/13547721/udp-socket-set-timeout      =
//============================================================================//

#define  BSD                // WIN for Winsock and BSD for BSD sockets

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <fcntl.h>          // Needed for file i/o constants
#include <string.h>
#include <ctype.h>

#ifdef WIN                  // If WIN
  #include <windows.h>      // Needed for all Winsock stuff
#endif

#ifdef BSD                  // If BSD
  #include <sys/types.h>    // Needed for sockets stuff
  #include <netinet/in.h>   // Needed for sockets stuff
  #include <sys/socket.h>   // Needed for sockets stuff
  #include <arpa/inet.h>    // Needed for sockets stuff
  #include <fcntl.h>        // Needed for sockets stuff
  #include <netdb.h>        // Needed for sockets stuff
  #include <sys/time.h>     // Needed to make socket timeout
#endif

//============================DEFINITIONS=====================================//
#define  PORT_NUM   6006    // Port number used at the server
#define  SIZE        256    // Buffer size
//========================FUNCTION PROTOTYPES=================================//
int sendFile(char *fileName, char *destIpAddr, int destPortNum);
//=========================GLOBAL VARIABLES===================================//


//================================MAIN========================================//

int main(int argc, char *argv[])
{
  char                 sendFileName[256];   // Send file name
  char                 recv_ipAddr[16];     // Reciver IP address
  int                  recv_port;           // Receiver port number
  int                  retcode;             // Return code


  strcpy(sendFileName, "sendFile.txt");
  strcpy(recv_ipAddr, "127.0.0.1");
  recv_port = PORT_NUM;
/*
  if (argc != 4)
  {
    printf("usage: 'tcpFileSend sendFile recvIpAddr recvPort' where        \n");
    printf("       sendFile is the filename of an existing file to be sent \n");
    printf("       to the receiver, recvIpAddr is the IP address of the    \n");
    printf("       receiver, and recvPort is the port number for the       \n");
    printf("       receiver where tcpFileRecv is running.                  \n");
    return(0);
  }

  strcpy(sendFileName, argv[1]);
  if(sendFileName == NULL)
  {
    //Show error if file could not be opened
    fprintf(stderr, "Could not open \"%s\"\n", argv[1]);
    return 1;
  }
  strcpy(recv_ipAddr, argv[2]);
  recv_port = atoi(argv[3]);
*/
  // Send the file
  printf("Starting file transfer... \n");
  retcode = sendFile(sendFileName, recv_ipAddr, recv_port);
  printf("File transfer is complete \n");

  //Successful program termination
  printf("\nClient program succesfully terminated\n");
  return 0;
}

//===========================FUNCTION DEFINITIONS=============================//
int sendFile(char *fileName, char *destIpAddr, int destPortNum)
{
  #ifdef WIN
    WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
    WSADATA wsaData;                              // Stuff for WSA functions
  #endif

    int                  client_s;        // Client socket descriptor
    struct sockaddr_in   server_addr;     // Server Internet address
    int                  addr_len;        // Internet address length
    char                 out_buf[4096];   // Output buffer for data
    char                 in_buf[4096];    // Input buffer for data
    int                  fh;              // File handle
    int                  length;          // Length of send buffer
    int                  retcode;         // Return code
    char                 eof;             // To hold the EOF character

    //Stuff needed to make our socket timeout.
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

  #ifdef WIN
    // This stuff initializes winsock
    WSAStartup(wVersionRequested, &wsaData);
  #endif

  // Create a client socket
  client_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }
  // Fill-in the server's address information and do a connect
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(destPortNum);
  server_addr.sin_addr.s_addr = inet_addr(destIpAddr);

  //Needed to make socket timeout
  if (setsockopt(client_s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
  {
    perror("ERROR\n");
  }

  // Open file to send
  fh = fopen(fileName,"r");
  if (fh == -1)
  {
     printf("  *** ERROR - unable to open '%s' \n", sendFile);
     exit(1);
  }

  // Send file to remote
  while(eof != EOF)
  {
    fgets(out_buf, SIZE, fh);
    eof = fgetc(fh); //FIXES DOUBLE SEND
    ungetc(eof, fh); //Need to put character back in the buffer for next read.
    printf("\nReady to send the following:\n%s\n", out_buf);
    //sleep(1); //Simulate packet loss with sleep to trigger timeout.

    retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retcode < 0)
    {
      printf("*** ERROR - sendto() failed \n");
      exit(-1);
    }

    //retcode here will timeout after 5sec and return a -1. This means no ACK received.
    retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&server_addr, &addr_len);
    printf("Received:\n%s\nRetcode: %d\n",in_buf, retcode); //Verify we got the ACK

    if(retcode < 0)
    {
      //ACK not received send it again
      printf("\n*** ERROR - No ACK!...Resending...\n");
      retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

  }

  if((out_buf[0] = fgetc(fh)) == EOF)
  {
    //Send the last EOF character to terminate this process on Server side.
    printf("\nWe have an EOF character\n");
    retcode = sendto(client_s, out_buf, 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retcode < 0)
    {
      printf("*** ERROR - sendto() failed \n");
      exit(-1);
    }
  }

  // Close the file that was sent to the receiver
  close(fh);

  // Close the client socket
  #ifdef WIN
    retcode = closesocket(client_s);
    if (retcode < 0)
    {
      printf("*** ERROR - closesocket() failed \n");
      exit(-1);
    }
  #endif
  #ifdef BSD
    retcode = close(client_s);
    if (retcode < 0)
    {
      printf("*** ERROR - close() failed \n");
      exit(-1);
    }
  #endif
  #ifdef WIN
    // Clean-up winsock
    WSACleanup();
  #endif
    // Return zero
    return(0);
}
