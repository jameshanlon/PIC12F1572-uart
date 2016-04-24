/*
 * PIC12F1572 UART example.
 *
 * Pins:
 *                     +----+
 *                Vdd -|1  8|- Vss
 *                RA5 -|2  7|- RA0/ICSPDAT ESUART TX
 *                RA4 -|3  6|- RA1/ICSPLCK ESUART RX
 *           RA3/MCLR -|4  5|- RA2
 *                     +----+
 */

#include <xc.h>

#define _XTAL_FREQ 16000000   // Oscillator frequency.

#pragma config FOSC = INTOSC  // INTOSC oscillator: I/O function on CLKIN pin.
#pragma config WDTE = OFF     // Watchdog Timer disable.
#pragma config PWRTE = OFF    // Power-up Timer enbable.
#pragma config MCLRE = ON     // MCLR/VPP pin function is MCLR.
#pragma config CP = OFF       // Program memory code protection disabled.
#pragma config BOREN = ON     // Brown-out Reset enabled.
#pragma config CLKOUTEN = OFF // CLKOUT function is disabled; I/O or oscillator function on the CLKOUT pin.
#pragma config WRT = OFF      // Flash Memory Write protection off.
#pragma config STVREN = ON    // Stack Overflow or Underflow will cause a Reset.
#pragma config BORV = LO      // Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LVP = OFF      // High-voltage on MCLR/VPP must be used for programming.

#include <stdio.h>
#include <stdlib.h>

unsigned char uart_byte;
unsigned char uart_flag;

void init_pic() {
  OSCCON = 0b01111010; // 16 Mhz oscillator.
  ANSELA = 0;          // Analog off.
}

void init_uart() {
  TRISAbits.TRISA1 = 1; // UART RX pin an input.
  TRISAbits.TRISA0 = 0; // UART TX pin an output.

  // 9600 bps, 16 MHz oscillator.
  SPBRGH = 0x01;
  SPBRGL = 0xA0;

  // 16-bit Baud Rate Generator
  BAUDCONbits.BRG16 = 1;

  TXSTAbits.TXEN = 1; // Transmit enabled.
  TXSTAbits.SYNC = 0; // Enable asynchronous mode.
  TXSTAbits.BRGH = 1; // High speed.

  RCSTAbits.SPEN = 1; // Enable serial port.
  RCSTAbits.CREN = 1; // Enable reception.

  // Enable interrupts.
  INTCONbits.GIE = 1;
  INTCONbits.PEIE = 1;

  // Enable the receive interrupt.
  PIE1bits.RCIE = 1;

  uart_flag = 0;
}

void send_byte(unsigned char byte) {
  // Wait until no char is being held for transmission in the TXREG.
  while (!PIR1bits.TXIF) {
    continue;
  }
  // Write the byte to the transmission register.
  TXREG = byte;
}

void interrupt interrupt_handler() {
  if (PIR1bits.RCIF) {
    // EUSART receiver enabled and unread char in receive FIFO.
    if (RCSTAbits.OERR) {
      // Overrun error.
      RCSTAbits.CREN = 0;          // Clear the OERR flag.
      uart_byte = RCREGbits.RCREG; // Clear any framing error.
      RCSTAbits.CREN = 1;
    } else {
      uart_byte = RCREGbits.RCREG;
      uart_flag = 1;
    }
  }
}

int main() {
  init_pic();
  init_uart();
  // Echo loop.
  while (1) {
    if (uart_flag) {
      send_byte(uart_byte);
      uart_flag = 0;
    }
  }
  return (EXIT_SUCCESS);
}
