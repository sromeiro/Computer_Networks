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
//=  discard.c          provided by Dr. Christensen                           =
//=  https://stackoverflow.com/questions/13547721/udp-socket-set-timeout      =
//============================================================================//

#define  BSD                // WIN for Winsock and BSD for BSD sockets

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <fcntl.h>          // Needed for file i/o constants
#include <string.h>         // Needed to manipulate strings
#include <ctype.h>          // Needed for something

#ifdef WIN                  // If WIN
  #include <windows.h>      // Needed for all Winsock stuff
  #include <io.h>           // Needed for open(), close(), and eof()
  #include <sys\stat.h>     // Needed for file i/o constants
#endif

#ifdef BSD                  // If BSD
  #include <sys/types.h>    // Needed for sockets stuff
  #include <netinet/in.h>   // Needed for sockets stuff
  #include <sys/socket.h>   // Needed for sockets stuff
  #include <arpa/inet.h>    // Needed for sockets stuff
  #include <fcntl.h>        // Needed for sockets stuff
  #include <unistd.h>       // for close
  #include <netdb.h>        // Needed for sockets stuff
  #include <sys/time.h>     // Needed to make socket timeout
  #include <sys/io.h>       // Needed for open(), close(), and eof()
  #include <sys/stat.h>     // Needed for file i/o constants
#endif

//============================DEFINITIONS=====================================//
#define  PORT_NUM   6006            // Port number used at the server
#define  BUFFER_SIZE 4096           // Size of buffers
#define  SIZE        512            // Size of packet
#define  SEND_FILE  "sendFile.txt"  // File name of received file
#define  DISCARD_RATE 0.02          // Discard rate (from 0.0 to 1.0)
#define  TIMEOUT 0                  // Timeout in seconds
#define  MTIMEOUT 1000              // Timeout in microseconds
#define  ATTEMPTS 100               // Number of attempts to resend packet
//========================FUNCTION PROTOTYPES=================================//
int sendFile(char *fileName, char *destIpAddr, int destPortNum);
double rand_val(void);      // LCG RNG using x_n = 7^5*x_(n-1)mod(2^31 - 1)
//================================MAIN========================================//

int main(int argc, char *argv[])
{
  char                 sendFileName[256];   // Send file name
  char                 recv_ipAddr[16];     // Reciver IP address
  int                  recv_port;           // Receiver port number

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
  sendFile(sendFileName, recv_ipAddr, recv_port);

  return 0;
}

//===========================FUNCTION DEFINITIONS=============================//

//=============================================================================
//=  Function to send a file using UDP                                        =
//=============================================================================
//=  Inputs:                                                                  =
//=    fileName ----- Name of file to open, read, and send                    =
//=    destIpAddr --- IP address or receiver                                  =
//=    destPortNum -- Port number receiver is listening on                    =
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
//=============================================================================

int sendFile(char *fileName, char *destIpAddr, int destPortNum)
{
  #ifdef WIN
    WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
    WSADATA wsaData;                              // Stuff for WSA functions
  #endif

  int                  client_s;        // Client socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  unsigned int         addr_len;        // Internet address length
  int                  multiplier1 = 1; // Adjustments for packets
  int                  multiplier2 = 1; // Adjustments for buffers
  if(DISCARD_RATE >= 0.01)
  {
    multiplier1 = 16;
    multiplier2 = 2;
  }
  if(strcmp(destIpAddr, "127.0.0.1"))
  {
    //If not local. Sending remote. Increase packet size.
    multiplier1 = 2;
  }
  char                 out_buf[BUFFER_SIZE*multiplier2];   // Output buffer
  char                 in_buf[BUFFER_SIZE*multiplier2];    // Input buffer
  int                  packetSize = SIZE*multiplier1;      // Packet size
  int                  fh;              // File handle
  int                  length;          // Length of send buffer
  int                  retcode;         // Return code
  double               random;          // Uniform random value from 0 to 1

  //Stuff needed to make our socket timeout.
  struct timeval tv;
  tv.tv_sec = TIMEOUT;
  tv.tv_usec = MTIMEOUT;

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
  #ifdef WIN
    fh = open(fileName, O_RDONLY | O_BINARY, S_IREAD | S_IWRITE);
  #endif

  #ifdef BSD
    fh = open(fileName, O_RDONLY, S_IREAD | S_IWRITE);
  #endif

  if (fh == -1)
  {
     printf("  *** ERROR - unable to open '%s' \n", fileName);
     exit(1);
  }

  // Send file to remote
  while((length = read(fh, out_buf, packetSize)) != 0)
  {
    memset(out_buf + length, 0, (packetSize - length) * sizeof(char));

    //Simulate a packet loss by getting a random value
    random = rand_val();
    if(random > DISCARD_RATE)
    {
      //Random check passed. Send packet.
      retcode = sendto(client_s, out_buf, length, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

      if (retcode < 0)
      {
        printf("*** ERROR - sendto() failed \n");
        exit(-1);
      }
    }

    //retcode here will timeout after 5sec and return a -1.
    //This means no ACK received.
    retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&server_addr, &addr_len);

    if(retcode < 0)
    {
      int i = 0; // For loop counter.
      for(i; i < ATTEMPTS; i++) // Resend ATTEMPTS times until give up
      {
        //ACK not received send it again
        random = rand_val();
        //If check passes send packet. Else lose packet.
        if(random > DISCARD_RATE)
        {
          //Random check passed. Send packet.
          retcode = sendto(client_s, out_buf, length, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
          if (retcode < 0)
          {
            printf("*** ERROR - sendto() failed \n");
            exit(-1);
          }
        }

        //Wait to receive ACK
        retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&server_addr, &addr_len);

        //Verify we got the ACK
        if(retcode > 0)
        {
          //ACK received, break out of for loop
          break;
        }
      }
    }
  } //End of main while loop

  //EOF reached. Send the last EOF character to close connection
  if(length == 0)
  {
    //Send the last EOF character to terminate this process on Server side.
    out_buf[0] = EOF;


    //Simulate packet loss with random value
    random = rand_val();
    //If check passes send packet. Else lose packet.
    if(random > DISCARD_RATE)
    {
      //Random check passed. Send packet.
      retcode = sendto(client_s, out_buf, length + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
      if (retcode < 0)
      {
        printf("*** ERROR - sendto() failed \n");
        exit(-1);
      }
    }

    //Wait to receive EOF back
    retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&server_addr, &addr_len);

    //Verify we got the ACK
    if(retcode < 0)
    {
      int i = 0; // For loop counter.
      for(i; i < ATTEMPTS * 0.10; i++) // Resend 10% of ATTEMPTS times
      {
        //EOF not received send it again
        random = rand_val();
        //If check passes send packet. Else lose packet.
        if(random > DISCARD_RATE)
        {
          //Random check passed. Send packet.
          retcode = sendto(client_s, out_buf, length + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
          if (retcode < 0)
          {
            printf("*** ERROR - sendto() failed \n");
            exit(-1);
          }
        }

        //Wait to receive EOF
        retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&server_addr, &addr_len);

        if(retcode > 0)
        {
          // EOF received, break out of for loop
          break;
        }
      }
    }
  } //End of EOF attempt

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
