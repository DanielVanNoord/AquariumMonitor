/*
 AquaMon tester based on the WizFi360 example: WebServer

 This is the AquaMon test program. It runs all sensors and can return the data in a basic web page or JSON. Before building verify the following items
 Set your SSID and Password in arduino_secrets.h
 Either uncomment SIMULATION_AM in board.h or add the sensors to the project.
 Set the JSON define in this file to return JSON or (with it commented out) HTML
*/

#include <WizFi360.h>
#include <WizFi360Client.h>
#include <WizFi360Server.h>
#include <WizFi360Udp.h>

#include <SPI.h>
#include <SD.h>

#include "arduino_secrets.h"
#include "board.h"
#include "water_temp_sensor.h"
#include "light_sensors.h"
#include "hall_sensors.h"
#include "bme_sensor.h"
#include "leak_sensor.h"

// setup according to the device you use
#define WIZFI360_EVB_PICO

// Comment this out to return HTML instead of JSON
#define JSON

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
#if defined(ARDUINO_MEGA_2560)
SoftwareSerial Serial1(6, 7); // RX, TX
#elif defined(WIZFI360_EVB_PICO)
SoftwareSerial Serial2(6, 7); // RX, TX
#endif
#endif

/* Baudrate */
#define SERIAL_BAUDRATE 115200
#if defined(ARDUINO_MEGA_2560)
#define SERIAL1_BAUDRATE 115200
#elif defined(WIZFI360_EVB_PICO)
#define SERIAL2_BAUDRATE 115200
#endif

int status = WL_IDLE_STATUS; // the Wifi radio's status

int reqCount = 0; // number of requests received

WiFiServer server(80);

File root;

void setup()
{
  // initialize serial for debugging
  Serial.begin(SERIAL_BAUDRATE);

  // give the user a moment to get the serial terminal connected.
  yield();
  delay(3000);

#ifndef SIMULATION_AM
//Print micro sd files from SPI1
  SPI1.setRX(12);
  SPI1.setCS(13);
  SPI1.setSCK(10);
  SPI1.setTX(11);

  Serial.print("Initializing SD card...");

  if (!SD.begin(13, SPI1)) {
    Serial.println("initialization of SD failed!");
  }
  else
  {
  Serial.println("initialization of SD done.");

  root = SD.open("/");

  printDirectory(root, 0);

  Serial.println("SD read done!");
  }
  
#endif
  
  // initialize serial for WizFi360 module
#if defined(ARDUINO_MEGA_2560)
  Serial1.begin(SERIAL1_BAUDRATE);
#elif defined(WIZFI360_EVB_PICO)
  Serial2.begin(SERIAL2_BAUDRATE);
#endif
  // initialize WizFi360 module
#if defined(ARDUINO_MEGA_2560)
  WiFi.init(&Serial1);
#elif defined(WIZFI360_EVB_PICO)
  WiFi.init(&Serial2);
#endif

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true)
      ;
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  #ifndef SIMULATION_AM
  // for complete reasons of madness this hangs when in simulator mode but not in run mode
  printWifiStatus();
  #endif

  Serial.println("Setting up sensors");
  // Setup the sensor or simulations on the board.
  init_light_sensors();
  init_hall_sensors();
  init_bme_sensor();
  init_water_temp_sensors();
  init_leak_sensor();

  Serial.println("Sensors are ready");

  // start the web server on port 80
  server.begin();
}

void loop()
{
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("New client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          Serial.println("Sending response");

#ifdef JSON
          // For JSON we are building a basic JSON response with all of the data
          char resp[512];

          sprintf(resp, "{\"temperature\": %f, \"humidity\": %f, \"pressure\": %f, \"hall1\": %d, \"hall2\": %d, \"water_temp1\": %f, \"water_temp2\": %f, \"light1\": %d, \"light2\": %d, \"leak\": %d}",
                  read_bme_temp(),
                  read_bme_humidity(),
                  read_bme_pressure(),
                  read_hall_sensor_1(),
                  read_hall_sensor_2(),
                  read_water_temp_sensor_1(),
                  read_water_temp_sensor_2(),
                  read_light_sensor_1(),
                  read_light_sensor_2(),
                  read_leak_sensor());
                  

          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send
          client.print(
              "HTTP/1.1 200 OK\r\n"
              "Content-Type:application/json\r\n"
              "Connection: close\r\n" // the connection will be closed after completion of the response
              "\r\n");

          // send the JSON data
          client.print(resp);
#else
          client.print(
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/html\r\n"
              "Connection: close\r\n" // the connection will be closed after completion of the response
              "Refresh: 20\r\n"       // refresh the page automatically every 20 sec
              "\r\n");
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>\r\n");
          client.print("<h1>Hello AquaMon!</h1>\r\n");
          client.print("Requests received: ");
          client.print(++reqCount);
          client.print("<br>\r\n");
          client.print("Light analog input A0: ");
          client.print(read_light_sensor_1());
          client.print("<br>\r\n");
          client.print("Light analog input A1: ");
          client.print(read_light_sensor_2());
          client.print("<br>\r\n");
          client.print("BME Temp: ");
          client.print(read_bme_temp());
          client.print("<br>\r\n");
          client.print("BME Hum: ");
          client.print(read_bme_humidity());
          client.print("<br>\r\n");
          client.print("BME Pres: ");
          client.print(read_bme_pressure());
          client.print("<br>\r\n");

          client.print("<br>\r\n");

          client.print("HALL 1: ");
          client.print(read_hall_sensor_1());
          client.print("<br>\r\n");

          client.print("HALL 2: ");
          client.print(read_hall_sensor_2());
          client.print("<br>\r\n");

          client.print("LEAK 1: ");
          client.print(read_leak_sensor());
          client.print("<br>\r\n");

          client.print("Water Temp 1: ");
          client.print(read_water_temp_sensor_1());
          client.print("<br>\r\n");

          client.print("Water Temp 2: ");
          client.print(read_water_temp_sensor_2());
          client.print("<br>\r\n");

          client.print("</html>\r\n");
#endif
          break;
        }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t cr = entry.getCreationTime();
      time_t lw = entry.getLastWrite();
      struct tm * tmstruct = localtime(&cr);
      Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
      tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    }
    entry.close();
  }
}
