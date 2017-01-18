
#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>

// note: See the below links for good PIT 8253/8253 programming information
// http://www.intel-assembler.it/portale/5/Programming-the-Intel-8253-8354-pit/howto-program-intel-pit-8253-8254.asp
// ftp://ftp.acer.fr/gpl/AS1800/linux-2.4.27/arch/i386/kernel/time.c
// https://www.kernel.org/doc/Documentation/virtual/kvm/timekeeping.txt
// http://wiki.osdev.org/PIT
// http://wiki.osdev.org/PC_Speaker
// http://ubuntuforums.org/archive/index.php/t-2024049.html
// http://tldp.org/HOWTO/IO-Port-Programming-4.html

// Compiling and setting the bits for the executable.
// gcc -o LAB5 LAB5.c
// sudo chown root:root LAB5
// sudo chmod u+s LAB5

void get_port_perms();
void play_music(int music_notes[4]);
int read_timer();
void set_timer_reload(int * lowbyte, int * highbyte);
void init_timer_count(int lowbyte, int highbyte);
void timer_loop(int lowbyte, int highbyte, FILE * tdata);

int main(int argc, char * argv[], char * env[])
{
    int i = 0, j = 0;   // tracking variables
    FILE * tdata = 0;   // file pointer for file to write timer data to 
    int lowbyte = 0, highbyte = 0;  // bytes for the reload value of the timer 2 reload register
    tdata = fopen("/home/joseph/Desktop/ECE_477/timer_data.dat", "r+"); // open file for writing timer data (erase old info)
    int music_notes[4] = {440, 330, 660, 220};     // notes for a little song!
    
    get_port_perms();     // self-explanatory
    play_music(music_notes);    // self-explanatory
 
    set_timer_reload(&lowbyte, &highbyte);
    outb(inb(0x61) & 0b11111100, 0x61); // deactivate timer so it stops counting (also the speaker)
    outb(0xb0, 0x43);	//Send command 0b10110000 to the Control Word register
                        // (channel 2, low/high byte r/w, counter mode 0, binary 16-bit mode)
    timer_loop(lowbyte, highbyte, tdata);   // gather the timer data for all the different usleep trials
    return 0;
}

void timer_loop(int lowbyte, int highbyte, FILE * tdata)
{
    int i = 0, j = 0;
    double dt = 0;
    int pre_ct = 0, aft_ct = 0;
    for(i = 1; i < 201; i++ ) {   // microsecond amounts loop
        for(j = 1; j < 101; j++) {   // iterations at a given microsecond amount
           init_timer_count(lowbyte, highbyte);
           pre_ct = read_timer();
           outb(inb(0x61) | 0b11111101, 0x61); // start timer running (speaker disabled)
           usleep(i);  // sleep for the designated amount of microseconds
           outb(inb(0x61) & 0b11111100, 0x61); // stop timer (speaker disabled)
           aft_ct = read_timer();
           dt = (double)(pre_ct - aft_ct)/1.193;
           fprintf(tdata, "%d,%f,", i, dt); 
       }
    }       
}
void set_timer_reload(int * lowbyte, int * highbyte)
{
    int reload = 0;
    reload=0xffff;
    *lowbyte = reload & 0xff;	// grab the lowest 8 bits of the reload value. 
    *highbyte = (reload >> 8) & 0xff; // grab highest 8 bits of the reload value
}

void init_timer_count(int lowbyte, int highbyte) 
{
    outb(lowbyte, 0x42);	// initializing the timer
    outb(highbyte, 0x42); 
}
int read_timer()
{
    int lowin = 0;
    int highin = 0;
    int netval = 0;
    lowin = inb(0x42);          // read timer low byte
    highin = inb(0x42);         // read timer high byte
    highin = highin << 8;
    netval = highin | lowin;    
    return netval;
}

void get_port_perms()
{
    if (seteuid(0) < 0) {  // appear as root just in case I am not already  (should have uid of root due to the above commands)
        perror("seteuid"); // error check
    }
    If (ioperm(0x61, 1,1) != 0) {
perror(“ioperm 61”); //request permissions to accsess port 61, the hardware gate for the PIT
}
   if(ioperm(0x42, 2,1) != 0) {
perror(“ioperm 42 43”);
} // request permissions to use PIT timer (channel) 2 and its associated Control Word register
    seteuid(getuid());  // appear as normal user again. Real UID should be whoever is running this program (joseph)
}

