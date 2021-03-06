#include <stdio.h>
#include <string.h>

#include "env.h"
#include "main.h"
#include "eeprom.h"
#include "AD7280A.h"
#include "balancing.h"
#include "uart.h"
#include "serial.h"

// Program space string macro replacement for the simulation
#ifndef _SIMULATION
#include <avr/pgmspace.h>
#else
#ifndef PSTR
#define PSTR(A) ((char*)A)
//#define strcmp_P  strcmp
#define snprintf_P snprintf
#define strncmp_P  strncmp
#endif  //PSTR
#endif  //_SIMULATION

extern t_pack_variable_data g_appdata;
extern t_eeprom_data        g_edat;
extern t_eeprom_battery     g_bat[MAXBATTERY];
extern t_ad7280_state       g_ad7280;

extern t_serialport g_serial;

// It starts at the '-' of "set_param -valuename value" and checks if valuename is
// refvalue. Returns 0 if not. And if equal it fills the pointer to the argument in
// *pstr and returns 1.
char check_valuename(const char *refvalue, char *command_buffer, char index, char **pstr)
{
  char reflen;
  char remainlen;
  int i;

  command_buffer = command_buffer + index;
  reflen = strlen(refvalue);
  remainlen = strlen(command_buffer);
  // Eliminate cases where it cannot fit
  if (remainlen <= reflen + 2)
    return 0;
  if (strncmp_P(command_buffer, refvalue, reflen) == 0)
    {
      // Jump over spaces
      i = reflen;
      while ((command_buffer[i] == ' ' || command_buffer[i] == '"') && i < remainlen)
	i++;
      if (i >= remainlen)
	return 0;
      *pstr = &command_buffer[i];
      return 1;
    }
  *pstr = NULL;
  return 0;
}

void execute_config_command(char *command_buffer, char size)
{
  int  i;
  char found;
  char *str;

  if (strncmp_P(command_buffer, PSTR("set_param"), 9))
    return;
  // Find the command type
  i = 0;
  found = 0;
  while ((i < size - 1) && !found)
    {
      if ((command_buffer[i] == '-'))
	{
	  found = 1;
	}
      i++;
    }
  if (found)
    {
      if (check_valuename(PSTR("date"), command_buffer, i, &str))
	{
	  set_install_date_EEPROM(str);
	}
      if (check_valuename(PSTR("batVmin"), command_buffer, i, &str))
	{
	  set_min_Vbat_EEPROM(str);
	}
      if (check_valuename(PSTR("batVmax"), command_buffer, i, &str))
	{
	  set_max_Vbat_EEPROM(str);
	}
      if (check_valuename(PSTR("tmin"), command_buffer, i, &str))
	{
	  set_min_temperature_EEPROM(str);
	}
      if (check_valuename(PSTR("tmax"), command_buffer, i, &str))
	{
	  set_max_temperature_EEPROM(str);
	}
      if (check_valuename(PSTR("fullcharge"), command_buffer, i, &str))
	{
	  set_full_charge_value_mAH_EEPROM(str);	  
	}
      if (check_valuename(PSTR("fullVbat"), command_buffer, i, &str))
	{
	  set_full_charge_value_mV_EEPROM(str);	  
	}
      if (check_valuename(PSTR("serial"), command_buffer, i, &str))
	{
	  set_serial_number_EEPROM(str);
	}
      if (check_valuename(PSTR("client"), command_buffer, i, &str))
	{
	  set_client_name_EEPROM(str);
	}
    }
}

#define SPAREBYTES

