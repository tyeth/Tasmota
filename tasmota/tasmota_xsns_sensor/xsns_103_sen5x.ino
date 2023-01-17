/*
  xsns_103_sen5x.ino - SEN5X gas and air quality sensor support for Tasmota

  Copyright (C) 2022  Tyeth Gundry

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_I2C
#ifdef USE_SEN5X
/*********************************************************************************************\
 * SEN5X - Gas (VOC - Volatile Organic Compounds / NOx - Nitrous Oxides) and Particulates (PPM)
 *
 * Source: Sensirion SEN5X Driver + Example, and Tasmota Driver 98 by Jean-Pierre Deschamps
 * Adaption for TASMOTA: Tyeth Gundry
 *
 * I2C Address: 0x59
\*********************************************************************************************/

#define XSNS_103 103
#define XI2C_76 76 // See I2CDEVICES.md

#define SEN5X_ADDRESS 0x69

#include <SensirionI2CSen5x.h>
#include <Wire.h>
SensirionI2CSen5x *sen5x;

bool sen5x_type = false;
bool sen5x_ready = false;
float sen5x_abshum;
float sen5x_massConcentrationPm1p0 = NAN;
float sen5x_massConcentrationPm2p5 = NAN;
float sen5x_massConcentrationPm4p0 = NAN;
float sen5x_massConcentrationPm10p0 = NAN;
float sen5x_ambientHumidity = NAN;
float sen5x_ambientTemperature = NAN;
float sen5x_vocIndex = NAN;
float sen5x_noxIndex = NAN;

/********************************************************************************************/

void sen5x_Init(void)
{
  int usingI2cBus = 0;
  TwoWire i2cBus = Wire;
#ifdef ESP32
  if (!I2cSetDevice(SEN5X_ADDRESS, 0))
  {
    if (!I2cSetDevice(SEN5X_ADDRESS, 1))
    {
      return;
    }               // check on bus 1
    i2cBus = Wire1; // switch to bus 1
    usingI2cBus = 1;
  }
#else
  if (!I2cSetDevice(SEN5X_ADDRESS))
  {
    return;
  }
#endif
  sen5x_ready = false;
  sen5x->begin(i2cBus);
  int error_stop = sen5x->deviceReset();
  if (error_stop != 0)
  {
    return;
  }
  // Wait 1 second for sensors to start recording + 100ms for reset command
  delay(1100);
  int error_start = sen5x->startMeasurement();
  if (error_start != 0)
  {
    return;
  }
  sen5x_ready = true;
  //    AddLog(LOG_LEVEL_DEBUG, PSTR("SEN: Serialnumber 0x%04X-0x%04X-0x%04X"), sen5x.serialnumber[0], sen5x.serialnumber[1], sen5x.serialnumber[2]);
  I2cSetActiveFound(SEN5X_ADDRESS, "SEN5X", usingI2cBus);
}

// #define POW_FUNC pow
#define POW_FUNC FastPrecisePow

float sen5x_AbsoluteHumidity(float temperature, float humidity)
{
  // taken from https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
  // precision is about 0.1°C in range -30 to 35°C
  // August-Roche-Magnus 	6.1094 exp(17.625 x T)/(T + 243.04)
  // Buck (1981) 		6.1121 exp(17.502 x T)/(T + 240.97)
  // reference https://www.eas.ualberta.ca/jdwilson/EAS372_13/Vomel_CIRES_satvpformulae.html
  float temp = NAN;
  const float mw = 18.01534f;  // molar mass of water g/mol
  const float r = 8.31447215f; // Universal gas constant J/mol/K

  if (isnan(temperature) || isnan(humidity))
  {
    return NAN;
  }

  temp = POW_FUNC(2.718281828f, (17.67f * temperature) / (temperature + 243.5f));

  // return (6.112 * temp * humidity * 2.1674) / (273.15 + temperature); 	//simplified version
  return (6.112f * temp * humidity * mw) / ((273.15f + temperature) * r); // long version
}

#define SAVE_PERIOD 30

