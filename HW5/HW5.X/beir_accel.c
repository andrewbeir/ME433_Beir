#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "i2c_master_int.h"
#include "i2c_display.h"
#include "configs.h"
#include "accel.h"

static const char ASCII[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00} // 20  (space)
    ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
    ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
    ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
    ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
    ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
    ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
    ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
    ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
    ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
    ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
    ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
    ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
    ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
    ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
    ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
    ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
    ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
    ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
    ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
    ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
    ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
    ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
    ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
    ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
    ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
    ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
    ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
    ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
    ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
    ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
    ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
    ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
    ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
    ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
    ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
    ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
    ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
    ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
    ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
    ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
    ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
    ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
    ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
    ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
    ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
    ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
    ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
    ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
    ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
    ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
    ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
    ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
    ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
    ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
    ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
    ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
    ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
    ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
    ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
    ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c �
    ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
    ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
    ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
    ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
    ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
    ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
    ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
    ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
    ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
    ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
    ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
    ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
    ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
    ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
    ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
    ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
    ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
    ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
    ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
    ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
    ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
    ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
    ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
    ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
    ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
    ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
    ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
    ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
    ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
    ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
    ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
    ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
    ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
    ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ?
    ,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f ?
};

void setup();
void reset_screen();
void wait();
int readADC();

void display_character(char acter[], int row, int col);
int getbit (int index, int j, int k);

int main(int argc, char** argv) {

    setup();
    reset_screen();

    acc_setup();
    acc_write_register(0x21,0x00);
    
    short xyz[3];
    short x; short x0; short x1; short half_x = 63;
    short y; short y0; short y1; short half_y = 31;
    short i; short j;
    short bar_width = 3;

    display_init();

    char str[] = "BEIR";

    while (1)
    {

        acc_read_register(OUT_X_L_A, (unsigned char *) xyz, 6);
        x = (127*xyz[0])/32768;
        y = (63*xyz[1])/32768;

        if (x > 0)  { x0 = half_x; x1 = half_x + x; }
        else        { x0 = half_x + x; x1 = half_x; }

        if (y > 0)  { y0 = half_y; y1 = half_y + y; }
        else        { y0 = half_y + y; y1 = half_y; }

        display_clear();

        for (i = 0; i < 127; i++) {
            if ((i > x0) && (i < x1)) {
                for (j = 0; j < bar_width; j++) {
                    display_pixel_set(half_y + j, i, 1);
                }
            }
        }

        for (i = 0; i < 63; i++) {
            if ((i > y0) && (i < y1)) {
                for (j = 0; j < bar_width; j++) {
                    display_pixel_set(i, half_x + j, 1);
                }
            }
        }
        display_character(str,0,0);
        //wait();
    }
    return (0);

}

void setup() {
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    __builtin_enable_interrupts();
    ANSELBbits.ANSB13 = 0;
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB13 = 1;
    TRISBbits.TRISB7 = 0;
    RPB15Rbits.RPB15R = 0b0101;
    T2CONbits.TCKPS = 2;
    PR2 = 19999;
    TMR2 = 0;
    OC1RS = 10000;
    OC1CONbits.OCTSEL = 0;
    OC1CONbits.OCM = 0b110;
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;
}

void reset_screen() {
    ANSELBbits.ANSB2 = 0;
    TRISBbits.TRISB2 = 0;

    LATBbits.LATB2 = 0;
    _CP0_SET_COUNT(0);
    int elapsed = 0;

    while (elapsed<2000000) {
        elapsed = _CP0_GET_COUNT();
    }

    LATBbits.LATB2 = 1;
    _CP0_SET_COUNT(0);
    elapsed = 0;

    while (elapsed<2000000){
        elapsed = _CP0_GET_COUNT();
    }
}

void wait() {
    while (1) {
        _CP0_SET_COUNT(0);
        LATBINV = 0b010000000;
        while (_CP0_GET_COUNT() < 20000000) {
            int val = readADC();
            OC1RS = val * 19999/1023;

            if (PORTBbits.RB13 == 1) {}
            else { LATBINV = 0b010000000; }
        }
    }
}

int readADC() {
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {}
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {}
    a = ADC1BUF0;
    return a;
}

void display_character(char acter[], int row, int col) {

    if (row > 64 || col > 128) {
        acter = "Error: pixel position    does not exist.";
        row = 0;
        col = 0;
    }

    int len = (int) strlen(acter);

    int i = 0;
    int j = 0;
    int k = 0;
    int ccount = 0;
    int rcount = 0;

    for (i = 0; i < len; i++) {                 // loop through all characters
        int index = (int) acter[i] - 0x20;      // find character in ASCII[][]
        for (j = 0; j < 5; j++) {               // loop through columns
            for (k = 0; k < 8; k++) {           // loop through rows
                int bit = getbit(index,j,k);
                if (bit) {
                    display_pixel_set((k+rcount*8+row),(j+ccount*5+col),1);
                }
                else {
                    display_pixel_set((k+rcount*8+row),(j+ccount*5+col),0);
                }
            }
        }
        ccount++;
        if (ccount+col/5 == 25) {               // wrap text to next line
            rcount++; ccount = 0;
        }
    }
    display_draw();
}

int getbit(int index, int j, int k) {           // return bit k of the jth byte
    return (ASCII[index][j] & (1 << (k-1))) >> (k-1);
}