void process_serial_command(void)
{
  if (g_serial.TXstate != SER_STATE_IDLE)
    return ;
  g_serial.outbuffer[0] = 0;

  // Send the first line of the report message, the interrupt at the end of the byte
  // transmission will send the other lines.
  //
  // Be carefull and do not use PSTR here or strcmp will fail!
  if (strncmp_P(g_serial.inbuffer, PSTR("get_report\n"), 11) == 0)
    {
      snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("bmsReportBegin\n"));
      g_serial.TXstate = SER_STATE_SEND_REPORT_VB;
    }
  else
    {
      if (strncmp_P(g_serial.inbuffer, PSTR("ping\n"), 5) == 0)
	{
#ifdef SPAREBYTES
	  snprintf(g_serial.outbuffer, TRSTRINGSZ, "SomBMS\n");
	  g_serial.TXstate = SER_STATE_SEND_PING3;
#else
	  snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("Sombrero BMS C2015-2016 Vreemdelabs.com\n"));
	  g_serial.TXstate = SER_STATE_SEND_PING1;
#endif
	}
      else
	if (strncmp_P(g_serial.inbuffer, PSTR("get_params\n"), 11) == 0)
	  {
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("setup date: %d/%d/%d\n"),
		     g_edat.install_date_day,
		     g_edat.install_date_month,
		     g_edat.install_date_year);
	    g_serial.TXstate = SER_STATE_SEND_DATE;
	  }
    }
  // This is a configuration command
  if (strncmp_P(g_serial.inbuffer, PSTR("set_param"), 9) == 0)
    {
      execute_config_command(g_serial.inbuffer, g_serial.inindex);
    }
  //
  g_serial.outsize = strlen(g_serial.outbuffer);
  g_serial.outindex = 0;
  // Start the transmission
  send_first_byte();
}

int get_undervoltage_event_count(int bat)
{
  if (bat >= g_edat.bat_elements)
    return 0;
  return g_bat[bat].lowVevents;
}

int get_total_undervoltage_event_count(void)
{
  int  sum;
  char i;

  sum = 0;
  for (i = 0; i < g_edat.bat_elements; i++)
    {
      sum += get_undervoltage_event_count(i);
    }
  return sum;
}

unsigned long get_charge_speed(int bat)
{
  if (bat >= g_edat.bat_elements)
    return 0;
  return g_bat[bat].cap;  
}

void print_decimal(const char *field_str, int value, const char *unit_str)
{
  int v, vd;

  v  = (value / 1000);
  vd = (value % 1000L);
  if (vd < 10)
    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("%s:%d.00%d%s\n"), field_str, v, vd, unit_str);
  else
    if (vd < 100)
      snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("%s:%d.0%2d%s\n"), field_str, v, vd, unit_str);
    else
      snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("%s:%d.%d%s\n"), field_str, v, vd, unit_str);
}

// Called in the transmit interrupt
char change_TX_state(char TXstate)
{
  char nextState;

  g_serial.outindex = 0;
  g_serial.outbuffer[0] = 0;
  switch (TXstate)
    {
    case SER_STATE_IDLE:
      g_serial.outsize = 0;
      g_serial.outCRC = 0;
      nextState = SER_STATE_IDLE;
      break;
    case SER_STATE_SEND_DEBUG:
      nextState = SER_STATE_IDLE;
      break;
      //-----------------------------------------------------------------------------
      // Response to "ping" command
    case SER_STATE_SEND_PING1:
      {
#ifdef SPAREBYTES
	snprintf(g_serial.outbuffer, TRSTRINGSZ, PSTR("Fi\n"));
	nextState = SER_STATE_SEND_PING3;
      }
      break;
#else
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("Firmware Version: %d.%d\n"), FIRMWARE_VERSION, FIRMWARE_SUBVERSION);
	nextState = SER_STATE_SEND_PING2;
      }
      break;
    case SER_STATE_SEND_PING2:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("Elements: %d\n"), g_edat.bat_elements);
	nextState = SER_STATE_SEND_PING3;
      }
      break;
