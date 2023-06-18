/*
Title   : Real Time Data Transfer From NodeMcu/ESP01 to Server
Project : IoT Enabled, Self-Calibrating, and Self-Healing Sensor System for In-situ Agriculture Applications 
          under the TIH-IoT CHANAKYA Group Fellowship 2021-2022, IIT-Bombay
Author  : Paavan Parekh, Param Parekh
Version : 1.0

*/
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define RX 3 
#define TX 2 
String WIFI_SSID = "";        // SSID
String PASSWORD = "";         // Password


String HOST = "192.168.87.123";
String PATH = "/nodes/store_feeds";
String PORT = "8000";

int countTrueCommand;
int countTimeCommand;
boolean found = false;

SoftwareSerial esp8266(RX, TX);


void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);
  esp8266.println("AT");
  Serial.println(esp8266.read());
  sendCommandToESP8266("AT", 5, "OK");
  sendCommandToESP8266("AT+CWMODE=1", 5, "OK");
  sendCommandToESP8266("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + PASSWORD + "\"", 20, "OK");
}

void loop() {
  
 
  // we can take  input from Sensors for below values of Temperature, Humidity, etc. 
  // For testing purpose, i have hard-coded values.        
  String data;
  int node_id = 1;
  float temperature = 23.05;
  float humidity = 45;
  float LWS = 34;
  float soil_temperature = 34;
  float soil_moisture = 467;
  float battery_status = 50;
  

  data = "{\"node_id\":" + String(node_id) +  ",\"temperature\":" + String(temperature,2)+ 
  ",\"humidity\":" + String(humidity,2) +",\"LWS\":" + String(LWS,2)+ ",\"soil_temperature\":" +
  String(soil_temperature,2)+ ",\"soil_moisture\":"+String(soil_moisture,2)+ ",\"battery_status\":"+
  String(battery_status,2) + "}";
  
 

  String postRequest = "POST " + PATH  + " HTTP/1.1\r\n" +
                       "Host: " + HOST + "\r\n" +
                       "Accept: *" + "/" + "*\r\n" +
                       "Content-Length: " + data.length() + "\r\n" +
                       "Content-Type: application/json\r\n" +
                       "\r\n" + data;

  sendCommandToESP8266("AT+CIPMUX=1", 5, "OK");
  sendCommandToESP8266("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  String cipSend = "AT+CIPSEND=0," + String(postRequest.length());
  sendCommandToESP8266(cipSend, 18, ">");
  sendData(postRequest);
  sendCommandToESP8266("AT+CIPCLOSE=0", 5, "OK");
}

void sendCommandToESP8266(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while (countTimeCommand < (maxTime * 1))
  {
    esp8266.println(command);
    if (esp8266.find(readReplay))
    {
      found = true;
      break;
    }

    countTimeCommand++;
  }

  if (found == true)
  {
    Serial.println("Success");
    countTrueCommand++;
    countTimeCommand = 0;
  }

  if (found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }

  found = false;
}

void sendData(String postRequest) {
  Serial.println("data is sent");
  esp8266.println(postRequest);
  delay(8000);
  countTrueCommand++;
}
