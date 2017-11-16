//============================================= file = projectDiscard.c =====
//=  Program to discard a percentage of messages                            =
//===========================================================================
//=  Notes:                                                                 =
//=    1) This program is a useful example for the class project            =
//=       to emulate a packet loss rate.                                    =
//=-------------------------------------------------------------------------=
//=  Build: gcc projectDiscard.c -o projectDiscard                          =
//=-------------------------------------------------------------------------=
//=  Execute: discard                                                       =
//=-------------------------------------------------------------------------=
//=  Author: Esthevan Romeiro & My Nyugen                                   =
//=          University of South Florida                                    =
//=          Email: eromeiro@mail.usf.edu ; mynguyen@mail.usf.edu           =
//=-------------------------------------------------------------------------=
//=  History: (10/26/17) - Genesis (from udpClient.c)                       =
//=-------------------------------------------------------------------------=
//=  Sources: Code snippets and functionality taken from the followin       =
//=  discard.c          provided by Dr. Christensen                         =
//=========================================================================//

//----- Include files -------------------------------------------------------
#include <stdio.h>               // Needed for printf()

//----- Constants -----------------------------------------------------------
#define DISCARD_RATE        0.10 // Discard rate (from 0.0 to 1.0)
#define TOTAL_MESSAGES    100000 // Total messages to print

//----- Function prototypes -------------------------------------------------
double rand_val(void);      // LCG RNG using x_n = 7^5*x_(n-1)mod(2^31 - 1)

//===========================================================================
//=  Main program                                                           =
//===========================================================================
int main(void)
{
    int    printCount;          // Counter of printf completed
    double z;                   // Uniform random value 0 to 1
    int    i;                   // Loop counter
    
    // Test loop to demonstate discard
    printCount = 0;
    // Consider how many total messages we need
    for (i=0; i<TOTAL_MESSAGES; i++)
    {
        z = rand_val();
        
        if (z > DISCARD_RATE)
        {
            printf("Message #%d \n", i);
            printCount++;
        }
    }
    
    // Output percentage of messages printed
    printf(">>> %f %% of messages printed \n",
        100.0 * (double) printCount / TOTAL_MESSAGES);
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