#endif //SPAREBYTES
    case SER_STATE_SEND_PING3:
      {
	g_serial.outbuffer[0] = g_serial.outCRC;
	g_serial.outbuffer[1] = '\0';
	g_serial.outsize = 2;
	nextState = SER_STATE_ENDOF_MSG;
      }
      break;
      //-----------------------------------------------------------------------------
      // Response to "get_params" command
    case SER_STATE_SEND_DATE:
      {
	int charge_percent;

	charge_percent = 100L * g_appdata.state_of_charge / g_edat.full_charge;
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("charge: %d%c\n"), charge_percent, '%');
	nextState = SER_STATE_SEND_CHARGE;
      }
      break;
    case SER_STATE_SEND_CHARGE:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("total capacity: %dAH\n"), (int)(g_edat.full_charge / 1000L));
	nextState = SER_STATE_SEND_TOTALCAP;
      }
      break;
    case SER_STATE_SEND_TOTALCAP:
      {
	print_decimal("Vbat", (int)(g_appdata.total_vbat), "V");
	nextState = SER_STATE_SEND_VBAT;
      }
      break;
    case SER_STATE_SEND_VBAT:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("Elements: %d\n"), g_edat.bat_elements);
	nextState = SER_STATE_SEND_ELEMENTS;
      }
      break;
    case SER_STATE_SEND_ELEMENTS:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("charge cycles: %d\n"), g_edat.charge_cycles);
	nextState = SER_STATE_SEND_CHARGECYCLES;
      }
      break;
    case SER_STATE_SEND_CHARGECYCLES:
      {
	int hours;

	hours = g_edat.charge_time_minutes / 60;
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("total charging time: %dh\n"), hours);
	nextState = SER_STATE_SEND_TOTALCHRGTIME;
      }
      break;
    case SER_STATE_SEND_TOTALCHRGTIME:
      {
	print_decimal("Vmax", (int)(g_edat.bat_maxv), "V");
	nextState = SER_STATE_SEND_VMAX;
      }
      break;
    case SER_STATE_SEND_VMAX:
      {
	print_decimal("Vmin", (int)(g_edat.bat_minv), "V");
	nextState = SER_STATE_SEND_VMIN;
      }
      break;
    case SER_STATE_SEND_VMIN:
      {
	print_decimal("MaxVbat", (int)(g_edat.full_chargeV), "V");
	nextState = SER_STATE_SEND_MAX_VBAT;
      }
      break;
    case SER_STATE_SEND_MAX_VBAT:
      {
	unsigned long undevorlatege_evt_cnt;

	undevorlatege_evt_cnt = get_total_undervoltage_event_count();
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("undervoltage events: %lu\n"), undevorlatege_evt_cnt);
	nextState = SER_STATE_SEND_UNDERVOLTAGEEVTS;
      }
      break;
    case SER_STATE_SEND_UNDERVOLTAGEEVTS:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("maxdischarge: %dA\n"), g_appdata.max_discharge);
	nextState = SER_STATE_SEND_MAXDISCHARGE;
      }
      break;
    case SER_STATE_SEND_MAXDISCHARGE:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("serial number: %s\n"), g_edat.serial_number);
	nextState = SER_STATE_SEND_SERIAL;
      }
      break;
    case SER_STATE_SEND_SERIAL:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("client: \"%s\"\n"), g_edat.client);
	nextState = SER_STATE_SEND_CLIENT;
      }
      break;
    case SER_STATE_SEND_CLIENT:
      {
	int years;
	int days;
	int hours;

	years = g_appdata.uptime_days / 365;
	days  = g_appdata.uptime_days % 365;
	hours = g_appdata.uptime_minutes / 60;
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("uptime: %dyears %ddays %dh\n"), years, days, hours);
	nextState = SER_STATE_SEND_UPTIME;
      }
      break;
    case SER_STATE_SEND_UPTIME:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("max recorded temp:  %d�C\n"), g_edat.max_temperature);
	nextState = SER_STATE_SEND_MAXT_RECORD;
      }
      break;
    case SER_STATE_SEND_MAXT_RECORD:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("max temperature:  %d�C\n"), g_edat.bat_tmax);
	nextState = SER_STATE_SEND_MAXT;
      }
      break;
    case SER_STATE_SEND_MAXT:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("min recorded temp:  %d�C\n"), g_edat.min_temperature);
	nextState = SER_STATE_SEND_MINT_RECORD;
      }
      break;
    case SER_STATE_SEND_MINT_RECORD:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("min temperature:  %d�C\n"), g_edat.bat_tmin);
	nextState = SER_STATE_SEND_MINT;

      }
      break;
    case SER_STATE_SEND_MINT:
      {
	int highter_temp;

	highter_temp = get_highter_temperature(g_appdata.temperature, CFGAD728AMODULES);
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("temperature:  %d�C\n"), highter_temp);
	nextState = SER_STATE_SEND_HITEMP;
      }
      break;
    case SER_STATE_SEND_HITEMP:
      {
	g_serial.outbuffer[0] = g_serial.outCRC;
	g_serial.outbuffer[1] = '\0';
	g_serial.outsize = 2;
	nextState = SER_STATE_ENDOF_MSG; // Finished
      }
      break;

      //-----------------------------------------------------------------------------
      // Response to "get_report" command
    case SER_STATE_SEND_REPORT_VB:
      {
	print_decimal("Vb", (int)(g_appdata.total_vbat), "");
	nextState = SER_STATE_SEND_REPORT_CHRG;
      }
      break;
    case SER_STATE_SEND_REPORT_CHRG:
      {
	int charge_percent;

	charge_percent = 100L * g_appdata.state_of_charge / g_edat.full_charge;
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("chrg: %d\n"), charge_percent);
	nextState = SER_STATE_SEND_REPORT_CHRGMA;
      }
      break;
    case SER_STATE_SEND_REPORT_CHRGMA:
      {
	print_decimal("chAH", (int)(g_appdata.state_of_charge), "");
	nextState = SER_STATE_SEND_REPORT_IMA;
      }
      break;
    case SER_STATE_SEND_REPORT_IMA:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("ImAH: %d\n"), (int)(g_appdata.c_discharge));
	nextState = SER_STATE_SEND_REPORT_STATE;
      }
      break;
    case SER_STATE_SEND_REPORT_STATE:
      {
	switch (g_appdata.app_state)
	  {
	  case STATE_START:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: init\n"));
	    break;
	  case STATE_CHARGEON:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: chargeon\n"));
	    break;
	  case STATE_CHARGING:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: charging\n"));
	    break;
	  case STATE_IDLE:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: idle\n"));
	    break;
	  case STATE_RUN:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: run\n"));
	    break;
	  case STATE_RELAX:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: relax\n"));
	    break;
	  case STATE_SECURITY:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: security\n"));
	    break;
	  case STATE_CURRENT_SECURITY:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: current flow security\n"));
	    break;
	  case STATE_CRITICAL_FAILURE:
	  default:
	    snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("State: failure\n"));
	    break;
	  }
	nextState = SER_STATE_SEND_REPORT_ELTS;
      }
      break;
    case SER_STATE_SEND_REPORT_ELTS:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("Elts: %d\n"), g_edat.bat_elements);
	nextState = SER_STATE_SEND_REPORT_TEMP;
      }
      break;
    case SER_STATE_SEND_REPORT_TEMP:
      {
	int highter_temp;
	
	highter_temp = get_highter_temperature(g_appdata.temperature, CFGAD728AMODULES);
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("temperature:  %d\n"), highter_temp);
	nextState = SER_STATE_SEND_REPORT_BATBEGIN;
      }
      break;
    case SER_STATE_SEND_REPORT_BATBEGIN:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("batbegin: %d\n"), g_serial.batcounter);
	nextState = SER_STATE_SEND_REPORT_BATVB;
      }
      break;
    case SER_STATE_SEND_REPORT_BATVB:
      {
	print_decimal("Vb", g_appdata.vbat[g_serial.batcounter], "");
	nextState = SER_STATE_SEND_REPORT_BATVLOW;
      }
      break;
    case SER_STATE_SEND_REPORT_BATVLOW:
      {
	print_decimal("VLowest", g_bat[g_serial.batcounter].lowestV, "");
	nextState = SER_STATE_SEND_REPORT_BATEVT;
      }
      break;      
    case SER_STATE_SEND_REPORT_BATEVT:
      {
	int underflow_events;

	underflow_events = get_undervoltage_event_count(g_serial.batcounter);
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("evt: %d\n"), underflow_events);
	nextState = SER_STATE_SEND_REPORT_BATAVGT;
      }
      break;
    case SER_STATE_SEND_REPORT_BATAVGT:
      {
	int average_charge_time; // Charge speed or whatever

	average_charge_time = get_charge_speed(g_serial.batcounter);
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("avgchgt: %d\n"), average_charge_time);
	nextState = SER_STATE_SEND_REPORT_BATBALANC;
      }
      break;
    case SER_STATE_SEND_REPORT_BATBALANC:
      {
	char balanced;

	balanced = ad7280_get_balance(&g_ad7280, g_serial.batcounter);
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("balan: %d\n"), balanced);
	g_serial.batcounter++;
	if (g_serial.batcounter >= g_edat.bat_elements)
	  {
	    g_serial.batcounter = 0;
	    nextState = SER_STATE_SEND_REPORT_END;    // Finished
	  }
	else
	  nextState = SER_STATE_SEND_REPORT_BATBEGIN; // Next battery element
      }
      break;
    case SER_STATE_SEND_REPORT_END:
      {
	snprintf_P(g_serial.outbuffer, TRSTRINGSZ, PSTR("bmsReportEnd\n"));
	nextState = SER_STATE_SEND_REPORT_FINISHED;
      }
      break;
    case SER_STATE_SEND_REPORT_FINISHED:
      {
	g_serial.outbuffer[0] = g_serial.outCRC;
	g_serial.outbuffer[1] = '\0';
	g_serial.outsize = 2;
	nextState = SER_STATE_ENDOF_MSG;
      }
      break;
    case SER_STATE_ENDOF_MSG:
    default:
      g_serial.outsize = 0;
      nextState = SER_STATE_IDLE;
    }
  g_serial.outsize = (nextState == SER_STATE_ENDOF_MSG)? g_serial.outsize : strlen(g_serial.outbuffer);
  return nextState;
}

