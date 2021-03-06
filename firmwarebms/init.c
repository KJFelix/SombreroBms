/******************************************************************************
 Title:    Sombrero BMS
 Author:   Patrick Areny arenyp@yahoo.fr
 Date:     2016
 Software: AVR-GCC 3.3 
 Hardware: ATMEGA168PA
*******************************************************************************/

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#define __DELAY_BACKWARD_COMPATIBLE__ 
#include <util/delay.h>

#include "env.h"
#include "main.h"
#include "uart.h"
#include "serial.h"
#include "adc.h"
#include "spi.h"
#include "eeprom.h"
#include "AD7280A.h"
#include "inout.h"
#include "init.h"

extern t_pack_variable_data g_appdata;
extern t_eeprom_data        g_edat;
extern t_eeprom_battery     g_bat[MAXBATTERY];
extern t_ad7280_state       g_ad7280;
extern t_serialport         g_serial;

/*
void INT0_init(void)
{
  // INT0 low -> interruption
  //CBI(MCUCR, ISC01);
  //CBI(MCUCR, ISC00);
  //SBI(GIFR, INTF0);
  //SBI(GICR, INT0);
}

ISR(INT0_vect)
{
  // Interrupt disabled
  //CBI(GIFR, INTF0);
  //CBI(GICR, INT0);
  // Do nothing, it just wakes up from idle mode
}

// Enable the timer interrupt and the INT0/PD2 input interrupt
void configure_Interrupts(void)
{
  //set_sleep_mode(SLEEP_MODE_IDLE);  
}

void configure_Timer(void)
{
}

void enter_idle_mode(void)
{
  // reset the timer
  
  // cpu in idle mode, (IO clock and timers are activated)
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
}
*/

// Catch all ISR
ISR(BADISR_vect)
{
  for (;;) UDR0='!';
}

// Called at start, sets the IO port direction, defaults the variables, 
// Initialises the SPI devices and configures the interrupts
void init()
{
  // Reset condition flag
  //char byte;
  //byte = MCUSR;

  // IO ports
  // Port B:
  DDRB  = 0x00 |  (1 << STOP_CHARGER) | (1 << RELON) | (1 << MOSI) | (1 << SCLK);
  PORTB = 0x00 |  (0 << STOP_CHARGER) | (0 << RELON) | (0 << MOSI) | (0 << SCLK); // | (1 << MISO);
  // Port C:
  DDRC  = 0x00 | (1 << GAUGE) | (1 << CTS) | (1 << CNVSTART);
  PORTC = 0x00 | (1 << GAUGE) | (0 << CTS) | (1 << CNVSTART);
  // Port D:
  DDRD  = 0x00 | (1 << TXD) | (1 << CSBAT) | (1 << PDOWN) | (1 << LED_ERROR) | (1 << LED_BALANCING) | (1 << BUZZER);
  PORTD = 0x00 | (1 << TXD) | (1 << CSBAT) | (1 << PDOWN) | (1 << LED_ERROR) | (1 << LED_BALANCING) | (0 << BUZZER);

  // Wait 50ms
  _delay_ms(50);

  // Default values
  memset(&g_appdata, 0, sizeof(g_appdata));
  // Start point
  g_appdata.app_state = STATE_START;

  // Init UART
  init_serial_vars();
  uart_init(UART_BAUD_SELECT(BAUDRATE, F_CPU));

  // Init SPI
  init_spi_master();

  sei(); // For the uart

  uart_puts("Init.\n");
  _delay_ms(1000);

  // Init the SPI device AD7820A
  if (init_AD7820A(&g_ad7280))
    {
      setled_error(1);
      while (1)
	{
	  _delay_ms(1000);
	  uart_puts_P(PSTR("AD7820A init failed\n"));
	}
    }

  // Initialises the ADC channel 7
  init_adc();

  // If something was stored in the EEPROM, read it
  read_cfg_from_EEPROM();
  read_bat_values_from_EEPROM(g_bat, g_edat.bat_elements);

  setled_balancing(0);

  // Enable interrupts
  sei();
}

