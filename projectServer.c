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
//=  discard.c          provided by Dr. Christensen                           =
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
  #include <unistd.h>       // for close
  #include <netdb.h>        // Needed for sockets stuff
  #include <sys/io.h>       // Needed for open(), close(), and eof()
  #include <sys/stat.h>     // Needed for file i/o constants
  #include <sys/time.h>     // Needed to make socket timeout
#endif

//============================DEFINITIONS=====================================//
#define  PORT_NUM   6006            // Arbitrary port number for the server
#define  BUFFER_SIZE 4097           // Size of buffers
#define  SIZE        512            // Size of packet
#define  RECV_FILE  "recvFile.txt"  // File name of received file
#define  DISCARD_RATE 0.0           // Discard rate (from 0.0 to 1.0)
#define  TIMEOUT 0                  // Timeout in seconds
#define  MTIMEOUT 1000            // Timeout in microseconds
#define  ATTEMPTS 100               // Number of attempts to resend packet
typedef int bool;                   // Create a bool typedef
#define true 1                      // True boolean value
#define false 0                     // False boolean value
//========================FUNCTION PROTOTYPES=================================//
int recvFile(char *fileName, int portNum);
double rand_val(void);      // LCG RNG using x_n = 7^5*x_(n-1)mod(2^31 - 1)
//=========================GLOBAL VARIABLES===================================//


//================================MAIN========================================//

int main()
{
  int                  portNum;         // Port number to receive on

  // Initialize parameters
  portNum = PORT_NUM;

  // Receive the file
  printf("Starting file transfer... \n");
  recvFile(RECV_FILE, portNum);
  //printf("File transfer is complete \n");

  //printf("\nServer program succesfully terminated\n");
  return 0;
}

//===========================FUNCTION DEFINITIONS=============================//

