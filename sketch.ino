#define BLYNK_TEMPLATE_ID "TMPL3pejADN8O"
#define BLYNK_TEMPLATE_NAME "EV"
#define BLYNK_AUTH_TOKEN "o51vepgQ3Jemlbs1Ct3ZYp7l1VvAoUFL"

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h> 
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "B9GN_M0f5APaQmYDrCTTBxmx__yYb6Ik";
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int voltagePin = 34;
const int currentPin = 35;
const int warningLED = 16;
const int relayPin = 18;
const float voltageConversionFactor = 3.3 / 4095.0;
const float currentConversionFactor = 3.3 / 4095.0;

const int DHT_PIN = 15;

DHTesp dhtSensor;




void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  pinMode(warningLED, OUTPUT);
  digitalWrite(warningLED, LOW);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  Blynk.begin(auth, ssid, pass);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Battery Manager");
  display.display();
  delay(2000);
}

void loop() {
  Blynk.run();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, trying to reconnect...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Reconnected to WiFi");
  }

  int voltageValue = analogRead(voltagePin);
  float batteryVoltage = voltageValue * voltageConversionFactor * 4;

  int currentValue = analogRead(currentPin);
  float batteryCurrent = currentValue * currentConversionFactor;

  

  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  Serial.println("Temp: " + String(data.temperature, 2) + "°C");



  // Check if the temperature reading is valid
 
    // Apply the custom adjustment


  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Voltage: ");
  display.print(batteryVoltage);
  display.print(" V");

  display.setCursor(0, 10);
  display.print("Current: ");
  display.print(batteryCurrent);
  display.print(" A");

  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(data.temperature);
  display.print(" C");

  

  display.display();

  Blynk.virtualWrite(V0, batteryVoltage);
  Blynk.virtualWrite(V1, batteryCurrent);
  Blynk.virtualWrite(V2, data.temperature);

  if (batteryVoltage < 3.0 ||batteryVoltage > 6.0 || batteryCurrent > 2.0 || data.temperature > 60.0) {
    digitalWrite(warningLED, HIGH);
    Blynk.virtualWrite(V4, 255);
    Serial.println("Warning: Battery Alert!");
    digitalWrite(relayPin, LOW); 
  } 
  else {
    digitalWrite(warningLED, LOW);
    Blynk.virtualWrite(V4, 0);
    Serial.println("It's Working!");
    digitalWrite(relayPin, HIGH);
  }
  delay(1000);
}
