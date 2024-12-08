/********* 
  Modified by Henrik
  Based on code from Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
    Lora ttgo-lora32-oled V1,Sender,prg btn toggles relay on reciver
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

// Button pin
#define PRG_BUTTON 0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Relay state
bool relayState = false;

// Button state
bool buttonPressed = false;

void setup() {
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
  display.print("LORA SENDER ");
  display.display();

  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("LoRa Sender Test");

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
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);

  // Initialize button pin
  pinMode(PRG_BUTTON, INPUT_PULLUP);
}

void loop() {
  // Read button state
  bool currentButtonState = digitalRead(PRG_BUTTON) == LOW;
  if (currentButtonState && !buttonPressed) { // Button pressed
    buttonPressed = true;

    // Toggle relay state
    relayState = !relayState;

    // Send toggle command to receiver
    String command = relayState ? "Relay ON" : "Relay OFF";
    LoRa.beginPacket();
    LoRa.print(command);
    LoRa.endPacket();
    
    Serial.println(command);
  } else if (!currentButtonState) {
    buttonPressed = false; // Reset button state
  }

  // Display status on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Lora-Send UP:");
  display.print(LoRa.beginPacket() ? "Yes" : "No");
  // Display Connection Status
  display.setCursor(0, 20);
   display.print("Signal: ");
  display.print(LoRa.packetRssi());

  // Display Signal Strength
  display.setCursor(0, 40);
  display.print("Button: ");
  display.print(currentButtonState ? "Yes" : "No");
  
  // Display Button State
  display.setCursor(0, 50);
  display.print("Relay: ");
  display.print(relayState ? "ON" : "OFF");

  display.display();

  // Delay for readability
  delay(1000);
}
