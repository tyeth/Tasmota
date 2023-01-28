/*
  user_config_override.h - user configuration overrides my_user_config.h for Tasmota

  Copyright (C) 2021  Theo Arends

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

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

/*****************************************************************************************************\
 * USAGE:
 *   To modify the stock configuration without changing the my_user_config.h file:
 *   (1) copy this file to "user_config_override.h" (It will be ignored by Git)
 *   (2) define your own settings below
 *
 ******************************************************************************************************
 * ATTENTION:
 *   - Changes to SECTION1 PARAMETER defines will only override flash settings if you change define CFG_HOLDER.
 *   - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *   - You still need to update my_user_config.h for major define USE_MQTT_TLS.
 *   - All parameters can be persistent changed online using commands via MQTT, WebConsole or Serial.
\*****************************************************************************************************/

/*
Examples :

// -- Master parameter control --------------------
#undef  CFG_HOLDER
#define CFG_HOLDER        4617                   // [Reset 1] Change this value to load SECTION1 configuration parameters to flash

// -- Setup your own Wifi settings  ---------------
#undef  STA_SSID1
#define STA_SSID1         "free4all_2G"             // [Ssid1] Wifi SSID

#undef  STA_PASS1
#define STA_PASS1         "YourWifiPassword"     // [Password1] Wifi password

// -- Setup your own MQTT settings  ---------------
#undef  MQTT_HOST
#define MQTT_HOST         "your-mqtt-server.com" // [MqttHost]

#undef  MQTT_PORT
#define MQTT_PORT         1883                   // [MqttPort] MQTT port (10123 on CloudMQTT)

#undef  MQTT_USER
#define MQTT_USER         "YourMqttUser"         // [MqttUser] Optional user

#undef  MQTT_PASS
#define MQTT_PASS         "YourMqttPass"         // [MqttPassword] Optional password

// You might even pass some parameters from the command line ----------------------------
// Ie:  export PLATFORMIO_BUILD_FLAGS='-DUSE_CONFIG_OVERRIDE -DMY_IP="192.168.1.99" -DMY_GW="192.168.1.1" -DMY_DNS="192.168.1.1"'

#ifdef MY_IP
#undef  WIFI_IP_ADDRESS
#define WIFI_IP_ADDRESS     MY_IP                // Set to 0.0.0.0 for using DHCP or enter a static IP address
#endif

#ifdef MY_GW
#undef  WIFI_GATEWAY
#define WIFI_GATEWAY        MY_GW                // if not using DHCP set Gateway IP address
#endif

#ifdef MY_DNS
#undef  WIFI_DNS
#define WIFI_DNS            MY_DNS               // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

#ifdef MY_DNS2
#undef  WIFI_DNS2
#define WIFI_DNS2           MY_DNS2              // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

// !!! Remember that your changes GOES AT THE BOTTOM OF THIS FILE right before the last #endif !!!
*/

#undef  STA_SSID1
#define STA_SSID1         "free4all_2G"             // [Ssid1] Wifi SSID

#undef  STA_PASS1
#define STA_PASS1         "password"     // [Password1] Wifi password

#ifdef SENSOR_MAX_MISS
#undef SENSOR_MAX_MISS
#endif
#define SENSOR_MAX_MISS 10              // Retries before sensor offline

#define USE_DISPLAY
#define USE_DISPLAYS
#define USE_DISPLAY_ST7789 

#define USE_UNIVERSAL_DISPLAY

//   //  #define USE_DT_VARS                          // Display variables that are exposed in JSON MQTT strings e.g. in TelePeriod messages.
//   //  #define MAX_DT_VARS     16                   // Defaults to 7
//   //  #define USE_GRAPH                            // Enable line charts with displays
//   //  #define NUM_GRAPHS     4  

#ifndef USE_AUTOCONF
#define USE_AUTOCONF                             // Enable Esp32 autoconf feature, requires USE_BERRY and USE_WEBCLIENT_HTTPS (12KB Flash)
#endif

#ifndef USE_I2C
#define USE_I2C
#endif

// //  #define USE_SCD30                              // [I2cDriver29] Enable Sensiron SCd30 CO2 sensor (I2C address 0x61) (+3k3 code)
#ifndef USE_SCD40
  #define USE_SCD40                              // [I2cDriver62] Enable Sensiron SCd40/Scd41 CO2 sensor (I2C address 0x62) (+3k5 code)
#endif
#ifndef USE_SPS30
  #define USE_SPS30
#endif

#ifndef USE_VL53L0X
  #define USE_VL53L0X                            // [I2cDriver31] Enable VL53L0x time of flight sensor (I2C address 0x29) (+4k code)
#endif

#ifdef VL53L0X_XSHUT_ADDRESS
#undef VL53L0X_XSHUT_ADDRESS
#endif
#define VL53L0X_XSHUT_ADDRESS 0x78           //   VL53L0X base address when used with XSHUT control

#ifdef USE_VL53L1X
 #undef USE_VL53L1X                            // [I2cDriver54] Enable VL53L1X time of flight sensor (I2C address 0x29) using Pololu VL53L1X library (+2k9 code)
#endif
// //    #define VL53L1X_XSHUT_ADDRESS 0x78           //   VL53L1X base address when used with XSHUT control
//   // #define VL53L1X_DISTANCE_MODE Long           //   VL53L1X distance mode : Long | Medium | Short


#ifdef USE_AHT1x
#undef USE_AHT1x
#endif
#ifndef USE_AHT2x
#define USE_AHT2x  
#endif

