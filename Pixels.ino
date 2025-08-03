// Pixels.ino - NeoPixel LED animation functions

// Animation variables
unsigned long lastPixelUpdate = 0;
uint8_t pixelHue = 0;
uint8_t fireAnimation[NEOPIXEL_COUNT];

// Forward declarations for animations
void breatheAnimation(uint8_t r, uint8_t g, uint8_t b);
void rainbowCycle();
void scannerAnimation(uint8_t r, uint8_t g, uint8_t b);
void sparkleAnimation();
void fireEffect();
void oceanWave();
void matrixRain();
void pulseAnimation(uint8_t r, uint8_t g, uint8_t b);
void chaosAnimation();
void lightningAnimation();

void updatePixels() {
  if (millis() - lastPixelUpdate < 20) return;
  lastPixelUpdate = millis();
  
  switch(pixelMode) {
    case PIXEL_OFF:
      pixels.clear();
      break;
      
    case PIXEL_IDLE:
      switch(idlePattern) {
        case PATTERN_BREATHE:
          breatheAnimation(0, 0, 255);
          break;
        case PATTERN_RAINBOW:
          rainbowCycle();
          break;
        case PATTERN_SCANNER:
          scannerAnimation(0, 0, 255);
          break;
        case PATTERN_SPARKLE:
          sparkleAnimation();
          break;
        case PATTERN_FIRE:
          fireEffect();
          break;
        case PATTERN_OCEAN:
          oceanWave();
          break;
        case PATTERN_MATRIX:
          matrixRain();
          break;
        case PATTERN_OFF:
          pixels.clear();
          break;
      }
      break;
      
    case PIXEL_RX:
      scannerAnimation(0, 255, 0); // Green scanner for RX
      break;
      
    case PIXEL_TX:
      pulseAnimation(255, 0, 0); // Red pulse for TX
      break;
      
    case PIXEL_JAMMER:
      chaosAnimation(); // Random chaos for jamming
      break;
      
    case PIXEL_TESLA:
      lightningAnimation(); // Lightning effect for Tesla
      break;
      
    case PIXEL_MENU:
      rainbowCycle(); // Rainbow effect in menus
      break;
  }
  
  pixels.show();
}

void breatheAnimation(uint8_t r, uint8_t g, uint8_t b) {
  static uint8_t breatheStep = 0;
  static int8_t breatheDir = 1;
  
  breatheStep += breatheDir * 2;
  if(breatheStep >= 100 || breatheStep <= 10) {
    breatheDir = -breatheDir;
  }
  
  uint8_t brightness = breatheStep;
  uint32_t color = pixels.Color((r * brightness) / 100, 
                                (g * brightness) / 100, 
                                (b * brightness) / 100);
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, color);
  }
  
  if(breatheStep % 20 == 0) {
    Serial.printf("[LED] Breathe: %d%%\n", brightness);
  }
}

void scannerAnimation(uint8_t r, uint8_t g, uint8_t b) {
  static int scanPos = 0;
  static int scanDir = 1;
  static unsigned long lastScanLog = 0;
  
  pixels.clear();
  
  // Main scanner dot
  pixels.setPixelColor(scanPos, pixels.Color(r, g, b));
  
  // Trail effect
  for(int i = 1; i <= 4; i++) {
    int trailPos = scanPos - (i * scanDir);
    if(trailPos >= 0 && trailPos < NEOPIXEL_COUNT) {
      uint8_t intensity = 255 - (i * 50);
      pixels.setPixelColor(trailPos, pixels.Color((r * intensity) / 255,
                                                  (g * intensity) / 255,
                                                  (b * intensity) / 255));
    }
  }
  
  scanPos += scanDir;
  if(scanPos >= NEOPIXEL_COUNT - 1 || scanPos <= 0) {
    scanDir = -scanDir;
    
    if(millis() - lastScanLog > 5000) {
      lastScanLog = millis();
      Serial.printf("[LED] Scanner: pos=%d, dir=%d\n", scanPos, scanDir);
    }
  }
}

void pulseAnimation(uint8_t r, uint8_t g, uint8_t b) {
  static uint8_t pulsePhase = 0;
  static unsigned long lastPulseLog = 0;
  
  pulsePhase += 5;
  
  uint8_t intensity = (sin(pulsePhase * 0.0174533) + 1) * 127;
  uint32_t color = pixels.Color((r * intensity) / 255,
                                (g * intensity) / 255,
                                (b * intensity) / 255);
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, color);
  }
  
  if(millis() - lastPulseLog > 2000) {
    lastPulseLog = millis();
    Serial.printf("[LED] Pulse: intensity=%d\n", intensity);
  }
}

