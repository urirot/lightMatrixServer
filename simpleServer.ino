#include <WiFi101.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
/*

  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.

  This sketch will print the IP address of your WiFi Shield (once connected)

  to the Serial monitor. From there, you can open that address in a web browser

  to turn on and off the LED on pin 9.

  If the IP address of your shield is yourAddress:

  http://yourAddress/H turns the LED on

  http://yourAddress/L turns it off

  This example is written for a network using WPA encryption. For

  WEP or WPA, change the WiFi.begin() call accordingly.

  Circuit:

   WiFi shield attached

   LED attached to pin 9

  created 25 Nov 2012

  by Tom Igoe

*/
#include <SPI.h>
#include <WiFi101.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define NUMPIXELS 256
#define NUM_COL 16
#define NUM_ROW 16
#define PIXEL_PIN 6
#define PIXEL_TYPE NEO_GRB + NEO_KHZ800

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NUM_COL, NUM_ROW, PIXEL_PIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                            PIXEL_TYPE);
#define DELAYVAL 500


const uint16_t colors[] = {matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);      // initialize serial communication

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // check for the presence of the shield:

  if (WiFi.status() == WL_NO_SHIELD) {

    Serial.println("WiFi shield not present");

    while (true);       // don't continue

  }

  // attempt to connect to WiFi network:

  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to Network named: ");

    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);

  }

  server.begin();                           // start the web server on port 80

  printWiFiStatus();                        // you're connected now, so print out the status

  matrix.begin();
  matrix.setTextWrap(false);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
}


int x    = matrix.width();
int color_pass = 0;

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected

      if (client.available()) {             // if there's bytes to read from the client,

        char c = client.read();             // read a byte, then

        Serial.write(c);                    // print it out the serial monitor

        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.

          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");

            client.println("Content-type:text/html");

            client.println();

            // the content of the HTTP response follows the header:

            client.print("Click <a href=\"/first\">here</a> to try configuration 1<br>");

            client.print("Click <a href=\"/second\">here</a> to try configuration 2<br>");

            // The HTTP response ends with another blank line:

            client.println();

            // break out of the while loop:

            break;

          }

          else {      // if you got a newline, then clear currentLine:

            currentLine = "";

          }

        }

        else if (c != '\r') {    // if you got anything else but a carriage return character,

          currentLine += c;      // add it to the end of the currentLine

        }

        // Check to see if the client request was "GET /first" or "GET /second":

        if (currentLine.endsWith("GET /first")) {
         
          firstSetting();
          // break out of the while loop:

          break;

        }

        if (currentLine.endsWith("GET /second")) {

          secondSetting();
          // break out of the while loop:

          break;

        }
      }
    }

    // close the connection:
    client.stop();
    Serial.println("client disonnected");

  }
  else {
    IPAddress ip;                             // the IP address of your shield
    ip = WiFi.localIP();
    printIP(ip);
  }
}

void printWiFiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");

  // print where to go in a browser:

  Serial.print("To see this page in action, open a browser to http://");

  Serial.println(ip);
}

void printIP(IPAddress ip) {
  matrix.fillScreen(0);
  matrix.setBrightness(5);
  matrix.setCursor(x, 4);

  matrix.print(ip);

  if (--x < -1 * 16 * 6) {
    x = matrix.width();
    if (++color_pass >= 3) color_pass = 0;
    matrix.setTextColor(colors[color_pass]);
  }
  matrix.show();
  delay(80);
}

void firstSetting() {
  pixels.clear();
  pixels.setBrightness(5);
  for (int i = 0; i < NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 150, 150));
  }
  pixels.show();
  delay(1000);
}

void secondSetting() {
  pixels.clear();
  pixels.setBrightness(5);
  for (int i = 0; i < NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(150, 150, 0));
  }
  pixels.show();
  delay(1000);
}
