
#include "count_binary.h"
#include "alt_types.h"
#include "system.h"
#include <time.h>

/* A "loop counter" variable. */
static unsigned char count;

// define a function that display number on an afficheur,input is the afficheur number and the number to display
static void oneAfficheurDisplay(int afficheur, int displayNumber)
{
    // define a avriable to store the seven segment number in binary
    static unsigned char segmentNumber[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, // 0-9
                                              0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};                        // A-F
    unsigned long afficheurNumber;
    afficheurNumber = IORD_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_BASE);
    switch (displayNumber)
    {
    case 0:
        afficheurNumber = segmentNumber[displayNumber] & 0xff << 23 | afficheurNumber >> 8;
        IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_BASE, afficheurNumber);
        break;
    }
}
// define a function that displays numbers on four afficheur,input is 4 numbers
static void fourAfficheurDisplay(int afficheur1, int afficheur2, int afficheur3, int afficheur4)
{
    // define a avriable to store the afficheur number in binary
    //    int
}

static void initial_message()
{
    printf("\n\n**************************\n");
    printf("* Hello from Nios II!    *\n");
    printf("* Counting from 00 to ff *\n");
    printf("**************************\n");
}

int main(void)
{
    int i;
    int __attribute__((unused)) wait_time; /* Attribute suppresses "var set but not used" warning. */
    FILE *lcd;

    count = 0;

#ifdef BUTTON_PIO_BASE
    init_button_pio();
#endif

    /* Initial message to output. */
    initial_message();

    while (1)
    {
        // IOWR_ALTERA_AVALON_PIO_DATA(0x41020, 0xffffffff);
        // usleep(100000);
        // IOWR_ALTERA_AVALON_PIO_DATA(0x41020, 0x00000000);
        // usleep(100000);

        oneAfficheurDisplay(1, count++);
    }
    return 0;
}
