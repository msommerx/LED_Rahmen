#include <Adafruit_NeoPixel.h>

#define LED_PIN        14   // WS2812 LEDs an GPIO 14
#define BRIGHTNESS_PIN A0   // Poti für Helligkeit
#define COLOR_BUTTON   2    // Button zum Wechseln der Farben
#define MODE_BUTTON    0    // Button zum Aktivieren des Farbverlaufs
#define NUMPIXELS      50   // Anzahl der LEDs
#define DELAYVAL       40   // Wartezeit für sanftere Effekte (ms)

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int colorIndex = 0;       // Aktuelle Farbe
bool colorButtonState = false;
bool lastColorButtonState = false;
bool rainbowMode = false; // Farbverlauf an/aus
bool modeButtonState = false;
bool lastModeButtonState = false;

float rainbowSpeed = 0.2;  // Geschwindigkeit des Farbverlaufs (kann <1 sein)
float hueCounter = 0;      // Zähler für sanfte Hue-Änderung
int rainbowHue = 0;        // Farbwert (0-255)

// 6 feste Farben (RGB)
uint32_t colors[] = {
  pixels.Color(200, 0, 0),   // Rot
  pixels.Color(0, 200, 0),   // Grün
  pixels.Color(0, 0, 200),   // Blau
  pixels.Color(200, 200, 0), // Gelb
  pixels.Color(0, 200, 200), // Cyan
  pixels.Color(200, 0, 200)  // Magenta
};

void setup() {
  pixels.begin();
  pinMode(COLOR_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
}

void loop() {
  // Helligkeit vom Poti auslesen & setzen
  int brightnessVal = analogRead(BRIGHTNESS_PIN);
  int brightness = map(brightnessVal, 0, 1023, 5, 255); // Min. Helligkeit 5, damit LEDs nicht komplett aus sind
  pixels.setBrightness(brightness);

  // Button zum Wechseln der Farbe mit Debounce
  int readingColor = digitalRead(COLOR_BUTTON);
  if (readingColor != lastColorButtonState) {
    delay(50); // Kurzes Delay für Debounce
    if (readingColor == LOW) { 
      colorIndex = (colorIndex + 1) % 6;  
    }
  }
  lastColorButtonState = readingColor;

  // Button für den Farbverlauf-Modus
  int readingMode = digitalRead(MODE_BUTTON);
  if (readingMode != lastModeButtonState) {
    delay(50);
    if (readingMode == LOW) { 
      rainbowMode = !rainbowMode; // Farbverlauf an/aus
    }
  }
  lastModeButtonState = readingMode;

  // Farbverlauf aktivieren oder feste Farbe setzen
  if (rainbowMode) {
    hueCounter += rainbowSpeed;
    if (hueCounter >= 1.0) { 
      rainbowHue = (rainbowHue + 1) % 256;
      hueCounter -= 1.0;
    }
    uint32_t color = pixels.ColorHSV(rainbowHue * 256); // HSV in RGB umwandeln
    pixels.fill(color, 0, NUMPIXELS);
  } else {
    pixels.fill(colors[colorIndex], 0, NUMPIXELS);
  }

  pixels.show();
  delay(DELAYVAL);
}
