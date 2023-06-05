#include <Arduino.h>
#include <ESP8266WiFi.h>

///////////////////////////////////////////////////////////////
/*
         |
        -+-
         A
        /=\               /\  /\    ___  _ __  _ __ __    __
      i/ O \i            /  \/  \  / _ \| '__|| '__|\ \  / /
      /=====\           / /\  /\ \|  __/| |   | |    \ \/ /
      /  i  \           \ \ \/ / / \___/|_|   |_|     \  /
    i/ O * O \i                                       / /
    /=========\        __  __                        /_/    _
    /  *   *  \        \ \/ /        /\  /\    __ _  ____  | |
  i/ O   i   O \i       \  /   __   /  \/  \  / _` |/ ___\ |_|
  /=============\       /  \  |__| / /\  /\ \| (_| |\___ \  _
  /  O   i   O  \      /_/\_\      \ \ \/ / / \__,_|\____/ |_|
i/ *   O   O   * \i
/=================\
       |___|


THIS JUST USES ALEXA NOW. NO NEED FOR THE API OR ANYTHING ELSE

*/
/////////////////////////////////////////////////////////////////

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5;      // Thing's onboard, green LED
const int ANALOG_PIN = A0;  // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

#include "fauxmoESP.h"

#define SERIAL_BAUDRATE 9600

#define WIFI_SSID "bespin"
#define WIFI_PASS "walterswifi2021"

#define LAMP "joy"

fauxmoESP fauxmo;

// Wi-Fi Connection
void wifiSetup()
{
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void initHardware()
{
  Serial.begin(9600);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void setup()
{
  initHardware();
  wifiSetup();

  fauxmo.createServer(true);
  fauxmo.setPort(80);
  fauxmo.enable(true);
  fauxmo.addDevice(LAMP);
  fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value)
                    {    
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    if ( (strcmp(device_name, LAMP) == 0) ) {
      Serial.println("LAMP switched by Alexa");
      if (state) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    } });
}

void loop()
{
  // fauxmoESP uses an async TCP server but a sync UDP server
  // Therefore, we have to manually poll for UDP packets
  fauxmo.handle();

  static unsigned long last = millis();
  if (millis() - last > 5000)
  {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }
}