void SEN5XUpdate(void) // Perform every second to ensure proper operation of the baseline compensation algorithm
{
  uint16_t error;
  char errorMessage[256];

  error = sen5x->readMeasuredValues(
      sen5x_massConcentrationPm1p0, sen5x_massConcentrationPm2p5, sen5x_massConcentrationPm4p0,
      sen5x_massConcentrationPm10p0, sen5x_ambientHumidity, sen5x_ambientTemperature, sen5x_vocIndex,
      sen5x_noxIndex);

  if (error)
  {
    Serial.print("Error trying to execute readMeasuredValues(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
#ifdef DEBUG_TASMOTA_SENSOR
    Serial.print("SEN5x readings:-")
    Serial.print("MassConcentrationPm1p0:");
    Serial.print(sen5x_massConcentrationPm1p0);
    Serial.print("\t");
    Serial.print("MassConcentrationPm2p5:");
    Serial.print(sen5x_massConcentrationPm2p5);
    Serial.print("\t");
    Serial.print("MassConcentrationPm4p0:");
    Serial.print(sen5x_massConcentrationPm4p0);
    Serial.print("\t");
    Serial.print("MassConcentrationPm10p0:");
    Serial.print(sen5x_massConcentrationPm10p0);
    Serial.print("\t");
    Serial.print("AmbientHumidity:");
    if (isnan(sen5x_ambientHumidity))
    {
      Serial.print("n/a");
    }
    else
    {
      Serial.print(sen5x_ambientHumidity);
    }
    Serial.print("\t");
    Serial.print("AmbientTemperature:");
    if (isnan(sen5x_ambientTemperature))
    {
      Serial.print("n/a");
    }
    else
    {
      Serial.print(sen5x_ambientTemperature);
    }
    Serial.print("\t");
    Serial.print("VocIndex:");
    if (isnan(sen5x_vocIndex))
    {
      Serial.print("n/a");
    }
    else
    {
      Serial.print(sen5x_vocIndex);
    }
    Serial.print("\t");
    Serial.print("NoxIndex:");
    if (isnan(sen5x_noxIndex))
    {
      Serial.println("n/a");
    }
    else
    {
      Serial.println(sen5x_noxIndex);
    }
#endif
  }
  if (!isnan(sen5x_ambientTemperature) && sen5x_ambientHumidity > 0)
    sen5x_abshum = sen5x_AbsoluteHumidity(sen5x_ambientTemperature, sen5x_ambientHumidity);
}

#ifdef USE_WEBSERVER
const char HTTP_SNS_SEN5X[] PROGMEM =
    "{s}SEN5X " D_JSON_RAW "{m}%d "
    "{e}" // {s} = <tr><th>, {m} = </th><td>, {e} = </td></tr>
    "{s}SEN5X " D_AIR_QUALITY "{m}%d "
    "{e}";
const char HTTP_SNS_AHUMSEN5X[] PROGMEM = "{s}SEN5X Abs Humidity{m}%s g/m3{e}";
#endif

#define D_JSON_AHUM "aHumidity"

void SEN5XShow(bool json)
{
  if (sen5x_ready)
  {
    char sen5x_abs_hum[33];
    bool ahum_available = (sen5x_ambientHumidity > 0) && !isnan(sen5x_ambientTemperature);
    if (ahum_available)
    {
      // has humidity + temperature
      dtostrfd(sen5x_abshum, 4, sen5x_abs_hum);
    }
    if (json)
    {
      ResponseAppend_P(PSTR(",\"SEN5X\":{"));
      ResponseAppend_P(PSTR("\"PM1\":%d,"), sen5x_massConcentrationPm1p0);
      ResponseAppend_P(PSTR("\"PM2.5\":%d,"), sen5x_massConcentrationPm2p5);
      ResponseAppend_P(PSTR("\"PM4\":%d,"), sen5x_massConcentrationPm4p0);
      ResponseAppend_P(PSTR("\"PM10\":%d,"), sen5x_massConcentrationPm10p0);
      if (!isnan(sen5x_noxIndex))
        ResponseAppend_P(PSTR("\"NOx\":%d,"), sen5x_noxIndex);
      if (!isnan(sen5x_vocIndex))
        ResponseAppend_P(PSTR("\"VOC\":%d,"), sen5x_vocIndex);
      ResponseAppendTHD(sen5x_ambientTemperature, sen5x_ambientHumidity);
      if (ahum_available)
        ResponseAppend_P(PSTR(",\"" D_JSON_AHUM "\":%s"), sen5x_abs_hum);
      ResponseJsonEnd();
    }

#ifdef USE_WEBSERVER

    WSContentSend_PD(HTTP_SNS_SEN5X, "1", sen5x_massConcentrationPm1p0);
    WSContentSend_PD(HTTP_SNS_SEN5X, "2.5", sen5x_massConcentrationPm2p5);
    WSContentSend_PD(HTTP_SNS_SEN5X, "4", sen5x_massConcentrationPm4p0);
    WSContentSend_PD(HTTP_SNS_SEN5X, "10", sen5x_massConcentrationPm10p0);
    if (!isnan(sen5x_noxIndex))
      WSContentSend_PD(HTTP_SNS_SEN5X, "NOx", sen5x_noxIndex);
    if (!isnan(sen5x_vocIndex))
      WSContentSend_PD(HTTP_SNS_SEN5X, "VOC", sen5x_vocIndex);
    WSContentSend_PD(HTTP_SNS_SEN5X, "Humidity", sen5x_ambientTemperature);
    WSContentSend_PD(HTTP_SNS_SEN5X, "Temperature", sen5x_ambientHumidity);
    if (ahum_available)
      WSContentSend_PD(HTTP_SNS_SEN5X, D_JSON_AHUM, sen5x_abs_hum);

#endif
  }
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns103(uint32_t function)
{
  if (!I2cEnabled(XI2C_76))
  {
    return false;
  }

  bool result = false;

  if (FUNC_INIT == function)
  {
    sen5x_Init();
  }
  else if (sen5x_type)
  {
    switch (function)
    {
    case FUNC_EVERY_SECOND:
      SEN5XUpdate();
      break;
    case FUNC_JSON_APPEND:
      SEN5XShow(1);
      break;
#ifdef USE_WEBSERVER
    case FUNC_WEB_SENSOR:
      SEN5XShow(0);
      break;
#endif // USE_WEBSERVER
    }
  }
  return result;
}

#endif // USE_SEN5X
#endif // USE_I2C
