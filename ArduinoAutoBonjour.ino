//  Copyright (C) 2010 Georg Kaindl
//  http://gkaindl.com
//
//  This file is part of Arduino EthernetBonjour.
//
//  EthernetBonjour is free software: you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version.
//
//  EthernetBonjour is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with EthernetBonjour. If not, see
//  <http://www.gnu.org/licenses/>.
//

//  Illustrates how to register a Bonjour service.

#if defined(ARDUINO) && ARDUINO > 18
#include <SPI.h>
#endif
#include <Ethernet.h>
#include <EthernetBonjour.h>

byte mac[] = {  
  0x00, 0xAA, 0xBB, 0xCD, 0xDE, 0x02 };

// substitute an address on your own network here

// NOTE: Alternatively, you can use the EthernetDHCP library to configure your
//       Ethernet shield.

EthernetServer server(80);
int ledPin = 9;
int ledPin2 = 8;
int lux = 0;
int ldrpin = 1;
String readString = String(30);
String state = String(3);
 String state2 = String(3);

void setup()
{
    pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ldrpin, INPUT);
  digitalWrite(ledPin,LOW);
  digitalWrite(ledPin2,LOW);
  state = "OFF";
  state2 = "OFF";
  Serial.begin(9600);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  server.begin();
  // Initialize the Bonjour/MDNS library. You can now reach or ping this
  // Arduino via the host name "arduino.local", provided that your operating
  // system is Bonjour-enabled (such as MacOS X).
  // Always call this before any other method!
  EthernetBonjour.begin("arduino");

  // Now let's register the service we're offering (a web service) via Bonjour!
  // To do so, we call the addServiceRecord() method. The first argument is the
  // name of our service instance and its type, separated by a dot. In this
  // case, the service type is _http. There are many other service types, use
  // google to look up some common ones, but you can also invent your own
  // service type, like _mycoolservice - As long as your clients know what to
  // look for, you're good to go.
  // The second argument is the port on which the service is running. This is
  // port 80 here, the standard HTTP port.
  // The last argument is the protocol type of the service, either TCP or UDP.
  // Of course, our service is a TCP service.
  // With the service registered, it will show up in a Bonjour-enabled web
  // browser. As an example, if you are using Apple's Safari, you will now see
  // the service under Bookmarks -> Bonjour (Provided that you have enabled
  // Bonjour in the "Bookmarks" preferences in Safari).
  EthernetBonjour.addServiceRecord("Arduino Automator._http",
                                   80,
                                   MDNSServiceTCP);
}

void loop()
{ 
  // This actually runs the Bonjour module. YOU HAVE TO CALL THIS PERIODICALLY,
  // OR NOTHING WILL WORK! Preferably, call it once per loop().
  EthernetBonjour.run();

  // The code below is just taken from the "WebServer" example in the Ethernet
  // library. The only difference here is that this web server gets announced
  // over Bonjour, but this happens in setup(). This just displays something
  // in the browser when you connect.
// listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (readString.length() < 30) {
          readString.concat(c);
        }
 
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          int LED = readString.indexOf("LED1=");
          int LED2 = readString.indexOf("LED2=");
         
         
 
          if (readString.substring(LED,LED+6) == "LED1=T") {
            digitalWrite(ledPin,HIGH);
            state = "ON";
          }
          else if (readString.substring(LED,LED+6) == "LED1=F") {
            digitalWrite(ledPin,LOW);
            state = "OFF";
          }
          if (readString.substring(LED2,LED2+6) == "LED2=T") {
            digitalWrite(ledPin2,HIGH);
            state2 = "ON";
          }
          else if (readString.substring(LED2,LED2+6) == "LED2=F") {
            digitalWrite(ledPin2,LOW);
            state2 = "OFF";
          }
         
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
 
          client.print("LED1 is ");
          client.print(state);
          client.print("<br><br>");
          client.print("LED2 is ");
          client.print(state2);
          client.print("<br><br>");
         
         
          if (state == "ON") {
            client.println("<a href=\"./?LED1=F\">Turn Off<a>");
          }
          else {
            client.println("<a href=\"./?LED1=T\">Turn On<a>");
          }
         
         
          if (state2 == "ON") {
            client.println("<a href=\"./?LED2=F\">Turn Off<a>");
          }
          else {
            client.println("<a href=\"./?LED2=T\">Turn On<a>");
          }
          break;
        }
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
 
          // output the value of each analog input pin
         
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    readString = "";
    // close the connection:
    client.stop();
  }
 
 
}
