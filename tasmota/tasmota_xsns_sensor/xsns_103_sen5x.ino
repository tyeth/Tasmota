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

struct SEN5XDATA_s {
  bool sen5x_ready;
  float abshum;
  float massConcentrationPm1p0;
  float massConcentrationPm2p5;
  float massConcentrationPm4p0;
  float massConcentrationPm10p0;
  float ambientHumidity;
  float ambientTemperature;
  float vocIndex;
  float noxIndex;
} *SEN5XDATA = nullptr;
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
  if (SEN5XDATA == nullptr) 
    SEN5XDATA = (SEN5XDATA_s *)calloc(1, sizeof(struct SEN5XDATA_s));
  SEN5XDATA->sen5x_ready = false;
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
  SEN5XDATA->sen5x_ready = true;
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
      SEN5XDATA->massConcentrationPm1p0, SEN5XDATA->massConcentrationPm2p5, SEN5XDATA->massConcentrationPm4p0,
      SEN5XDATA->massConcentrationPm10p0, SEN5XDATA->ambientHumidity, SEN5XDATA->ambientTemperature, SEN5XDATA->vocIndex,
      SEN5XDATA->noxIndex);

  if (error)
  {
    Serial.print("Error trying to execute readMeasuredValues(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
#ifdef DEBUG_TASMOTA_SENSOR
    Serial.print("SEN5x readings:-");
    Serial.print("MassConcentrationPm1p0:");
    Serial.print(SEN5XDATA->massConcentrationPm1p0);
    Serial.print("\t");
    Serial.print("MassConcentrationPm2p5:");
    Serial.print(SEN5XDATA->massConcentrationPm2p5);
    Serial.print("\t");
    Serial.print("MassConcentrationPm4p0:");
    Serial.print(SEN5XDATA->massConcentrationPm4p0);
    Serial.print("\t");
    Serial.print("MassConcentrationPm10p0:");
    Serial.print(SEN5XDATA->massConcentrationPm10p0);
    Serial.print("\t");
    Serial.print("AmbientHumidity:");
    if (isnan(SEN5XDATA->ambientHumidity))
    {
      Serial.print("n/a");
    }
    else
    {
      Serial.print(SEN5XDATA->ambientHumidity);
    }
    Serial.print("\t");
    Serial.print("AmbientTemperature:");
    if (isnan(SEN5XDATA->ambientTemperature))
    {
      Serial.print("n/a");
    }
    else
    {
      Serial.print(SEN5XDATA->ambientTemperature);
    }
    Serial.print("\t");
    Serial.print("VocIndex:");
    if (isnan(SEN5XDATA->vocIndex))
    {
      Serial.print("n/a");
    }
    else
    {
      Serial.print(SEN5XDATA->vocIndex);
    }
    Serial.print("\t");
    
    Serial.print("NoxIndex:");
    if (isnan(SEN5XDATA->noxIndex))
    {
      Serial.println("n/a");
    }
    else
    {
      Serial.println(SEN5XDATA->noxIndex);
    }
#endif
  }
  if (!isnan(SEN5XDATA->ambientTemperature) && SEN5XDATA->ambientHumidity > 0)
    SEN5XDATA->abshum = sen5x_AbsoluteHumidity(SEN5XDATA->ambientTemperature, SEN5XDATA->ambientHumidity);
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
  if (SEN5XDATA->sen5x_ready)
  {
    char sen5x_abs_hum[33];
    bool ahum_available = (SEN5XDATA->ambientHumidity > 0) && !isnan(SEN5XDATA->ambientTemperature);
    if (ahum_available)
    {
      // has humidity + temperature
      dtostrfd(SEN5XDATA->abshum, 4, sen5x_abs_hum);
    }
    if (json)
    {
      ResponseAppend_P(PSTR(",\"SEN5X\":{"));
      ResponseAppend_P(PSTR("\"PM1\":%d,"), SEN5XDATA->massConcentrationPm1p0);
      ResponseAppend_P(PSTR("\"PM2.5\":%d,"), SEN5XDATA->massConcentrationPm2p5);
      ResponseAppend_P(PSTR("\"PM4\":%d,"), SEN5XDATA->massConcentrationPm4p0);
      ResponseAppend_P(PSTR("\"PM10\":%d,"), SEN5XDATA->massConcentrationPm10p0);
      if (!isnan(SEN5XDATA->noxIndex))
        ResponseAppend_P(PSTR("\"NOx\":%d,"), SEN5XDATA->noxIndex);
      if (!isnan(SEN5XDATA->vocIndex))
        ResponseAppend_P(PSTR("\"VOC\":%d,"), SEN5XDATA->vocIndex);
      ResponseAppendTHD(SEN5XDATA->ambientTemperature, SEN5XDATA->ambientHumidity);
      if (ahum_available)
        ResponseAppend_P(PSTR(",\"" D_JSON_AHUM "\":%s"), sen5x_abs_hum);
      ResponseJsonEnd();
    }

#ifdef USE_WEBSERVER

    WSContentSend_PD(HTTP_SNS_SEN5X, "1", SEN5XDATA->massConcentrationPm1p0);
    WSContentSend_PD(HTTP_SNS_SEN5X, "2.5", SEN5XDATA->massConcentrationPm2p5);
    WSContentSend_PD(HTTP_SNS_SEN5X, "4", SEN5XDATA->massConcentrationPm4p0);
    WSContentSend_PD(HTTP_SNS_SEN5X, "10", SEN5XDATA->massConcentrationPm10p0);
    if (!isnan(SEN5XDATA->noxIndex))
      WSContentSend_PD(HTTP_SNS_SEN5X, "NOx", SEN5XDATA->noxIndex);
    if (!isnan(SEN5XDATA->vocIndex))
      WSContentSend_PD(HTTP_SNS_SEN5X, "VOC", SEN5XDATA->vocIndex);
    WSContentSend_PD(HTTP_SNS_SEN5X, "Humidity", SEN5XDATA->ambientTemperature);
    WSContentSend_PD(HTTP_SNS_SEN5X, "Temperature", SEN5XDATA->ambientHumidity);
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
  else if (SEN5XDATA != nullptr)
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
