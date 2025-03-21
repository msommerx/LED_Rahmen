#include <Adafruit_NeoPixel.h>

#define LED_PIN        14   // WS2812 LEDs an GPIO 14
#define BRIGHTNESS_BTN 4    // Taster für Helligkeit (GPIO 4)
#define COLOR_BUTTON   2    // Taster für Farbwechsel
#define MODE_BUTTON    0    // Taster für Rainbow-Modus
#define NUMPIXELS      52   // Anzahl der LEDs
#define DELAYVAL       50   // Verzögerung für Effekte (ms)
#define RAINBOW_SPEED  64   // Geschwindigkeit des Rainbow-Modus (kleinere Werte = langsamer)

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// 🌟 Helligkeitsstufen (5% und 80%)
int brightnessLevels[] = {13, 204}; 
int brightnessIndex = 1; // Start mit 80%

// 🎨 7 Farben für den normalen Modus
uint32_t colors[] = {
  pixels.Color(255, 0, 0),   // 🔴 Rot
  pixels.Color(0, 255, 0),   // 🟢 Grün
  pixels.Color(0, 0, 255),   // 🔵 Blau
  pixels.Color(255, 255, 0), // 🟡 Gelb
  pixels.Color(0, 255, 255), // 🔵 Cyan
  pixels.Color(255, 0, 255), // 🟣 Magenta
  pixels.Color(255, 165, 0)  // 🟠 Orange
};

int colorIndex = 0;      // Aktuelle Farbe
bool rainbowMode = false; // Rainbow-Modus an/aus
int rainbowStep = 0;     // Für den Farbverlauf

// Timer für automatische Abschaltung
bool timerActive = false;
unsigned long timerStart = 0;
const unsigned long shutdownTime = 3600000; // 1 Stunde (60 * 60 * 1000 ms)

// Debounce-Variablen
bool lastBrightnessState = HIGH;
bool lastColorState = HIGH;
bool lastModeState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 150;  // Entprellzeit

void setup() {
  pixels.begin();
  pinMode(COLOR_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESS_BTN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();

  // 🌟 Helligkeit per Taster umschalten
  bool readingBrightness = digitalRead(BRIGHTNESS_BTN);
  if (readingBrightness == LOW && lastBrightnessState == HIGH && (currentMillis - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = currentMillis;
    brightnessIndex = (brightnessIndex + 1) % 2; // Wechsel zwischen 5% und 80%
    
    if (brightnessIndex == 0) {
      timerStart = millis();
      timerActive = true;
      Serial.println("Timer gestartet: LED schaltet in 1 Stunde aus.");
    } else {
      timerActive = false; // Timer deaktivieren, wenn Helligkeit auf 80 % geht
      Serial.println("Helligkeit auf 80%, Timer gestoppt.");
    }
  }
  lastBrightnessState = readingBrightness;

  // Automatische Abschaltung nach 1 Stunde
  if (timerActive && (currentMillis - timerStart >= shutdownTime)) {
    pixels.setBrightness(0);
    pixels.show();
    Serial.println("LED nach 1 Stunde ausgeschaltet.");
    timerActive = false; // Timer stoppen, um mehrfaches Ausschalten zu verhindern
    return;
  }

  pixels.setBrightness(brightnessLevels[brightnessIndex]);

  // 🎨 Farbwechsel per Taster
  bool readingColor = digitalRead(COLOR_BUTTON);
  if (readingColor == LOW && lastColorState == HIGH && (currentMillis - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = currentMillis;
    colorIndex = (colorIndex + 1) % 7;  // 7 Farben
    Serial.print("Neue Farbe: ");
    Serial.println(colorIndex);
  }
  lastColorState = readingColor;

  // 🌈 Rainbow-Modus ein/aus
  bool readingMode = digitalRead(MODE_BUTTON);
  if (readingMode == LOW && lastModeState == HIGH && (currentMillis - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = currentMillis;
    rainbowMode = !rainbowMode;
    Serial.print("Rainbow Mode: ");
    Serial.println(rainbowMode);
  }
  lastModeState = readingMode;

  // 🌈 Rainbow oder feste Farbe setzen
  if (rainbowMode) {
    for (int i = 0; i < NUMPIXELS; i++) {
      int hue = (rainbowStep + (i * 256 / NUMPIXELS)) % 65536;
      pixels.setPixelColor(i, pixels.ColorHSV(hue));
    }
    rainbowStep = (rainbowStep + RAINBOW_SPEED) % 65536; // 🌈 Langsamerer Farbverlauf
  } else {
    pixels.fill(colors[colorIndex], 0, NUMPIXELS);
  }

  pixels.show();
  delay(DELAYVAL);
}