void chaosAnimation() {
  static unsigned long lastChaosLog = 0;
  static int chaosCount = 0;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    if(random(10) < 3) {
      pixels.setPixelColor(i, pixels.Color(random(256), random(256), random(256)));
      chaosCount++;
    } else {
      pixels.setPixelColor(i, 0);
    }
  }
  
  if(millis() - lastChaosLog > 3000) {
    lastChaosLog = millis();
    Serial.printf("[LED] Chaos: %d pixels lit\n", chaosCount);
    chaosCount = 0;
  }
}

void lightningAnimation() {
  static unsigned long lastStrike = 0;
  static int strikeCount = 0;
  
  if(millis() - lastStrike > random(100, 500)) {
    lastStrike = millis();
    strikeCount++;
    
    Serial.printf("[LED] Lightning strike #%d\n", strikeCount);
    
    // Lightning strike
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
    pixels.show();
    delay(50);
    
    // Fade out
    for(int fade = 255; fade > 0; fade -= 15) {
      for(int i = 0; i < NEOPIXEL_COUNT; i++) {
        pixels.setPixelColor(i, pixels.Color(fade, fade, fade * 0.8));
      }
      pixels.show();
      delay(10);
    }
  } else {
    // Ambient electrical blue
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      if(random(100) < 5) {
        pixels.setPixelColor(i, pixels.Color(0, 0, random(50, 150)));
      } else {
        pixels.setPixelColor(i, 0);
      }
    }
  }
}

void sparkleAnimation() {
  static unsigned long lastSparkleLog = 0;
  
  pixels.clear();
  
  int sparkleCount = 5;
  for(int i = 0; i < sparkleCount; i++) {
    int pos = random(NEOPIXEL_COUNT);
    pixels.setPixelColor(pos, pixels.Color(255, 255, 255));
  }
  
  if(millis() - lastSparkleLog > 5000) {
    lastSparkleLog = millis();
    Serial.println(F("[LED] Sparkle animation active"));
  }
}

void fireEffect() {
  static unsigned long lastFireLog = 0;
  
  // Cool down every cell a little
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    int cooling = random(0, 3);
    int newHeat = fireAnimation[i] - cooling;
    fireAnimation[i] = (newHeat < 0) ? 0 : newHeat;
  }
  
  // Heat from each cell drifts up
  for(int k = NEOPIXEL_COUNT - 1; k >= 2; k--) {
    fireAnimation[k] = (fireAnimation[k - 1] + fireAnimation[k - 2] + fireAnimation[k - 2]) / 3;
  }
  
  // Randomly ignite new sparks
  if(random(255) < 120) {
    int y = random(7);
    int newHeat = fireAnimation[y] + random(160, 255);
    fireAnimation[y] = (newHeat > 255) ? 255 : newHeat;
  }
  
  // Convert heat to LED colors
  for(int j = 0; j < NEOPIXEL_COUNT; j++) {
    uint8_t temperature = fireAnimation[j];
    uint8_t r = (temperature * 2 > 255) ? 255 : temperature * 2;
    uint8_t g = temperature > 127 ? 255 : temperature * 2;
    uint8_t b = temperature > 200 ? 200 : 0;
    pixels.setPixelColor(j, pixels.Color(r, g, b));
  }
  
  if(millis() - lastFireLog > 5000) {
    lastFireLog = millis();
    int avgHeat = 0;
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      avgHeat += fireAnimation[i];
    }
    avgHeat /= NEOPIXEL_COUNT;
    Serial.printf("[LED] Fire: avg heat=%d\n", avgHeat);
  }
}

void oceanWave() {
  static uint16_t wavePhase = 0;
  static unsigned long lastWaveLog = 0;
  
  wavePhase += 2;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint8_t blue = 64 + sin((i + wavePhase) * 0.1) * 63;
    uint8_t green = 32 + sin((i + wavePhase) * 0.1) * 31;
    pixels.setPixelColor(i, pixels.Color(0, green, blue));
  }
  
  if(millis() - lastWaveLog > 5000) {
    lastWaveLog = millis();
    Serial.printf("[LED] Ocean wave: phase=%d\n", wavePhase);
  }
}

