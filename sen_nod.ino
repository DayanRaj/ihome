#include <Arduino.h>


// temp=A0  pir=5  buzzer=6  ldr=A2 reedswitch=6
#include <dht.h>
#include <SoftwareSerial.h>

#define DHT11_PIN 7
#define DEBUG true

dht DHT;
SoftwareSerial esp8266(2, 3);
//*************PIR Start******************//
int PIR = 5; //PIR Sensor
int buzzer = 6;
//*************LDR Start******************//
int LDR = 7;
int LDRvalue = 0;
int light_sensitivity = 500;
int LED = 8;
int RS = 6;
int LED1 = 8;
int Reedswitch;
String Data = "";
String Object = "";
String b1 = ""; //PIR Sensor
String c1 = ""; //LDR Sensor
String d1 = ""; //Reed Switch

double Temperature, Humidity; String Temp = " ";
String Hum = " ";

void setup()
{
  pinMode(buzzer , OUTPUT);//PIR Sensor
  pinMode(PIR , INPUT);//PIR Sensor
  pinMode(RS, INPUT); //Reed Switch
  pinMode(LED, OUTPUT); //LED
  pinMode(LDR, INPUT); //LDR Sensor
  pinMode(LED1, OUTPUT); //Reed Switch Sensor
  Serial.begin(9600);
  esp8266.begin(9600);
  conn_establish();
}

void loop()
{
  test();
  send_info();
}
void conn_establish()
{
  Serial.print("conn_establish...");
  sendData("AT+RST\r\n", 3000, DEBUG); // reset module
  sendData("AT+CWMODE=1\r\n", 3000, DEBUG); // configure as access point
  sendData("AT+CWLAP\r\n", 3000, DEBUG);
  sendData("AT+CWJAP=\"lcAPtest\",\"sollamaten\"\r\n", 5000, DEBUG);
  //  sendData("AT+CWJAP=\"LC\",\"timepass\"\r\n", 5000, DEBUG);
  sendData("AT+CIFSR\r\n", 3000, DEBUG);
  sendData("AT+CIPMUX=1\r\n", 3000, DEBUG); // configure for multiple connections
  Serial.print("established...");
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command); // send the read character to the esp8266
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug == true)
  {
    Serial.print(response);

  }
  if (debug == false)
  {
  }

  return response;
}

void sendHTTPRequest(int connectionId, String content)
{
  // build HTTP request
  String httpRequest;
  httpRequest = "GET /iot/iot.php?DATA=" + Temp +  "&PIR=" + b1 + "&LDR=" + c1 + "&REEDSWITCH=" + d1 + " HTTP/1.1 \r\n";
  httpRequest += "Host:172.24.1.1";
  httpRequest += "\r\n\r\n";
  sendCIPData(connectionId, httpRequest);
}

void sendCIPData(int connectionId, String data)
{
  int stat = 0;
  String cipbuf = "";
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += data.length();
  cipSend += "\r\n";
  ///////////////////////////////////////////////////////////////
  esp8266.print(cipSend); // send the read character to the esp8266
  while (esp8266.available())
  {
    // The esp has data so display its output to the serial window
    char c = esp8266.read(); // read the next character.
    if (c == '>')
      stat = 1;
    cipbuf += c;
  }
  Serial.println(cipbuf);
  if (stat == 1)
  {
    sendData(data, 7000, DEBUG);
  }
  else
  {
    Serial.println("Error sending data");
  }
}

void send_info()
{
  String packet = "";
  //*************************WSN01******************************
  packet = "0," + Temp + "," + Hum + "," + b1 + "," + c1 + ",WSN01" + ",FE";
  sendData("AT+CIPSTART=0,\"TCP\",\"172.24.1.1\",117\r\n", 5000, DEBUG);
  sendCIPData(0, packet);
  sendData("AT+CIPCLOSE=0\r\n", 3000, DEBUG);
  Data = "";
  //*************************WSN02******************************
//    packet="0,"+Temp+","+Hum+","+b1+","+c1+",WSN02"+",FE";
//    sendData("AT+CIPSTART=1,\"TCP\",\"172.24.1.1\",117\r\n", 5000, DEBUG);
//    sendCIPData(1,packet);
//    sendData("AT+CIPCLOSE=0\r\n", 3000, DEBUG);
//    Data = "";
  //*************************WSN03******************************
//    packet="0,"+Temp+","+Hum+","+b1+","+c1+",WSN03"+",FE";
//    sendData("AT+CIPSTART=2,\"TCP\",\"172.24.1.1\",117\r\n", 5000, DEBUG);
//    sendCIPData(2,packet);
//    sendData("AT+CIPCLOSE=0\r\n", 3000, DEBUG);
//    Data = "";
  //*************************WSN04******************************
//    packet="0,"+Temp+","+Hum+","+b1+","+c1+",WSN04"+",FE";
//    sendData("AT+CIPSTART=3,\"TCP\",\"172.24.1.1\",117\r\n", 5000, DEBUG);
//    sendCIPData(3,packet);
//    sendData("AT+CIPCLOSE=0\r\n", 3000, DEBUG);
//    Data = "";
  //*************************WSN05******************************
  //  packet="0,"+Temp+","+Hum+","+b1+","+c1+",WSN05"+",FE";
  //  sendData("AT+CIPSTART=4,\"TCP\",\"172.24.1.1\",117\r\n", 5000, DEBUG);
  //  sendCIPData(4,packet);
  //  sendData("AT+CIPCLOSE=0\r\n", 3000, DEBUG);
  //  Data = "";
}


void test()
{
  //********************TEMPERATURE Start***********************

  //  int t = analogRead(A0);
  //  int t_value = t *4.88 / 10;
  //  Data = (String)t_value;
  //  Serial.println(Data);

  //DHT***************************************
  int chk = DHT.read11(DHT11_PIN);
  Temperature = DHT.temperature;
  Humidity = DHT.humidity;
  Temp = (String) Temperature;
  Hum = (String) Humidity;
  
  //**********************PIR Start***********************************
  //  Object b1=(String)PIR;
  int PIRvalue = 0;

  PIRvalue = digitalRead(PIR);
  if (PIRvalue == LOW)
  {
    b1 = "Y";
    digitalWrite(buzzer, LOW);
  }
  else
  {
    b1 = "N";
    digitalWrite(buzzer, HIGH);
    delay(50);
    digitalWrite(buzzer, LOW);
  }
  Serial.println(b1);
  //**********************PIR End***********************************



  //************************LDR Start************************************

  LDRvalue = analogRead(A2);
  if (LDRvalue > light_sensitivity)
  {
    c1 = "On";
    digitalWrite(LED, HIGH);
  }
  else
  {
    c1 = "Of";
    digitalWrite(LED, LOW);
  }
  Serial.println(c1);
  //************************LDR End*******************************

  //***********************Reed switch Start**********************
//  Reedswitch = digitalRead(RS);
//  if (Reedswitch == HIGH )
//  {
//    d1 = "On";
//    digitalWrite(LED1, HIGH);
//  }
//  else
//  {
//    d1 = "Off";
//    digitalWrite(LED1, LOW);
//  }
//  Serial.println(d1);
}



