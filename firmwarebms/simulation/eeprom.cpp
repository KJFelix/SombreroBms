#include <stdlib.h>
#include <string.h>

#include "../env.h"
#include "../main.h"
#include "../AD7280A.h"
#include "../eeprom.h"

#include "fake_battery.h"

extern t_pack_variable_data g_appdata;
extern t_eeprom_data        g_edat;
extern t_eeprom_battery     g_bat[MAXBATTERY];

//---------------------------------------
// Read all, Write all
//---------------------------------------

void read_cfg_from_EEPROM(void)
{
  unsigned int i;

  // Default values to make the buzer scream
  g_edat.install_date_year  = 2016;
  g_edat.install_date_month = 1;
  g_edat.install_date_day   = 1;
  g_edat.bat_maxv = BAT_MAXV  - 300;   // mili volts
  g_edat.bat_minv = BAT_MINMV + 200;
  g_edat.bat_tmax =  50;               // temperature
  g_edat.bat_tmin = -20;
  g_edat.bat_elements = CFGBATNUMBER;
  g_edat.full_charge  = DEFAULT_CHARGE; // AH
  g_edat.max_current  = DEFAULT_MAX_CURRENT_A;
  g_edat.serial_number[0] = '1';
  for (i = 1; i < sizeof(g_edat.serial_number) / sizeof(char); i++)
    g_edat.serial_number[i] = 0;       //  8 bytes             plus 0 end of string
  g_edat.client[0] = 0;                // 32 bytes Client name plus 0 end of string
  // These are often programmed
  g_edat.charge_cycles = 0;
  g_edat.charge_time_minutes = 0;      // Total charging time
  g_edat.min_temperature = 25;         // Extreme temperatures
  g_edat.max_temperature = 25;
}

void read_bat_values_from_EEPROM(t_eeprom_battery *pbats, int elements)
{
  int i;

  for (i = 0; i < elements; i++)
    {
      pbats[i].lowestV = g_edat.bat_maxv;
      pbats[i].lowVevents = 0;
      pbats[i].cap = 40000;
    }
}

void write_cfg_to_EEPROM(void)
{
}

void write_bat_values_to_EEPROM(t_eeprom_battery *pbats, int elements)
{
}

// Adds an error code to a circular memory at the end of the EEPROM.
// The first byte is the index of the value that will be written or rewritten.
void add_error_log_EEPROM(char code)
{
}