void matrixRain() {
  static uint8_t drops[NEOPIXEL_COUNT];
  static unsigned long lastMatrixLog = 0;
  static int dropCount = 0;
  
  // Fade all pixels
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint32_t color = pixels.getPixelColor(i);
    uint8_t g = (color >> 8) & 0xFF;
    if(g > 10) {
      g -= 10;
    } else {
      g = 0;
    }
    pixels.setPixelColor(i, pixels.Color(0, g, 0));
  }
  
  // Add new drops
  if(random(10) < 3) {
    int pos = random(NEOPIXEL_COUNT);
    pixels.setPixelColor(pos, pixels.Color(0, 255, 0));
    dropCount++;
  }
  
  if(millis() - lastMatrixLog > 5000) {
    lastMatrixLog = millis();
    Serial.printf("[LED] Matrix rain: %d drops\n", dropCount);
    dropCount = 0;
  }
}

void rainbowCycle() {
  static unsigned long lastRainbowLog = 0;
  
  pixelHue += 256;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    int pixelHueOffset = (i * 65536L / NEOPIXEL_COUNT) + pixelHue;
    pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHueOffset)));
  }
  
  if(millis() - lastRainbowLog > 10000) {
    lastRainbowLog = millis();
    Serial.printf("[LED] Rainbow: hue=%d\n", pixelHue / 256);
  }
}

// Special effect functions
void successAnimation() {
  Serial.println(F("[LED] Success animation"));
  
  // Green wave
  for(int wave = 0; wave < 3; wave++) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.clear();
      for(int j = 0; j < 5; j++) {
        int pos = (i + j) % NEOPIXEL_COUNT;
        int brightness = 255 - (j * 50);
        pixels.setPixelColor(pos, pixels.Color(0, brightness, 0));
      }
      pixels.show();
      delay(30);
    }
  }
  pixels.clear();
  pixels.show();
}

void errorAnimation() {
  Serial.println(F("[LED] Error animation"));
  
  // Red flash
  for(int flash = 0; flash < 5; flash++) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
    pixels.show();
    delay(100);
    pixels.clear();
    pixels.show();
    delay(100);
  }
}

void bootAnimation() {
  Serial.println(F("[LED] Boot animation"));
  
  // Fill up with white
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();
    delay(30);
  }
  
  // Fade to blue
  for(int fade = 255; fade > 0; fade -= 5) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(fade * 0.2, fade * 0.2, fade));
    }
    pixels.show();
    delay(10);
  }
  
  pixels.clear();
  pixels.show();
}

void signalStrengthLEDs(int rssi) {
  // Map RSSI to number of LEDs
  int ledsToLight = map(rssi, -120, -30, 0, NEOPIXEL_COUNT);
  ledsToLight = constrain(ledsToLight, 0, NEOPIXEL_COUNT);
  
  pixels.clear();
  
  for(int i = 0; i < ledsToLight; i++) {
    uint32_t color;
    if(i < NEOPIXEL_COUNT * 0.3) {
      color = pixels.Color(255, 0, 0); // Red for weak
    } else if(i < NEOPIXEL_COUNT * 0.7) {
      color = pixels.Color(255, 255, 0); // Yellow for medium
    } else {
      color = pixels.Color(0, 255, 0); // Green for strong
    }
    pixels.setPixelColor(i, color);
  }
  
  pixels.show();
}

void teslaSuccessAnimation() {
  Serial.println(F("[LED] Tesla success animation"));
  
  // Electric blue flash pattern
  for(int flash = 0; flash < 5; flash++) {
    // Bright flash
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 255));
    }
    pixels.show();
    delay(50);
    
    // Dim
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 50));
    }
    pixels.show();
    delay(50);
  }
  
  // Final ripple effect
  for(int r = 0; r < NEOPIXEL_COUNT/2; r++) {
    pixels.clear();
    pixels.setPixelColor(NEOPIXEL_COUNT/2 + r, pixels.Color(0, 0, 255));
    pixels.setPixelColor(NEOPIXEL_COUNT/2 - r, pixels.Color(0, 0, 255));
    if(r > 0) {
      pixels.setPixelColor(NEOPIXEL_COUNT/2 + r - 1, pixels.Color(0, 0, 128));
      pixels.setPixelColor(NEOPIXEL_COUNT/2 - r + 1, pixels.Color(0, 0, 128));
    }
    pixels.show();
    delay(30);
  }
  
  pixels.clear();
  pixels.show();
}