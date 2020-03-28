/****************************************************************************************************************************
   Mega_WiFiNINA.ino
   For AVR or Generic boards using WiFiNINA Modules/Shields, using much less code to support boards with smaller memory

   WiFiManager_NINA_WM_Lite is a library for the Mega, Teensy, SAM DUE, SAMD and STM32 boards (https://github.com/khoih-prog/WiFiManager_NINA_Lite)
   to enable store Credentials in EEPROM to easy configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services
   without Hardcoding.

   Built by Khoi Hoang https://github.com/khoih-prog/WIFININA_WM_Lite
   Licensed under MIT license
   Version: 1.0.1

   Version Modified By   Date        Comments
   ------- -----------  ----------   -----------
   1.0.0   K Hoang      26/03/2020  Initial coding
   1.0.1   K Hoang      27/03/2020  Fix SAMD soft-reset bug. Add support to remaining boards
 *****************************************************************************************************************************/

/* Comment this out to disable prints and save space */
#define DEBUG_WIFI_WEBSERVER_PORT Serial
#define WIFININA_DEBUG_OUTPUT     Serial

#if !( defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) )
#error This code is intended to run only on the Arduino Mega 1280/2560 boards ! Please check your Tools->Board setting.
#endif

#if defined(ARDUINO_AVR_MEGA2560)
#define BOARD_TYPE      "AVR Mega2560"
#else
#define BOARD_TYPE      "AVR Mega"
#endif

// Start location in EEPROM to store config data. Default 0
// Config data Size currently is 128 bytes)
#define EEPROM_START      0

#include <WiFiManager_NINA_Lite.h>

#define MAX_BLYNK_SERVER_LEN      34
char Blynk_Server [MAX_BLYNK_SERVER_LEN]  = "";

#define MAX_BLYNK_TOKEN_LEN       34
char Blynk_Token  [MAX_BLYNK_TOKEN_LEN]   = "";

#define MAX_BLYNK_PORT_LEN        6
char Blynk_Port   [MAX_BLYNK_PORT_LEN]  = "";

#define MAX_MQTT_SERVER_LEN      34
char MQTT_Server  [MAX_MQTT_SERVER_LEN]   = "";

MenuItem myMenuItems [] = 
{
  { "sv", "Blynk Server", Blynk_Server, MAX_BLYNK_SERVER_LEN },
  { "tk", "Token",        Blynk_Token,  MAX_BLYNK_TOKEN_LEN },
  { "pt", "Port",         Blynk_Port,   MAX_BLYNK_PORT_LEN },
  { "mq", "MQTT Server",  MQTT_Server,  MAX_MQTT_SERVER_LEN },
};

uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);  //MenuItemSize;

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print(F("H"));        // H means connected to WiFi
  else
    Serial.print(F("F"));        // F means not connected to WiFi
  
  if (num == 80) 
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0) 
  {
    Serial.print(F(" "));
  }
} 

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define HEARTBEAT_INTERVAL    600000L
  // Print hearbeat every HEARTBEAT_INTERVAL (600) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

WiFiManager_NINA_Lite* WiFiManager_NINA;

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  Serial.print(F("\nStart Mega_WiFiNINA on "));
  Serial.println(BOARD_TYPE);

  WiFiManager_NINA = new WiFiManager_NINA_Lite();

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  //WiFiManager_NINA->setConfigPortalIP(IPAddress(192, 168, 120, 1));
  //WiFiManager_NINA->setConfigPortalChannel(1);

  WiFiManager_NINA->begin();
}

void displayCredentials(void)
{
  Serial.println("Your stored Credentials :");

  for (int i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.println(String(myMenuItems[i].displayName) + " = " + myMenuItems[i].pdata);
  }
}

void loop()
{
  WiFiManager_NINA->run();

  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (int i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }

  check_status();
}
