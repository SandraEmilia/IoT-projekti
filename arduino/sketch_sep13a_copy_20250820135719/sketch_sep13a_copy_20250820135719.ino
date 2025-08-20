#include <SPI.h>
#include <WiFiNINA.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClient.h>
#include <ArduinoHttpClient.h>
#include <Servo.h>


// Wifi-tiedot
char ssid[] = "YOUR WIFI";
char pass[] = "YOUR PASSWORD";

//DHT11 anturi
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


// Muuttujat
WiFiServer server(80);
float lampotila = 0;
int minTemp = 0;
int maxTemp = 100;

// Palvelimen IP ja portti
char serverAddress[] = "192.168.1.3";
int port = 8080;
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, serverAddress, port);

// Näyttö
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Haetaan asetukset palvelimelta
bool fetchSettings(int &minTemp, int &maxTemp) {
  client.get("/settings");
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Palvelimen vastauskoodi: ");
  Serial.println(statusCode);
  Serial.print("Palvelimen vastaus: ");
  Serial.println(response);

  if (statusCode == 200) {
    int minIndex = response.indexOf("minTemperature");
    int maxIndex = response.indexOf("maxTemperature");
    if (minIndex != -1 && maxIndex != -1) {
      int colonMin = response.indexOf(':', minIndex);
      int commaMin = response.indexOf(',', colonMin);
      int colonMax = response.indexOf(':', maxIndex);
      int endBrace = response.indexOf('}', colonMax);

      String minString = response.substring(colonMin + 1, commaMin);
      String maxString = response.substring(colonMax + 1, endBrace);

      minTemp = minString.toInt();
      maxTemp = maxString.toInt();

      return true;
    }
  }
  return false;
}

// Servo-moottori 

Servo servo;
const int servoPin = 9;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  dht.begin();

  // Yhdistää wifiin

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Yhdistetään WiFiin...");
    delay(5000);
  }

  Serial.print("Yhdistetty! IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED-näyttöä ei löydy!"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("OLED valmis!");
  display.display();
  delay(2000);

  // Servon alustus
  servo.attach(servoPin);
  servo.write(0);
}

void loop() {
  

  if (!fetchSettings(minTemp, maxTemp)) {
    Serial.println("Asetusten haku epäonnistui");
  }
  
  // Luetaan lämpötila
  lampotila = dht.readTemperature();

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);

  if (isnan(lampotila)) {
    Serial.println("Sensoriluku epäonnistui.");
  } else {
    Serial.print("Temperature: ");
    Serial.print(lampotila);
    Serial.println(" °C");
    display.print("Temperature: ");
    display.print(lampotila);
    display.println(" C");

    display.print("Min: ");
    display.println(minTemp);
    display.print("Max: ");
    display.println(maxTemp);

    if (lampotila > maxTemp) {
      Serial.println("Kuuma - avataan");
      display.println("Kuuma");
      servo.write(90); // avaa
    } else if (lampotila < minTemp) {
      Serial.println("Kylmä - suljetaan");
      display.println("Kylmä");
      servo.write(0); // sulje
    } else {
      Serial.println("Just hyvä - pidetään ennallaan");
      display.println("Just hyvä");
    }

  }
  display.display();

// Lähetetään lämpötila palvelimelle
String postData = "{\"temperature\": " + String((int)lampotila) + "}";

client.beginRequest();
client.post("/temperature");
client.sendHeader("Content-Type", "application/json");
client.sendHeader("Content-Length", postData.length());
client.beginBody();
client.print(postData);
client.endRequest();

// Vastaus palvelimelta
int statusCode = client.responseStatusCode();
String response = client.responseBody();



  delay(10000);
}
