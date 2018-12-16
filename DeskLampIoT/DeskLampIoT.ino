/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will print the IP address of your WiFi Shield (once connected)
  to the Serial monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 5.

  If the IP address of your shield is yourAddress:
  http://yourAddress/H turns the LED on
  http://yourAddress/L turns it off

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the Wifi.begin() call accordingly.

  Circuit:
   WiFi shield attached
   LED attached to pin 5

  created for arduino 25 Nov 2012
  by Tom Igoe

  ported for sparkfun esp32
  31.01.2017 by Jan Hendrik Berlin

*/

#include <WiFi.h>
#include "time.h"

const char* ssid     = "SUPERONLINE_WiFi_8555";
const char* password = "U9JVXAPUV49H";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 10800;
const int   daylightOffset_sec = 0;

WiFiServer server(3340);
int alarm_hour = 0, alarm_minute = 0;
int tshut_hour = 0, tshut_minute = 0, tshut_second = 0;
struct tm timeinfo;
struct tm remTime;
bool timerSet = false, alarmSet = false;

void setup()
{
  Serial.begin(115200);
  pinMode(12, OUTPUT);     // Relay pin
  digitalWrite(12, HIGH);  // HIGH signal breaks the current
  delay(10);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Repeat until the module is connected.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Repeat until the module gets time info from NTP server
  while (!getLocalTime(&timeinfo))
  {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
    delay(5000);
  }

  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";
    // make a String to hold incoming data from the client
    bool requestNotProcessed = false;
    while (client.connected())
    {
      // loop while the client's connected
      if (client.available())
      {
        // If there's bytes to read from the client, read them all.
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        currentLine += c;                   // add it to the end of the currentLine
        requestNotProcessed = true;         // Mark request flag to true
      }
      else if (requestNotProcessed)
      {
        // After all characters are read, process the command
        processCommand(currentLine);
        currentLine = "";                   // Reset the command buffer
        requestNotProcessed = false;        // Reset the request flag
      }
      else
      {
        // If there is nothing to do, print current time every 5 seconds
        if (getLocalTime(&timeinfo) && (timeinfo.tm_sec % 5 == 0))
        {
          printLocalTime();
          checkCommands();
          delay(1000);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  else
  {
    // If there is no connection, print current time as heartbeat.
    delay(1000);
    printLocalTime();
    checkCommands();
  }
}

// Processes commands sent over the internet. Current options are
// Alarm(AHH[MM])   : Lights up the desk lamp at given hour and minute
// Timer(RHH[MMSS]) : Shuts down the desk lamp at current time plus given duration.
// Open(O)        :Lights up the desk lamp instantly
// Close(C)       :Lights down the desk lamp instantly
void processCommand(String command)
{
  command.toUpperCase();
  if (command[0] == 'A')
  {
    Serial.println("Alarm Request");

    // Hour parameter is mandatory with this command
    alarm_hour = command.substring(1, 3).toInt();
    Serial.print("Hour: ");
    Serial.println(alarm_hour);

    // Minute parameter is option, 0 if not given.
    if (command[3] != '\0')
      alarm_minute = command.substring(3, 5).toInt();
    else
      alarm_minute = 0;

    Serial.print("Minute: ");
    Serial.println(alarm_minute);

    // Set Alarm flag
    alarmSet = true;
  }
  else if (command.startsWith("R"))
  {
    Serial.println("Timed Shutdown Request");

    // Hour parameter is mandatory with this command.
    tshut_hour = command.substring(1, 3).toInt();
    Serial.print("Hour: ");
    Serial.println(tshut_hour);

    // Minute parameter is optional, 0 if not given.
    if (command[3] != '\0')
      tshut_minute = command.substring(3, 5).toInt();
    else
      tshut_minute = 0;
    Serial.print("Minute: ");
    Serial.println(tshut_minute);

    // Second parameter is optional, 0 if not given.
    if (command[5] != '\0')
      tshut_second = command.substring(5, 7).toInt();
    else
      tshut_second = 0;
    Serial.print("Second: ");
    Serial.println(tshut_second);

    // Take a copy of current time data, add given parameters,
    //then correct overflows with mktime() function.
    memcpy(&remTime, &timeinfo, sizeof(remTime));
    remTime.tm_hour += tshut_hour;
    remTime.tm_min += tshut_minute;
    remTime.tm_sec += tshut_second;
    mktime(&remTime);
    Serial.print("Light will be off on: ");
    Serial.println(&remTime, "%A, %B %d %Y %H:%M:%S");
    timerSet = true;
  }
  else if (command[0] == 'O')
  {
    Serial.println("Open Request");
    digitalWrite(12, LOW);
  }
  else if (command[0] == 'C')
  {
    Serial.println("Close Request");
    digitalWrite(12, HIGH);
  }
  command = "";
}

// Prints local time
void printLocalTime()
{
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// Checks the timer and shuts down the desk lamp.
void checkTimer()
{
  if (timerSet)
  {
    time_t curTime = mktime(&timeinfo);
    time_t futureTime = mktime(&remTime);
    double remSecs = difftime(futureTime, curTime);
    if (remSecs <= 0)
    {
      Serial.println("Alarm Time.");
      timerSet = false;
      digitalWrite(12, HIGH);
    }
    else
    {
      Serial.print(remSecs);
      Serial.println(" seconds to Alarm Time.");
    }
  }
}

// Checks the alarm conditions and lights up the desk lamp.
void checkAlarm()
{
  if (alarmSet)
  {
    if (timeinfo.tm_hour == alarm_hour && timeinfo.tm_min == alarm_minute)
    {
      alarmSet = false;
      digitalWrite(12, LOW);
      Serial.println("Alarm Time.");
    }
  }
}

// Checks the alarms and timers if they shall be activated upon
void checkCommands()
{
  checkTimer();
  checkAlarm();
}
