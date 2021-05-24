#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
String readString;
const char* ssid = "THOR-ADI";
const char* password = "justis123";
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbzY5fMJ0fi8x9ih1FqnXkPxQJM-5go6cWuxX16iAGxJ43QxqHv_uP8lvgtgqzJ2IZMMaA";
const int RS = D0, EN = D1, d4 = D2, d5 = D8, d6 = 3, d7 = 1;
LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);
DHT dht(D3, DHT11);
int oxygen = 96;
int pulse = 89;
int temp;

#define SS_PIN D4
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup()
{
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Send test data
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Scan RFID");
  delay(5000);
  lcd.clear();

}
void sendData1(int oxygen, int pulse, int temp, int bed)
{
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  String string_oxygen     =  String(oxygen, DEC);
  String string_pulse     =  String(pulse, DEC);
  String string_temp     =  String(temp, DEC);
  String string_bed     =  String(bed, DEC);

  String url = "/macros/s/" + GAS_ID + "/exec?id=56&temp=" +string_temp+ "&oxygen=" +string_oxygen+ "&pulse_rate=" +string_pulse+ "&bed_no=" +string_bed;
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
void loop()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  temp = dht.readTemperature();
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();



  if (content.substring(1) == "6A B1 A6 B2") //change here the UID of the card/cards that you want to give access
  {
    lcd.setCursor(0, 0);
    lcd.print("Bed 1");
    lcd.setCursor(0, 1);
    lcd.print("Scan Oxymeter");
    delay(5000);
    lcd.clear();
    //    oxygen = pox.getSpO2();
    //    heart = pox.getHeartRate();
    lcd.setCursor(0, 0);
    lcd.print("Spo2: ");
    lcd.print(oxygen);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Pulse: ");
    lcd.print(pulse);
    delay(10000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("C");
    delay(10000);
    lcd.clear();
    sendData1(oxygen, pulse, temp, 1);


  }

  else if (content.substring(1) == "5A FC 86 B1")  {
    lcd.setCursor(0, 0);
    lcd.print("Bed 2");
    lcd.setCursor(0, 1);
    lcd.print("Scan Oxymeter");
    delay(5000);
    lcd.clear();
    //    oxygen = pox.getSpO2();
    //    heart = pox.getHeartRate();
    lcd.setCursor(0, 0);
    lcd.print("Spo2: ");
    //lcd.setCursor(0, 5);
    lcd.print(oxygen);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Pulse: ");
    lcd.print(pulse);
    delay(10000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print("C");
    delay(10000);
    lcd.clear();
    sendData1(oxygen, pulse, temp, 2);

  }
  else {
    lcd.print("Scan RFID");
    delay(500);
    lcd.clear();
  }

}