#ifndef USE_VEML7700
#define USE_VEML7700
#endif 
// //  #define USE_SGP30                              // [I2cDriver18] Enable SGP30 sensor (I2C address 0x58) (+1k1 code)
// //  #define USE_SGP40
 
#ifndef USE_SEN5X
 #define USE_SEN5X                              // [I2cDriver76] Enable SEN5X sensor (I2C address 0x69) (+3k code)
#endif

#ifdef USE_SHT
#undef USE_SHT
#endif
// #define USE_SHT                                // [I2cDriver8] Enable SHT1X sensor (+1k4 code)
// #define USE_HTU                                // [I2cDriver9] Enable HTU21/SI7013/SI7020/SI7021 sensor (I2C address 0x40) (+1k5 code)
// #define USE_BMP                                // [I2cDriver10] Enable BMP085/BMP180/BMP280/BME280 sensors (I2C addresses 0x76 and 0x77) (+4k4 code)
//     #define USE_BME68X                           // Enable support for BME680/BME688 sensor using Bosch BME68x library (+6k9 code)
// //  #define USE_BH1750                             // [I2cDriver11] Enable BH1750 sensor (I2C address 0x23 or 0x5C) (+0k5 code)
#ifdef USE_VEML6070
#undef USE_VEML6070
#endif                           // [I2cDriver12] Enable VEML6070 sensor (I2C addresses 0x38 and 0x39) (+1k5 code)
// //     #define USE_VEML6070_RSET    270000          // VEML6070, Rset in Ohm used on PCB board, default 270K = 270000ohm, range for this sensor: 220K ... 1Meg
// //     #define USE_VEML6070_SHOW_RAW                // VEML6070, shows the raw value of UV-A
#ifdef USE_VEML6075
#undef USE_VEML6075
#endif      


//  #define USE_ADS1115                            // [I2cDriver13] Enable ADS1115 16 bit A/D converter (I2C address 0x48, 0x49, 0x4A or 0x4B) based on Adafruit ADS1x15 library (no library needed) (+0k7 code)
// //  #define USE_INA219                             // [I2cDriver14] Enable INA219 (I2C address 0x40, 0x41 0x44 or 0x45) Low voltage and current sensor (+1k code)
//   //  #define INA219_SHUNT_RESISTOR (0.100)        // 0.1 Ohm default shunt resistor, can be overriden in user_config_override or using Sensor13
// //  #define USE_INA226 
                            // [I2cDriver35] Enable INA226 (I2C address 0x40, 0x41 0x44 or 0x45) Low voltage and current sensor (+2k3 code)
#ifndef USE_SHT3X
 #define USE_SHT3X  
#endif


// //  #define USE_RTC_CHIPS                          // Enable RTC chip support and NTP server - Select only one
// //    #define USE_DS3231                           // [I2cDriver26] Enable DS3231 RTC (I2C address 0x68) (+1k2 code)
// //    #define DS3231_ENABLE_TEMP

// #define USE_MQTT_TLS  

// #define DEBUG_TASMOTA_CORE                       // Enable core debug messages
// #define DEBUG_TASMOTA_DRIVER                     // Enable driver debug messages
#define DEBUG_TASMOTA_SENSOR                     // Enable sensor debug messages
//#define USE_DEBUG_DRIVER                         // Use xdrv_99_debug.ino providing commands CpuChk, CfgXor, CfgDump, CfgPeek and CfgPoke
#define USE_BERRY_DEBUG                        // Compile Berry bytecode with line number information, makes exceptions easier to debug. Adds +8% of memory consumption for compiled code

#ifndef USE_LVGL
// // -- LVGL Graphics Library ---------------------------------
#define USE_LVGL                                 // LVGL Engine, requires Berry (+382KB)
  #define USE_LVGL_PSRAM                         // Allocate LVGL memory in PSRAM if PSRAM is connected - this might be slightly slower but leaves main memory intact
  #define USE_LVGL_HASPMOTA                      // Enable OpenHASP compatiblity and Robotocondensed fonts (+90KB flash)
  #define USE_LVGL_MAX_SLEEP  10                 // max sleep in ms when LVGL is enabled, more than 10ms will make display less responsive
  #define USE_LVGL_PNG_DECODER                   // include a PNG image decoder from file system (+16KB)
  // #define USE_LVGL_TOUCHSCREEN                   // Use virtual touch screen with Berry driver
  #define USE_LVGL_FREETYPE                      // Use the FreeType renderer to display fonts using native TTF files in file system (+77KB flash)
#endif

#ifdef SERIAL_LOG_LEVEL
#undef SERIAL_LOG_LEVEL
#endif
#define SERIAL_LOG_LEVEL       LOG_LEVEL_DEBUG_MORE    // [SerialLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)


#ifdef WEB_LOG_LEVEL
#undef WEB_LOG_LEVEL
#endif
#define WEB_LOG_LEVEL          LOG_LEVEL_DEBUG_MORE    // [WebLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)


// #ifdef SOC_TOUCH_VERSION_1                       // ESP32
//   #define ESP32_TOUCH_THRESHOLD   40             // [TouchThres] Below this level a touch is detected
// #endif
// #ifdef SOC_TOUCH_VERSION_2                       // ESP32-S2 and ESP32-S3
//   #define ESP32_TOUCH_THRESHOLD   40000          // [TouchThres] Above this level a touch is detected
// #endif
///######## define one of these above once happy with build
///######## to experiment with touch support

#endif  // _USER_CONFIG_OVERRIDE_H_
