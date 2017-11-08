//=============================================== file = projectServer.c =======
//=  A UDP based protocol that provides fast and reliable transfer            =
//=  between two hosts on the internet.
//=============================================================================
//=  Notes:                    NEED TO UPDATE                                 =
//=    1) This program conditionally compiles for Winsock and BSD sockets.    =
//=       Set the initial #define to WIN or BSD as appropriate.               =
//=    2) This program needs udpServer to be running on another host.         =
//=       Program udpServer must be started first.                            =
//=    3) This program assumes that the IP address of the host running        =
//=       udpServer is defined in "#define IP_ADDR"                           =
//=    4) The steps #'s correspond to lecture topics.                         =
//=---------------------------------------------------------------------------=
//=  Example execution:       NEED TO UPDATE                                  =
//=  (udpServer and udpClient running on host 127.0.0.1)                      =
//=    Received from server: This is a reply message from SERVER to CLIENT    =
//=---------------------------------------------------------------------------=
//=  Build:                   NEED TO UPDATE                                  =
//=    Windows (WIN):  Borland: bcc32 udpClient.c                             =
//=                    MinGW: gcc udpClient.c -lws2_32 -o updClient           =
//=                    Visual C: cl ucpClient.c wsock32.lib                   =
//=    Unix/Mac (BSD): gcc ucpClient.c -lnsl -o ucpClient                     =
//=---------------------------------------------------------------------------=
//=  Execute:                                                                 =
//=---------------------------------------------------------------------------=
//=  Author: Esthevan Romeiro & My Nyugen                                     =
//=          University of South Florida                                      =
//=          Email: eromeiro@mail.usf.edu ; My's email                        =
//=---------------------------------------------------------------------------=
//=  History: (10/26/17) - Genesis (from udpServer.c)                         =
//=---------------------------------------------------------------------------=
//=  Sources: Code snippets and functionality taken from the following:       =
//=  udpClient.c        provided by Dr. Christensen                           =
//=  udpServer.c        provided by Dr. Christensen                           =
//=  tcpClient.c        provided by Dr. Christensen                           =
//=  tcpServer.c        provided by Dr. Christensen                           =
//=  Homework03.c       provided by Dr. Christensen                           =
//============================================================================//

#define  BSD                // WIN for Winsock and BSD for BSD sockets

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <string.h>         // Needed to handle strings

#ifdef WIN                  // If Win
  #include <windows.h>      // Needed for all Winsock stuff
#endif

#ifdef BSD                  // If BSD
  #include <sys/types.h>    // Needed for sockets stuff
  #include <netinet/in.h>   // Needed for sockets stuff
  #include <sys/socket.h>   // Needed for sockets stuff
  #include <arpa/inet.h>    // Needed for sockets stuff
  #include <fcntl.h>        // Needed for sockets stuff
  #include <netdb.h>        // Needed for sockets stuff
#endif

//============================DEFINITIONS=====================================//
#define  PORT_NUM   1050    // Arbitrary port number for the server
#define  SIZE        256    // Buffer size
#define  RECV_FILE  "recvFile.txt"  // File name of received file

//========================FUNCTION PROTOTYPES=================================//
int recvFile(char *fileName, int portNum);

//=========================GLOBAL VARIABLES===================================//


//================================MAIN========================================//

