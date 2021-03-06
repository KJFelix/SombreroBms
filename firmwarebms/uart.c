#include <stdio.h>
#include <string.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "serial.h"

t_serialport g_serial;

void init_serial_vars(void)
{
  g_serial.RXstate = SER_STATE_IDLE;
  g_serial.inbuffer[0] = 0;
  g_serial.inindex = 0;
  g_serial.inCRC = 0;
  g_serial.TXstate = SER_STATE_IDLE;
  g_serial.outbuffer[0] = 0;
  g_serial.outindex = 0;
  g_serial.outCRC = 0;
  g_serial.outsize = 0;
}

void uart_init(unsigned int baudrate)
{
  // Set baud rate
  UBRR0H = (unsigned char)((baudrate >> 8) & 0xFF);
  UBRR0L = (unsigned char)baudrate & 0xFF;
  // Enable receiver and transmitter + interrupts
  UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
  // Set frame format: asynchronous, 8data, no parity, 1 stop bit
  UCSR0C = (3 << UCSZ00) | (0 << USBS0);
}

// Only used for debug purposes
void uart_puts(char *str)
{
  if (g_serial.RXstate == SER_STATE_IDLE)
    {
      strncpy(g_serial.outbuffer, str, TRSTRINGSZ);
      g_serial.TXstate = SER_STATE_SEND_DEBUG;
      g_serial.outsize = strlen(g_serial.outbuffer);
      g_serial.outindex = 0;
      // Start the transmission
      send_first_byte();
    }
}

void uart_puts_P(const char *str)
{
  if (g_serial.RXstate == SER_STATE_IDLE)
    {
      strncpy_P(g_serial.outbuffer, str, TRSTRINGSZ);
      g_serial.TXstate = SER_STATE_SEND_DEBUG;
      g_serial.outsize = strlen(g_serial.outbuffer);
      g_serial.outindex = 0;
      // Start the transmission
      send_first_byte();
    }
}

// RX interrupt
ISR(USART_RX_vect, ISR_BLOCK)
{
  char received;

  received = UDR0;
  serial_RX_Ir(received);
}

// TX interrupt
ISR(USART_TX_vect, ISR_BLOCK)
{
  // The last byte was sent, send another one if available
  if (g_serial.outindex < g_serial.outsize &&
      g_serial.outindex < TRSTRINGSZ)
    {
      // Put data into buffer, sends the data
      g_serial.outCRC = g_serial.outCRC ^ g_serial.outbuffer[g_serial.outindex];
      UDR0 = g_serial.outbuffer[g_serial.outindex];
      g_serial.outindex++;
    }
  else
    {
      g_serial.TXstate = change_TX_state(g_serial.TXstate);
      if (g_serial.TXstate != SER_STATE_IDLE && g_serial.outsize != 0)
	{
	  g_serial.outCRC = g_serial.outCRC ^ g_serial.outbuffer[g_serial.outindex];
	  UDR0 = g_serial.outbuffer[g_serial.outindex];
	  g_serial.outindex++;
	}
    }
}

void send_first_byte()
{
  char byte;

  if (g_serial.outindex < g_serial.outsize)
    {
      g_serial.outCRC = byte = g_serial.outbuffer[g_serial.outindex++];
      // Wait for the buffer to be ready
      while ((UCSR0A & (1 << UDRE0)) == 0);
      UDR0 = byte;
    }
}