void serial_RX_Ir(char received)
{
  switch (g_serial.RXstate)
    {
    case SER_STATE_IDLE:
    case SER_STATE_RECEIVE:
      {
	if (g_serial.inindex >= RCVSTRINGSZ)
	  {
	    g_serial.inCRC = 0;
	    g_serial.inindex = 0;
	    g_serial.RXstate = SER_STATE_IDLE;
	  }
	g_serial.inbuffer[g_serial.inindex++] = received;
	g_serial.inCRC = g_serial.inCRC ^ received;
	if (received == '\n')
	  {
	    g_serial.inbuffer[g_serial.inindex] = 0; // Add an end to form a string
	    g_serial.RXstate = SER_STATE_CRC;
	  }
      }
      break;
    case SER_STATE_CRC:
      {
	if (g_serial.inCRC == received)
	  {
	    g_serial.RXstate = SER_STATE_WAIT_ENDOF_MESSAGE;
	  }
	else
	  {
	    g_serial.inCRC = 0;
	    g_serial.inindex = 0;
	    g_serial.RXstate = SER_STATE_IDLE;
	  }
      }
      break;
    case SER_STATE_WAIT_ENDOF_MESSAGE:
      {
	if (received == 0)
	  process_serial_command();
      }
    default:
      {
	g_serial.inCRC = 0;
	g_serial.inindex = 0;
	g_serial.RXstate = SER_STATE_IDLE;
      }
      break;
    }
}

