/*********
  Modified by Henrik
  Based on code from Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
  Lora ttgo-lora32-oled V1, Reciver , Relay connected to GPIO2 
*********/

// Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

// Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// Frequency Band (433E6 for Asia, 866E6 for Europe, 915E6 for North America)
#define BAND 866E6

// OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Relay pin
#define RELAY_PIN 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;
bool relayState = false;
bool connected = false;
unsigned long lastPacketTime = 0;
const unsigned long connectionTimeout = 5000; // 5 seconds timeout for connection

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA RECEIVER ");
  display.display();

  Serial.println("LoRa Receiver Test");

  // SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);

  // Setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 10);
  display.println("LoRa Initializing OK!");
  display.display();

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relay OFF initially
}

void loop() {
  // Check for received packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    connected = true; // Mark as connected since we received a packet
    lastPacketTime = millis();

    // Read the packet
    LoRaData = "";
    while (LoRa.available()) {
      LoRaData += (char)LoRa.read();
    }

    Serial.println("Received: " + LoRaData);

    // Process received data
    if (LoRaData == "Relay ON") {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH); // Turn relay ON
    } else if (LoRaData == "Relay OFF") {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW); // Turn relay OFF
    }

    // Print RSSI of the packet
    int rssi = LoRa.packetRssi();
    Serial.println("RSSI: " + String(rssi));

    // Update OLED display
    updateDisplay(rssi);
  }

  // Check for connection timeout
  if (millis() - lastPacketTime > connectionTimeout) {
    connected = false; // Mark as disconnected
    updateDisplay(-99); // Update display to show "Disconnected"
  }
}

void updateDisplay(int rssi) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("LORA RECEIVER");

  // Connection status
  display.setCursor(0, 15);
  display.print("Connected: ");
  display.print(connected ? "Yes" : "No");

  // Signal strength
  display.setCursor(0, 30);
  display.print("Signal: ");
  if (connected) {
    display.print(rssi);
  } else {
    display.print("N/A");
  }

  // Relay state
  display.setCursor(0, 45);
  display.print("Relay: ");
  display.print(relayState ? "ON" : "OFF");

  display.display();
}
