//================================================== file = tcpFileSend.c =====
//=  A file transfer program using TCP - this is the file send                =
//=============================================================================
//=  Notes:                                                                   =
//=    1) This program conditionally compiles for Winsock and BSD sockets.    =
//=       Set the initial #define to WIN or BSD as appropriate.               =
//=    2) This program sends a file to tcpFileRecv                            =
//=    3) This program take command line input as shown below                 =
//=    4) Ignore build warnings on unused retcode and options.                =
//=---------------------------------------------------------------------------=
//=  Example execution: (for tcpFileSend sendFile.dat 127.0.0.1 1050)         =
//=    Starting file transfer...                                              =
//=    File transfer is complete                                              =
//=---------------------------------------------------------------------------=
//=  Build: bcc32 tcpFileSend.c or cl tcpFileSend.c wsock32.lib for Winsock   =
//=         gcc tcpFileSend.c -lnsl for BSD                                   =
//=---------------------------------------------------------------------------=
//=  Execute: tcpFileSend                                                     =
//=---------------------------------------------------------------------------=
//=  Author: Ken Christensen                                                  =
//=          University of South Florida                                      =
//=          WWW: http://www.csee.usf.edu/~christen                           =
//=          Email: christen@csee.usf.edu                                     =
//=---------------------------------------------------------------------------=
//=  History:  KJC (08/04/17) - Genesis (from tcpClient.c)                    =
//=============================================================================
#define  BSD                // WIN for Winsock and BSD for BSD sockets
//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <fcntl.h>          // Needed for file i/o constants
#include <string.h>
#include <ctype.h>

#ifdef WIN
  #include <windows.h>      // Needed for all Winsock stuff
  #include <io.h>             // Needed for open(), close(), and eof()
  #include <sys\stat.h>       // Needed for file i/o constants
#endif
#ifdef BSD
  #include <sys/types.h>    // Needed for sockets stuff
  #include <netinet/in.h>   // Needed for sockets stuff
  #include <sys/socket.h>   // Needed for sockets stuff
  #include <arpa/inet.h>    // Needed for sockets stuff
  #include <fcntl.h>        // Needed for sockets stuff
  #include <netdb.h>        // Needed for sockets stuff
  #include <sys/io.h>       // Needed for open(), close(), and eof()
  #include <sys/stat.h>     // Needed for file i/o constants
#endif
//----- Defines ---------------------------------------------------------------
#define  PORT_NUM   6066    // Port number used at the server
#define  SIZE        256    // Buffer size
#define  SEND_FILE  "sendFile.txt"  // File name of received file
//----- Prototypes ------------------------------------------------------------
int sendFile(char *fileName, char *destIpAddr, int destPortNum);
//===== Main program ==========================================================
int main(int argc, char *argv[])
{
  char                 sendFileName[256];   // Send file name
  char                 recv_ipAddr[16];     // Reciver IP address
  int                  recv_port;           // Receiver port number
  int                  options;             // Options
  int                  retcode;             // Return code
  // Usage and parsing command line arguments
  if(argc == 1)
  {
    printf("\nNo server specified. Using default IP, Port Number and File\n");
    strcpy(sendFileName, SEND_FILE);
    strcpy(recv_ipAddr, "127.0.0.1");
    recv_port = PORT_NUM;
  }
  else if (argc < 4)
  {
    printf("usage: 'tcpFileSend sendFile recvIpAddr recvPort' where        \n");
    printf("       sendFile is the filename of an existing file to be sent \n");
    printf("       to the receiver, recvIpAddr is the IP address of the    \n");
    printf("       receiver, and recvPort is the port number for the       \n");
    printf("       receiver where tcpFileRecv is running.                  \n");
    return(0);
  }
  else
  {
    strcpy(sendFileName, argv[1]);
    if(sendFileName == NULL)
    {
      //Show error if file could not be opened
      fprintf(stderr, "Could not open \"%s\"\n", argv[1]);
      return 1;
    }
    strcpy(recv_ipAddr, argv[2]);
    recv_port = atoi(argv[3]);
  }
  // Send the file
  printf("Starting file transfer... \n");
  retcode = sendFile(sendFileName, recv_ipAddr, recv_port);
  printf("File transfer is complete \n");
  // Return
  return(0);
}
//=============================================================================
//=  Function to send a file using TCP                                        =
//=============================================================================
//=  Inputs:                                                                  =
//=    fileName ----- Name of file to open, read, and send                    =
//=    destIpAddr --- IP address or receiver                                  =
//=    destPortNum -- Port number receiver is listening on                    =
//=    options ------ Options (not implemented)                               =
//=---------------------------------------------------------------------------=
//=  Outputs:                                                                 =
//=    Returns -1 for fail and 0 for success                                  =
//=---------------------------------------------------------------------------=
//=  Side effects:                                                            =
//=    None known                                                             =
//=---------------------------------------------------------------------------=
//=  Bugs:                                                                    =
//=    None known                                                             =
//=---------------------------------------------------------------------------=
int sendFile(char *fileName, char *destIpAddr, int destPortNum)
{
#ifdef WIN
  WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
  WSADATA wsaData;                              // Stuff for WSA functions
#endif
  int                  client_s;        // Client socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  char                 out_buf[4096];   // Output buffer for data
  int                  fh;              // File handle
  int                  length;          // Length of send buffer
  int                  retcode;         // Return code
#ifdef WIN
  // This stuff initializes winsock
  WSAStartup(wVersionRequested, &wsaData);
#endif
  // Create a client socket
  client_s = socket(AF_INET, SOCK_STREAM, 0);
  if (client_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }
  // Fill-in the server's address information and do a connect
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(destPortNum);
  server_addr.sin_addr.s_addr = inet_addr(destIpAddr);
  retcode = connect(client_s, (struct sockaddr *)&server_addr,
    sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - connect() failed \n");
    exit(-1);
  }
  // Open file to send
  #ifdef WIN
    fh = open(fileName, O_RDONLY | O_BINARY, S_IREAD | S_IWRITE);
  #endif

  #ifdef BSD
    fh = open(fileName, O_RDONLY, S_IREAD | S_IWRITE);
  #endif

  if (fh == -1)
  {
     printf("  *** ERROR - unable to open '%s' \n", sendFile);
     exit(1);
  }
  // Send file to remote
  while((length = read(fh, out_buf, SIZE)) != 0)
  {

    retcode = send(client_s, out_buf, length, 0);
    if (retcode < 0)
    {
      printf("*** ERROR - recv() failed \n");
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
