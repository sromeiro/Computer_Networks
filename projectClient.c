//=============================================== file = projectClient.c =======
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
//=  History: (10/26/17) - Genesis (from udpClient.c)                    =
//=============================================================================

#define  WIN                // WIN for Winsock and BSD for BSD sockets

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()

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
#define  PORT_NUM         1050   // Port number used at the server
#define  IP_ADDR    "127.0.0.1"  // IP address of server (*** HARDWIRED ***)

//========================FUNCTION PROTOTYPES=================================//


//=========================GLOBAL VARIABLES===================================//


//================================MAIN========================================//

int main()
{
  #ifdef WIN
    WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
    WSADATA wsaData;                              // Stuff for WSA functions
  #endif
    int                  client_s;        // Client socket descriptor
    struct sockaddr_in   server_addr;     // Server Internet address
    char                 out_buf[4096];   // Output buffer for data
    char                 in_buf[4096];    // Input buffer for data
    int                  retcode;         // Return code

  #ifdef WIN
    // This stuff initializes winsock
    WSAStartup(wVersionRequested, &wsaData);
  #endif








  //Successful program termination
  return 0;
}

//===========================FUNCTION DEFINITIONS=============================//
