#include <Wire.h>
#include <TFT_eSPI.h>
#include <Adafruit_PN532.h>
#include <BlynkSimpleEsp8266.h>

// Initialize TFT display
TFT_eSPI tft;

// Initialize RFID reader
#define PN532_SDA 21
#define PN532_SCL 22

Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);

// Variables
int sourceStation = 0;
int destinationStation = 0;
int fare = 0;
int cardBalance = 100; // Initial balance on the card

char auth[] = ""; // Replace with your Blynk authentication token
char ssid[] = "";       // Replace with your Wi-Fi SSID
char pass[] = "";   // Replace with your Wi-Fi password

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize TFT display
  tft.begin();
  tft.setRotation(3); // Set display rotation to match your setup
  tft.fillScreen(TFT_BLACK);

  // Initialize RFID reader
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();

  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1); // Halt the program
  }

  // Configure board settings
  nfc.SAMConfig();

  // Connect to Wi-Fi
  Blynk.begin(auth, ssid, pass);

  // Draw initial screen
  drawMenu();
}

void loop() {
  // Check for RFID card presence
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Read the card ID and perform necessary operations
    // Map the card ID to a user and retrieve fare calculation parameters

    // Example: Wait for user input and update source station variable
    while (true) {
      if (getTouchInput() < 50) { // Touch input for station selection
        sourceStation = readStationMenu();
        break;
      }
    }

    // Example: Wait for user input and update destination station variable
    while (true) {
      if (getTouchInput() < 50) { // Touch input for station selection
        destinationStation = readStationMenu();
        break;
      }
    }

    // Calculate fare
    fare = calculateFare(sourceStation, destinationStation);

    // Deduct fare from card balance
    cardBalance -= fare;

    // Display fare and updated balance on TFT display
    drawFareScreen(fare, cardBalance);

    // Display remaining amount on Blynk Value Display widget
    Blynk.virtualWrite(V1, cardBalance);

    // Print start and destination station status on Serial Monitor
    Serial.print("Start Station: ");
    Serial.println(sourceStation);
    Serial.print("Destination Station: ");
    Serial.println(destinationStation);
  }

  // Run Blynk
  Blynk.run();

  // Additional code for handling user input, menu selection, etc.
}

// Function to read station menu and return selected station
int readStationMenu() {
  // Example: Read touch input and return selected station
  // You can modify this function to use other input methods such as buttons, rotary encoder, etc.
  int station = 0;

  // Read touch input and map it to the corresponding station number
  int touchValue = getTouchInput();
  if (touchValue < 300) {
    station = 1;
  } else if (touchValue < 600) {
    station = 2;
  } else if (touchValue < 900) {
    station = 3;
  } else if (touchValue < 1200) {
    station = 4;
  } else if (touchValue < 1500) {
    station = 5;
  } else {
    station = 6;
  }

  return station;
}

// Function to calculate fare based on source and destination stations
int calculateFare(int source, int destination) {
  int fare = round((destination - source) / 2.0) * 5;
  return fare;
}

// Function to draw the station menu on the TFT display
void drawMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.println("Select Source Station:");
  tft.setCursor(20, 50);
  tft.println("1. Station A");
  tft.setCursor(20, 80);
  tft.println("2. Station B");
  tft.setCursor(20, 110);
  tft.println("3. Station C");
  tft.setCursor(20, 140);
  tft.println("4. Station D");
  tft.setCursor(20, 170);
  tft.println("5. Station E");
  tft.setCursor(20, 200);
  tft.println("6. Station F");
}

// Function to draw fare and updated balance on TFT display
void drawFareScreen(int fare, int balance) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.println("Fare: " + String(fare));
  tft.setCursor(20, 50);
  tft.println("Updated Balance: " + String(balance));
}

// Function to simulate touch input using analogRead()
int getTouchInput() {
  int touchValue = analogRead(A0); // Change A0 to the appropriate pin

  // You can map the analog reading range to a desired value range if needed
  // Example: return map(touchValue, 0, 1023, 0, 100);

  return touchValue;
}
