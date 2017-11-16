//=============================================== file = projectServer.c ======
//=  A UDP based protocol that provides fast and reliable transfer            =
//=  between two hosts on the internet.                                       =
//=============================================================================
//=  Notes:                    NEED TO UPDATE                                 =
//=    1) This program conditionally compiles for Winsock and BSD sockets.    =
//=       Set the initial #define to WIN or BSD as appropriate.               =
//=    2) This program needs projectServer to be running on another host.     =
//=       Program projectServer must be started first.                        =
//=    3) This program assumes that the IP address of the host running        =
//=       udpServer is defined in "#define IP_ADDR"                           =
//=    4) The steps #'s correspond to lecture topics.                         =
//=---------------------------------------------------------------------------=
//=  Example execution:       NEED TO UPDATE                                  =
//=  (projectServer and projectClient running on host 127.0.0.1)              =
//=    Received from server: This is a reply message from SERVER to CLIENT    =
//=---------------------------------------------------------------------------=
//=  Build:                                                                   =
//=    Windows (WIN):  Borland: bcc32 projectServer.c                         =
//=                    MinGW: gcc projectServer.c -lws2_32 -o projectServer   =
//=                    Visual C: cl projectServer.c wsock32.lib               =
//=    Unix/Mac (BSD): gcc projectServer.c -lnsl -o projectSever              =
//=---------------------------------------------------------------------------=
//=  Execute:                                                                 =
//=---------------------------------------------------------------------------=
//=  Author: Esthevan Romeiro & My Nyugen                                     =
//=          University of South Florida                                      =
//=          Email: eromeiro@mail.usf.edu ; mynguyen@mail.usf.edu             =
//=---------------------------------------------------------------------------=
//=  History: (10/26/17) - Genesis (from udpServer.c)                         =
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
#include <string.h>         // Needed to handle strings

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
#define  PORT_NUM   6006            // Arbitrary port number for the server
#define  SIZE        256            // Buffer size
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

  int                  server_s;            // Server socket descriptor
  struct sockaddr_in   server_addr;         // Server Internet address
  struct sockaddr_in   client_addr;         // Client Internet address
  struct in_addr       client_ip_addr;      // Client IP address
  int                  addr_len;            // Internet address length
  char                 out_buf[4096];       // Output buffer for data
  char                 in_buf[4096];        // Input buffer for data
  int                  retcode;             // Return code
  int                  fh;                  // File handle
  int                  length;              // Length in received buffer
  char                 compare_buf[4096];   //Comparisson bufer

  //Stuff needed to make our socket timeout
  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  #ifdef WIN
    // This stuff initializes Winsock
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

  //Needed to make socket timeout
  if (setsockopt(server_s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
  {
    perror("ERROR\n");
  }

  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }

  // Open IN_FILE for file to write
  fh = fopen(fileName,"w");
  if (fh == -1)
  {
     printf("  *** ERROR - unable to create '%s' \n", RECV_FILE);
     exit(1);
  }

  // Receive and write file from projectClient
  do
  {
    printf("\nWaiting for recvfrom() to complete... \n");
    addr_len = sizeof(client_addr);

    //retcode here will timeout after 5sec and return a -1. This means no message received.
    retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);

    //Message not received if retcode < 0
    if (retcode < 0)
    {
      //Message was not received. Don't send ACK and wait to receive again.
      printf("\n*** ERROR - Did not receive message. Waiting to receive again... \n");
      retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);
      length = strlen(in_buf);
    }

    //Message received if retcode >= 0
    if(retcode >= 0)
    {
      //Message received, see if EOF exists to terminate.
      if(in_buf[0] == EOF)
      {
        //EOF received, break out and terminate program.
        printf("SERVER received an EOF. Breaking out of loop\n");
        break;
      }

      printf("Message received. Sending ACK to Client...\n");
      //sleep(6); //Simulate packet loss with sleep to trigger timout.

      // Copy the four-byte client IP address into an IP address structure
      memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

      // SEND ACK HERE
      strcpy(out_buf, "ACK!\n");
        
        
     // ======= add losing packet loss code here ---  lines 196-201================
      retcode = sendto(server_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
      if (retcode < 0)
      {
        printf("*** ERROR - sendto() failed \n");
        exit(-1);
      }
     // ======= add losing packet loss code here ---  lines 196-201================
        
        
      //Compare what is in in_buf to what we have in previous buffer.
      //If same, then don't re-write, ACK wasn't received.
      if(strcmp(compare_buf, in_buf) != 0)
      {
        //This is a new message. Proceed with writing.
        length = strlen(in_buf);
        printf("\nLength received: %d\n\nReceived from client: %s \n", length, in_buf);
        fputs(in_buf, fh);
        //Save what is in in_buf to a new buffer for comparisson later.
        strcpy(compare_buf, in_buf);
      }
      else
      {
        // Duplicate message. Client didn't get ACK. Get the new incoming message.
        printf("\nReceived duplicate.\n");
      }
    }

  } while (length > 0);


  // Close the received file
  close(fh);

  // Print an informational message of IP address and port of the client
  printf("\nIP address of client = %s  port = %d) \n", inet_ntoa(client_ip_addr),
    ntohs(client_addr.sin_port));

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