//=============================================================================
//=  Function to receive a file using UDP                                     =
//=============================================================================
//=  Inputs:                                                                  =
//=    fileName ----- Name of file to open, read, and send                    =
//=    portNum ------ Port number server is listening on                      =
//=============================================================================
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
  unsigned int         addr_len;            // Internet address length
  int                  multiplier1 = 1;
  int                  multiplier2 = 1;
  if(DISCARD_RATE >= 0.01)
  {
    multiplier1 = 16;
    multiplier2 = 2;
  }
  char                 out_buf[BUFFER_SIZE*multiplier2];       // Output buffer for data
  char                 in_buf[BUFFER_SIZE*multiplier2];        // Input buffer for data
  int                  packetSize = SIZE*multiplier1;
  int                  retcode;             // Return code
  int                  fh;                  // File handle
  int                  length;              // Length in received buffer
  char                 compare_buf[BUFFER_SIZE*multiplier2];   //Comparisson bufer
  double               random;              // Uniform random value from 0 to 1
  bool                 first;               // Flag to signal first round

  //Stuff needed to make our socket timeout
  struct timeval tv;
  tv.tv_sec = TIMEOUT;
  tv.tv_usec = MTIMEOUT;
  struct timeval tv1;
  tv1.tv_sec = 5;
  tv1.tv_usec = 0;

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
  if (setsockopt(server_s, SOL_SOCKET, SO_RCVTIMEO,&tv1,sizeof(tv1)) < 0)
  {
    perror("ERROR\n");
  }

  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }

  // Open IN_FILE for file to write
  fh = open(fileName, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);
  if (fh == -1)
  {
     printf("  *** ERROR - unable to create '%s' \n", RECV_FILE);
     exit(1);
  }

  first = true;

  // Receive and write file from projectClient
  do
  {
    if(first == true)
    {
      //printf("\nWaiting for FIRST recvfrom() to complete... \n");
      addr_len = sizeof(client_addr);

      //retcode here will timeout after 5sec and return a -1. This means no message received.
      memset(in_buf, 0, (packetSize) * sizeof(char)); //Clear the buffer before receiving new message
      retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);
      length = strlen(in_buf);
      if (setsockopt(server_s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
      {
        perror("ERROR\n");
      }
      first = false;
    }
    else
    {
      //printf("\nWaiting for recvfrom() to complete... \n");
      addr_len = sizeof(client_addr);

      //retcode here will timeout after 5sec and return a -1. This means no message received.
      memset(in_buf, 0, (packetSize) * sizeof(char)); //Clear the buffer before receiving new message
      retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);
      length = strlen(in_buf);
    }

    //Message not received if retcode < 0
    if (retcode < 0)
    {
      int i = 0; // For loop counter
      for(i; i < ATTEMPTS; i++) // Resend ATTEMPTS times until give up.
      {
        //Message was not received. Don't send ACK and wait to receive again.
        //printf("\n*** ERROR - Did not receive message. Waiting to receive again... \n");

        retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);

        if(retcode > 0)
        {
          //printf("\nMessage received on attempt %d\n",i+1);
          length = strlen(in_buf);
          break;
        }
      }

      if(i == ATTEMPTS - 1)
      {
        printf("\nMessage never received\n");
      }

    }

    //Message received if retcode >= 0
    if(retcode >= 0)
    {
      //Message received, see if EOF exists to terminate.
      if(in_buf[0] == EOF)
      {
        //EOF received, break out and terminate program.
        //printf("SERVER received an EOF. Breaking out of loop\n");
        break;
      }

      //printf("Sending ACK to Client...\n");
      //sleep(6); //Simulate packet loss with sleep to trigger timout.

      // Copy the four-byte client IP address into an IP address structure
      memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

      // SEND ACK HERE
      strcpy(out_buf, "ACK!\n");

      //Simulate a packet loss by getting a random value
      random = rand_val();
      //printf("\nRandom = %f\n", random);
      if(random > DISCARD_RATE)
      {
        //Random check passed. Send ACK.
        retcode = sendto(server_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (retcode < 0)
        {
          printf("*** ERROR - sendto() failed \n");
          exit(-1);
        }
      }
      else
      {
        //Random check failed. Lose ACK.
        //printf("\nACK LOST!\n");
      }

      //Compare what is in in_buf to what we have in previous buffer.
      //If same, then don't re-write, ACK wasn't received.
      if(strcmp(compare_buf, in_buf) != 0)
      {
        //This is a new message. Proceed with writing.
        length = strlen(in_buf);
        //length = retcode;
        //printf("\nLength received: %d\n\nReceived from client: \n%s \n", length, in_buf);
        write(fh, in_buf, length);
        //Save what is in in_buf to a new buffer for comparisson later.
        strcpy(compare_buf, in_buf);
      }
      else
      {
        // Duplicate message. Client didn't get ACK. Get the new incoming message.
        //printf("\nReceived duplicate.\n");
      }
    }

  } while (length > 0);


  if(in_buf[0] == EOF)
  {
    out_buf[0] == EOF;
    //printf("\nSend EOF back to acknowledge it was received by server\n");
    //Simulate a packet loss by getting a random value
    random = rand_val();
    //printf("\nRandom = %f\n", random);
    if(random > DISCARD_RATE)
    {
      //Random check passed. Send ACK.
      retcode = sendto(server_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
      if (retcode < 0)
      {
        printf("*** ERROR - sendto() failed \n");
        exit(-1);
      }
    }
    else
    {
      //Random check failed. Lose ACK.
      //printf("\nEOF LOST!\n");
    }
  }

  // Close the received file
  close(fh);

  // Print an informational message of IP address and port of the client
  //printf("\nIP address of client = %s  port = %d) \n", inet_ntoa(client_ip_addr),
    //ntohs(client_addr.sin_port));

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

//=========================================================================
//= Multiplicative LCG for generating uniform(0.0, 1.0) random numbers    =
//=   - x_n = 7^5*x_(n-1)mod(2^31 - 1)                                    =
//=   - With x seeded to 1 the 10000th x value should be 1043618065       =
//=   - From R. Jain, "The Art of Computer Systems Performance Analysis," =
//=     John Wiley & Sons, 1991. (Page 443, Figure 26.2)                  =
//=========================================================================
double rand_val(void)
{
    const long  a =      16807;  // Multiplier
    const long  m = 2147483647;  // Modulus
    const long  q =     127773;  // m div a
    const long  r =       2836;  // m mod a
    static long x = 1;           // Random int value (seed is set to 1)
    long        x_div_q;         // x divided by q
    long        x_mod_q;         // x modulo q
    long        x_new;           // New x value

    // RNG using integer arithmetic
    x_div_q = x / q;
    x_mod_q = x % q;
    x_new = (a * x_mod_q) - (r * x_div_q);
    if (x_new > 0)
        x = x_new;
    else
        x = x_new + m;

    // Return a random value between 0.0 and 1.0
    return((double) x / m);
}