void play_music(int music_notes[4])
{
    int k = 0;
    int reload =0, lowbyte = 0, highbyte = 0;
    outb(0xb6, 0x43);	//Send command 0b10110110 to the Control Word register
                        // (channel 2, low/high byte r/w, counter mode 3 (wave generator), binary 16-bit mode)
    for(k = 0; k < 4; k++) {
        reload = 1193180 / music_notes[k]; // Ratio of counter frequency to sound frequency (how big the reload value should be)
        lowbyte = reload & 0xff;	// grab the lowest 8 bits of the reload value. 
        highbyte = (reload >> 8) & 0xff; // grab highest 8 bits of the reload value
        outb(lowbyte, 0x42);	//send the low byte to channel 2 (to be stored in the reload register)
        outb(highbyte, 0x42);	//send the high byte to channel 2 (to be stored in the reload register)
        outb( inb(0x61) | 0b11, 0x61); // read port 61 current state, enable bits 0 (timer 2 hardware gate) and 1 (speaker).
        sleep(1); // have the speaker run for a second (our program sleeps while the counter-speaker combo does its thing)
        outb (inb(0x61) & 0b11111100, 0x61);        // disable speaker and timer 
    }    
    
}

/* Some additional notes and diagrams  */
/* From The Linux Documentation Project */

// An excellent PIT diagram from https://www.kernel.org/doc/Documentation/virtual/kvm/timekeeping.txt
// --------------             ----------------
//|              |           |                |
//|  1.1932 MHz  |---------->| CLOCK      OUT | ---------> IRQ 0
//|    Clock     |   |       |                |
// --------------    |    +->| GATE  TIMER 0  |
//                   |        ----------------
//                   |
//                   |        ----------------
//                   |       |                |
//                   |------>| CLOCK      OUT | ---------> 66.3 KHZ DRAM
//                   |       |                |            (aka /dev/null)
//                   |    +->| GATE  TIMER 1  |
//                   |        ----------------
//                  |
//                   |        ----------------
//                   |       |                |
//                   |------>| CLOCK      OUT | ---------> Port 61h, bit 5
//                           |                |      |
//Port 61h, bit 0 ---------->| GATE  TIMER 2  |       \_.----   ____
//                            ----------------         _|    )--|LPF|---Speaker
//                                                    / *----   \___/
//Port 61h, bit 1 -----------------------------------/

// C/O http://www.intel-assembler.it/portale/5/Programming-the-Intel-8253-8354-pit/howto-program-intel-pit-8253-8254.asp#ixzz44RrQILKA
//+------------------+---------------------------------------------------------
//! The Control Word !
//+------------------+
//
//              +---+---+---+---+---+---+---+---+
//              ! 7 ! 6 ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 !
//              +---+---+---+---+---+---+---+---+
//                +-+-+   +-+-+   +---+---+   +-- BCD 0 - Binary 16 bit
//                  !       !         !               1 - BCD 4 decades
//+-----------------+----+  !         !
//! Select Counter       !  !         +---------- Mode Number 0 - 5
//! 0 - Select Counter 0 !  !
//! 1 - Select Counter 1 !  !         +----------------------------+
//! 2 - Select Counter 2 !  !         ! Read/Load                  !
//+----------------------+  !         ! 0 - Counter Latching       !
//                          +---------+ 1 - Read/Load LSB only     !
//                                    ! 2 - Read/Load MSB only     !
//                                    ! 3 - Read/Load LSB then MSB !
//                                    +----------------------------+
//
//
//MODE 3 - Square Wave Rate Generator
//-----------------------------------
//Similar to mode 2, except the ouput will remain high until one half of the
//count has been completed and then low for the other half.

//Measure the time that elapses from when the timer is enabled (immediately before a call to usleep) until the call from usleep returns.  Measure the time for 100 calls to usleep for integer values between 1 and 200 microseconds (a total of 100*200=20000 data points).  Each sample should be recorded in a line of a comma separated file containing an integer argument to usleep, a comma, and a double time in microseconds taken.  These should be imported into a spreadsheet or matrix analysis program.  Your lab report should include an analysis of the data gathered.


    /* Note from The Linux Documentation Project */
    /* You are not required to explicitly drop your port access privileges with ioperm(..., 0) at the end of your program; this is done automatically as the process exits. */ 