int main()
{
  int                  portNum;         // Port number to receive on
  int                  retcode;         // Return code

  // Initialize parameters
  portNum = PORT_NUM;

  // Receive the file
  printf("Starting file transfer... \n");
  retcode = recvFile(RECV_FILE, portNum);
  printf("File transfer is complete \n");




/*
  #ifdef WIN
    WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
    WSADATA wsaData;                              // Stuff for WSA functions
  #endif
    int                  server_s;        // Server socket descriptor
    struct sockaddr_in   server_addr;     // Server Internet address
    struct sockaddr_in   client_addr;     // Client Internet address
    struct in_addr       client_ip_addr;  // Client IP address
    int                  addr_len;        // Internet address length
    char                 out_buf[4096];   // Output buffer for data
    char                 in_buf[4096];    // Input buffer for data
    int                  retcode;         // Return code

  #ifdef WIN
    // This stuff initializes winsock
    WSAStartup(wVersionRequested, &wsaData);
  #endif

  // >>> Step #1 <<<
  // Create a socket
  //   - AF_INET is Address Family Internet and SOCK_DGRAM is datagram
  server_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  // >>> Step #2 <<<
  // Fill-in my socket's address information
  server_addr.sin_family = AF_INET;                 // Address family to use
  server_addr.sin_port = htons(PORT_NUM);           // Port number to use
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
  retcode = bind(server_s, (struct sockaddr *)&server_addr,
    sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }

  // >>> Step #3 <<<
  // Wait to receive a message from client
  printf("Waiting for recvfrom() to complete... \n");
  addr_len = sizeof(client_addr);
  retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0,
    (struct sockaddr *)&client_addr, &addr_len);
  if (retcode < 0)
  {
    printf("*** ERROR - recvfrom() failed \n");
    exit(-1);
  }

  // Copy the four-byte client IP address into an IP address structure
  memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

  // Print an informational message of IP address and port of the client
  printf("IP address of client = %s  port = %d) \n", inet_ntoa(client_ip_addr),
    ntohs(client_addr.sin_port));

  // Output the received message
  printf("Received from client: %s \n", in_buf);

  // >>> Step #4 <<<
  // Send to the client using the server socket
  strcpy(out_buf, "This is a reply message from SERVER to CLIENT");
  retcode = sendto(server_s, out_buf, (strlen(out_buf) + 1), 0,
    (struct sockaddr *)&client_addr, sizeof(client_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - sendto() failed \n");
    exit(-1);
  }

  // >>> Step #5 <<<
  // Close all open sockets
  #ifdef WIN
    retcode = closesocket(server_s);
    if (retcode < 0)
    {
      printf("*** ERROR - closesocket() failed \n");
      exit(-1);
    }
  #endif
  #ifdef BSD
    retcode = close(server_s);
    if (retcode < 0)
    {
      printf("*** ERROR - close() failed \n");
      exit(-1);
    }
  #endif

  #ifdef WIN
    // This stuff cleans-up winsock
    WSACleanup();
  #endif
*/




  printf("\nServer program succesfully terminated\n");
  return 0;
}

//===========================FUNCTION DEFINITIONS=============================//
int recvFile(char *fileName, int portNum)
{
  #ifdef WIN
    WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
    WSADATA wsaData;                              // Stuff for WSA functions
  #endif

  int                  server_s;        // Server socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  struct sockaddr_in   client_addr;     // Client Internet address
  struct in_addr       client_ip_addr;  // Client IP address
  int                  addr_len;        // Internet address length
  char                 out_buf[4096];   // Output buffer for data
  char                 in_buf[4096];    // Input buffer for data
  int                  retcode;         // Return code
  int                  fh;              // File handle
  int                  length;          // Length in received buffer

  #ifdef WIN
    // This stuff initializes winsock
    WSAStartup(wVersionRequested, &wsaData);
  #endif

  // Create a welcome socket
  server_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  // Fill-in server (my) address information and bind the welcome socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNum);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  retcode = bind(server_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }

  printf("Preparing file to send...\n");

  // Open IN_FILE for file to write
  fh = fopen(fileName,"w");
  if (fh == -1)
  {
     printf("  *** ERROR - unable to create '%s' \n", RECV_FILE);
     exit(1);
  }

  // Receive ad write file from tcpFileSend
  do
  {
    printf("\n...receiving...\n");
    retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);
    if (retcode < 0)
    {
      printf("*** ERROR - recvfrom() failed \n");
      exit(-1);
    }
    printf("TEST\n");
    fputs(in_buf, fh);
    length = strlen(in_buf);
    printf("\nlength = %d\n", length);
  } while (length > 0);

  // Close the received file
  close(fh);

  // Close the welcome and connect sockets
  #ifdef WIN
    retcode = closesocket(server_s);
    if (retcode < 0)
    {
      printf("*** ERROR - closesocket() failed \n");
      exit(-1);
    }
  #endif
  #ifdef BSD
    retcode = close(server_s);
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
