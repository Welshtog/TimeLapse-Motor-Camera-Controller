#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "Rail";
const char *password = "canon2017";
const int maxSlideTime = 7200000;                   // will adjust in testing
int ShutterPin = D5;                                // Camera Shutter Pin
int MotorPin = D6;                                  // Motor Pin
int EndLED   = D1;
int DirPin   = D2;
ESP8266WebServer server(80);

/* Web form */
void getRoot(){
     server.send(200, "text/html", "<!DOCTYPE html><html><head><title>Doug's funky tool</title><style>body {font-family: Arial, Helvetica, sans-serif;}label {font-size: 14px;line-height: 112%;mso-line-height-rule: exactly;font-weight: bold;}.title {font-size: 21.5px;font-weight: bold;text-align: center;}.container {background-color: #b2cefe;padding: 20px;width: 600px;}.input {width: 98%;padding: 12px 20px;margin: 4px 2px;box-sizing: border-box;background-color: #f1f1f1;}.button {background-color: #f1f1f1;border: none;color: black;padding: 16px 32px;text-decoration: none;margin: 4px 2px;cursor: pointer;float: right;}table,td,th {text-align: left;padding: 15px;}table {border-collapse: collapse;width: 100%;}table.header {border-collapse: collapse;width: 100%;background-color: #f1f1f1;}@media screen and (max-width:600px) {body {font-family: Arial, Helvetica, sans-serif;}label {font-size: 25px;font-weight: bold;}.title {font-size: 50px;font-weight: bold;text-align: center;}.container {background-color: #b2cefe;padding: 20px;width: 600px;}}</style></head><body><h2></h2><div class='container'><table class='header'><tr><td class='title'>Doug's funky tool</td></tr></table><table><form action='/' method='post'><tr><td></td></tr><tr><td><label for='shots'>Shots</label></td><td><input type='text' id='shots' name='shots' class='input' value='300'></td></tr><tr><td><label for='length'>Length</label></td><td><input type='text' id='length' name='length' class='input' value='50'></td></tr><tr><td><label for='slide'>Slide Time</label></td><td><input type='text' id='slide' name='slide' class='input' value='2000'></td></tr><tr><td><label for='dir'>Direction</label></td><td><select id='dir' name='dir' class='input'><option value='1'>Forward</option><option value='0'>Backward</option></select></td></tr><tr><td><label for='submit'></label></td><td><input type='submit' value='Submit' class='button'></td></tr></form></table></div></body></html>");
}

void postRoot() {
  int shotCount = atoi(server.arg("shots").c_str());
  int shotLength = atoi(server.arg("length").c_str());
  int slideTime = atoi(server.arg("slide").c_str());
  int dir = atoi(server.arg("dir").c_str());

if ((shotCount -1) * slideTime > maxSlideTime) {     // verify that the sequence will fit on the rail - TBC
  server.send(400, "text/html", "Shot sequence will not fit on slide rail.");
  return;
}

  server.send(201, "text/html", "");

  for (int i=1; i <= shotCount; i++) {
        digitalWrite(ShutterPin, HIGH);             //Shutter Pin on
  Serial.println("Start of shot.");
    delay(shotLength);
        digitalWrite(ShutterPin, LOW);              //Shutter Pin off
  Serial.println("End of shot.");
      delay(1000);                                  // wait a short period for the shutter to close (now 1s)
    if ((i < shotCount) && (dir == 0)){             // set direction pin to right rotation
        digitalWrite(DirPin, HIGH);
        digitalWrite(MotorPin, HIGH);               // start move
   Serial.println("Start of slide.");
       delay(slideTime); 
        digitalWrite(MotorPin, LOW);                // stop move
      Serial.println("End of slide.");
       delay(1000);                                  // wait a short period to allow vibrations to subside (now 1s)
    }
   else if ((i < shotCount) && (dir == 1)){
        digitalWrite(DirPin, LOW);                  // set direction pin to Left rotation
        digitalWrite(MotorPin, HIGH);               // start move
   Serial.println("Start of slide.");
       delay(slideTime); 
        digitalWrite(MotorPin, LOW);                // stop move
      Serial.println("End of slide.");
       delay(200);                                  // wait a short period to allow vibrations to subside
    }
    }
  }


void setup() {
    delay(1000);
    Serial.begin(115200);
    Serial.println();
    Serial.print("Configuring access point...");     // You can remove the password parameter if you want the AP to be open. */

    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    pinMode(ShutterPin, OUTPUT);
    pinMode(MotorPin, OUTPUT);
    pinMode(EndLED, OUTPUT);
    pinMode(DirPin, OUTPUT);

    server.on("/", HTTP_GET, getRoot);
    server.on("/", HTTP_POST, postRoot);
    server.begin();
    Serial.println("HTTP server started");
   
}

void loop() {
    server.handleClient();                        
}
