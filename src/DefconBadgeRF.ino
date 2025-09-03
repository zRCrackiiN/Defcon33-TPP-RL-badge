// DC33 (DEF CON 33)
// The Pirates' Plunder / Rabbit Labs
// TPP Badge

// Badge designed by TehRabbitt 
// https://rabbit-labs.com/

// Firmware by RocketGod
// https://betaskynet.com
// https://discord.gg/thepirates

#include "Globals.h"

// Global object definitions
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Global variable definitions
MenuState currentMenu = MENU_MAIN;
MenuState previousMenu = MENU_MAIN;
int menuSelection = 0;
int menuOffset = 0;
const int maxMenuItems = 4;

IdlePattern idlePattern = PATTERN_BREATHE;
uint8_t ledBrightness = 50;
PixelMode pixelMode = PIXEL_IDLE;

// LED Pattern preview state
IdlePattern previewPattern = PATTERN_BREATHE;
IdlePattern savedPattern = PATTERN_BREATHE;
bool inPatternPreview = false;

String raw_rx = "0";
String jammer_tx = "0";
unsigned long sample[SAMPLE_SIZE];
int samplecount = 0;
float frequency = 433.92;
float teslaFrequency = 433.92;
int mod = 2;
float deviation = 47.60;
float setrxbw = 812.50;
int datarate = 99;
byte activeModule = 0;
bool sdCardPresent = false;
bool cc1101APresent = false;
bool cc1101BPresent = false;

unsigned long lastButtonPress = 0;
const unsigned long buttonDebounce = 200;

// Menu navigation
int frequencyIndex = 15; // Default to 433.92

// Tesla configuration  
int teslaTransmissions = 5;
int teslaDelay = 23;

// Jammer configuration
int jammerPower = 12;
bool jammerSweep = false;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Initialize hardware
  initializeHardware();
  
  // Start normal operation
  pixelMode = PIXEL_MENU;
  updateDisplay();
}

void loop() {
  handleButtons();
  updatePixels();
  
  if(raw_rx == "1") {
    handleRXMode();
  }
  
  if(jammer_tx == "1") {
    runJammer();
  }
}

void debugPrint(String message, bool serial, bool screen, int displayTime) {
  if (serial) {
    Serial.println(message);
  }
  if (screen && display.width() > 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(F("=[ DEBUG ]="));
    display.println(F(""));
    
    // Word wrap for long messages
    int lineLength = 20;
    for (int i = 0; i < message.length(); i += lineLength) {
      display.println(message.substring(i, min(i + lineLength, (int)message.length())));
    }
    
    display.display();
    delay(displayTime);
  }
}

//==============================================================================================
// DISPLAY.INO
// Display.ino - All display drawing and UI functions
//==============================================================================================

void updateDisplay() {
  display.clearDisplay();
  
  Serial.printf("[Display] Updating display for menu %d\n", currentMenu);
  
  switch(currentMenu) {
    case MENU_MAIN:
      drawMainMenu();
      break;
    case MENU_RX:
      drawRXMenu();
      break;
    case MENU_TX:
      drawTXMenu();
      break;
    case MENU_JAMMER:
      drawJammerMenu();
      break;
    case MENU_TESLA:
      drawTeslaMenu();
      break;
    case MENU_SETTINGS:
      drawSettingsMenu();
      break;
    case MENU_ABOUT:
      drawAbout();
      break;
    case MENU_CREDITS:
      drawCredits();
      break;
    case MENU_RX_CONFIG:
      drawRXConfig();
      break;
    case MENU_TX_CONFIG:
      drawTXConfig();
      break;
    case MENU_DISPLAY_SETTINGS:
      drawDisplaySettings();
      break;
    case MENU_LED_PATTERN:
      drawLEDPatternMenu();
      break;
    case MENU_LED_BRIGHTNESS:
      drawBrightnessMenu();
      break;
    case MENU_FREQUENCY_SELECT:
      drawFrequencySelect();
      break;
    case MENU_TESLA_CONFIG:
      drawTeslaConfig();
      break;
  }
  
  display.display();
}

void drawMainMenu() {
  Serial.println(F("[Display] Drawing main menu"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ TPP BADGE ]="));
  
  // Show hardware status on main menu
  display.setCursor(90, 0);
  display.print(F("HW:"));
  if(cc1101APresent || cc1101BPresent) display.print(F("R"));
  if(sdCardPresent) display.print(F("S"));
  
  display.setCursor(0, 8);
  display.println(F(""));
  
  const char* menuItems[] = {
    "RX Mode",
    "TX Mode", 
    "Jammer",
    "Tesla",
    "Settings",
    "About",
    "Credits"
  };
  
  int itemCount = 7;
  drawMenuItems(menuItems, itemCount);
}

void drawRXMenu() {
  Serial.println(F("[Display] Drawing RX menu"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ RX MODE ]="));
  display.println(F(""));
  
  const char* menuItems[] = {
    "Start RX",     
    "Configure",    
    "Stop RX",
    "View Logs"
  };
  
  int itemCount = 4;
  drawMenuItems(menuItems, itemCount);
  
  if(raw_rx == "1") {
    display.setCursor(80, 0);
    display.print(F("[RX]"));
  }
}

void drawTXMenu() {
  Serial.println(F("[Display] Drawing TX menu"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ TX MODE ]="));
  display.println(F(""));
  
  const char* menuItems[] = {
    "Configure",
    "Binary TX",
    "TX from File",
    "Last RX"
  };
  
  int itemCount = 4;
  drawMenuItems(menuItems, itemCount);
}

void drawJammerMenu() {
  Serial.println(F("[Display] Drawing Jammer menu"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ JAMMER ]="));
  display.println(F(""));
  
  const char* menuItems[] = {
    "Start Jammer",
    "Stop Jammer",
    "Configure"
  };
  
  int itemCount = 3;
  drawMenuItems(menuItems, itemCount);
  
  if(jammer_tx == "1") {
    display.setCursor(80, 0);
    display.print(F("[JAM]"));
  }
}

void drawTeslaMenu() {
  Serial.println(F("[Display] Drawing Tesla menu"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ TESLA ]="));
  display.println(F(""));
  
  char freqStr[20];
  sprintf(freqStr, "Freq: %.2f MHz", teslaFrequency);
  
  const char* menuItems[] = {
    "Send Signal",
    freqStr
  };
  
  int itemCount = 2;
  drawMenuItems(menuItems, itemCount);
  
  display.setCursor(0, 55);
  display.print(F("SELECT toggles freq"));
}

void drawTeslaConfig() {
  Serial.println(F("[Display] Drawing Tesla config"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ TESLA FREQ ]="));
  display.println(F(""));
  
  display.setCursor(10, 20);
  display.print(F("Current: "));
  display.print(teslaFrequency, 2);
  display.println(F(" MHz"));
  
  display.setCursor(10, 35);
  display.println(F("Press SELECT to"));
  display.setCursor(10, 45);
  display.println(F("toggle 315/433.92"));
}

void drawSettingsMenu() {
  Serial.println(F("[Display] Drawing Settings menu"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ SETTINGS ]="));
  display.println(F(""));
  
  char moduleStr[20];
  if(!cc1101APresent && !cc1101BPresent) {
    sprintf(moduleStr, "Module: NONE");
  } else if(cc1101APresent && !cc1101BPresent) {
    sprintf(moduleStr, "Module: A only");
  } else if(!cc1101APresent && cc1101BPresent) {
    sprintf(moduleStr, "Module: B only");
  } else {
    sprintf(moduleStr, "Module: %c", activeModule == 0 ? 'A' : 'B');
  }
  
  char freqStr[20];
  sprintf(freqStr, "Freq: %.2f", frequency);
  
  const char* modStr;
  switch(mod) {
    case 0: modStr = "Mod: 2-FSK"; break;
    case 1: modStr = "Mod: GFSK"; break;
    case 2: modStr = "Mod: ASK/OOK"; break;
    case 3: modStr = "Mod: 4-FSK"; break;
    case 4: modStr = "Mod: MSK"; break;
    default: modStr = "Mod: Unknown"; break;
  }
  
  const char* menuItems[] = {
    moduleStr,
    freqStr,
    modStr,
    "Display Settings",
    "Save Config"
  };
  
  int itemCount = 5;
  drawMenuItems(menuItems, itemCount);
}

void drawDisplaySettings() {
  Serial.println(F("[Display] Drawing Display Settings"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ DISPLAY ]="));
  display.println(F(""));
  
  const char* menuItems[] = {
    "LED Pattern",
    "LED Brightness",
    "Back"
  };
  
  int itemCount = 3;
  drawMenuItems(menuItems, itemCount);
}

void drawLEDPatternMenu() {
  Serial.printf("[Display] Drawing LED Pattern menu, selection: %d\n", menuSelection);
  
  if (!inPatternPreview) {
    savedPattern = idlePattern;
    inPatternPreview = true;
  }
  
  previewPattern = (IdlePattern)menuSelection;
  idlePattern = previewPattern;
  pixelMode = PIXEL_IDLE;
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ LED PATTERN ]="));
  display.println(F(""));
  
  const char* menuItems[] = {
    "Breathe",
    "Rainbow",
    "Scanner",
    "Sparkle",
    "Fire",
    "Ocean", 
    "Matrix",
    "SuperGay",
    "Red",
    "Green",
    "Blue",
    "White",
    "Magenta",
    "Cyan",
    "Yellow",
    "Orange",
    "Neon Pulse",
    "Plasma",
    "Meteor Rain",
    "Twinkle",
    "Vaporwave",
    "Police",
    "Pirate Ship",
    "Theater Chase",
    "Running Lights",
    "Bouncing Balls",
    "Heartbeat",
    "Strobe",
    "Cylon",
    "Snow Sparkle",
    "Pacifica",
    "Lava Lamp",
    "Lightning",
    "Confetti",
    "Off"
  };
  
  int itemCount = 35;
  drawMenuItems(menuItems, itemCount);
  
  display.setCursor(0, 56);
  display.setTextSize(1);
  display.print(F("Now: "));
  if(menuSelection < itemCount) {
    display.print(menuItems[menuSelection]);
  }
}

// Add this function to handle exiting the pattern menu
void exitPatternMenu(bool save) {
  if (inPatternPreview) {
    if (save) {
      // Keep the previewed pattern
      Serial.printf("[Display] Saving pattern: %d\n", previewPattern);
    } else {
      // Restore the original pattern
      idlePattern = savedPattern;
      Serial.printf("[Display] Restoring pattern: %d\n", savedPattern);
    }
    inPatternPreview = false;
  }
}

void drawBrightnessMenu() {
  Serial.printf("[Display] Drawing brightness menu, current: %d\n", ledBrightness);
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ BRIGHTNESS ]="));
  display.println(F(""));
  
  display.setCursor(10, 20);
  display.print(F("Level: "));
  display.print(ledBrightness);
  display.println(F("/255"));
  
  // Draw brightness bar
  int barWidth = map(ledBrightness, 0, 255, 0, 100);
  display.drawRect(10, 35, 102, 10, SH110X_WHITE);
  display.fillRect(11, 36, barWidth, 8, SH110X_WHITE);
  
  display.setCursor(10, 50);
  display.print(F("UP/DOWN to adjust"));
}

void drawFrequencySelect() {
  Serial.printf("[Display] Drawing frequency select, current: %.2f MHz, index: %d\n", 
                frequency, frequencyIndex);
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ FREQUENCY ]="));
  display.println(F(""));
  
  // Show current frequency
  display.setCursor(0, 16);
  display.print(F("Current: "));
  display.print(frequency, 2);
  display.println(F(" MHz"));
  
  // Show selected frequency
  display.setCursor(0, 28);
  display.print(F("> "));
  display.print(commonFrequencies[frequencyIndex], 2);
  display.println(F(" MHz"));
  
  // Show band info
  display.setCursor(0, 45);
  if(commonFrequencies[frequencyIndex] < 350) {
    display.print(F("Band: 300 MHz"));
  } else if(commonFrequencies[frequencyIndex] < 500) {
    display.print(F("Band: 433 MHz"));
  } else if(commonFrequencies[frequencyIndex] < 900) {
    display.print(F("Band: 868 MHz"));
  } else {
    display.print(F("Band: 915 MHz"));
  }
  
  display.setCursor(0, 55);
  display.print(F("UP/DOWN to change"));
}

void drawRXConfig() {
  Serial.println(F("[Display] Drawing RX config"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ RX CONFIG ]="));
  display.println(F(""));
  
  char buffer[20];
  sprintf(buffer, "Freq: %.2f MHz", frequency);
  
  const char* menuItems[] = {
    buffer,
    "BW: 812.50 kHz",
    "Rate: 99.97 kBaud",
    "Back"
  };
  
  int itemCount = 4;
  drawMenuItems(menuItems, itemCount);
}

void drawTXConfig() {
  Serial.println(F("[Display] Drawing TX config"));
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ TX CONFIG ]="));
  display.println(F(""));
  
  char buffer[20];
  sprintf(buffer, "Freq: %.2f MHz", frequency);
  
  const char* menuItems[] = {
    buffer,
    "Dev: 47.60 kHz",
    "Rate: 99.97 kBaud",
    "Back"
  };
  
  int itemCount = 4;
  drawMenuItems(menuItems, itemCount);
}

void drawMenuItems(const char* items[], int itemCount) {
  int startItem = menuOffset;
  int endItem = min(startItem + maxMenuItems, itemCount);
  
  Serial.printf("[Display] Drawing menu items %d to %d, selection=%d\n", 
                startItem, endItem-1, menuSelection);
  
  for (int i = startItem; i < endItem; i++) {
    int yPos = 16 + ((i - startItem) * 10);
    
    if (i == menuSelection) {
      display.fillRect(0, yPos - 1, 128, 10, SH110X_WHITE);
      display.setTextColor(SH110X_BLACK);
    } else {
      display.setTextColor(SH110X_WHITE);
    }
    
    display.setCursor(2, yPos);
    display.print(items[i]);
  }
  
  display.setTextColor(SH110X_WHITE);
  
  // Show scroll indicators if needed
  if (menuOffset > 0) {
    display.setCursor(120, 16);
    display.print(F("^"));
  }
  if (endItem < itemCount) {
    display.setCursor(120, 54);
    display.print(F("v"));
  }
}

//============================================
// FILEOPS.INO

// FileOps.ino - SD card file operations and management

File logs;
String currentLogFile = "";
unsigned long logStartTime = 0;
int logEntryCount = 0;

// Forward declarations
void createDirectory(const char* path);
void startNewLogFile();
void listDirectory(const char* dirname, uint8_t levels);
void viewFiles(const char* path);
void fileOptions(const char* filename);
void viewFileContent(const char* filename);
void deleteFileConfirm(const char* filename);
void deleteFile(fs::FS &fs, const char* path);
void showCardInfo();

void initializeFileSystem() {
  if(!sdCardPresent) {
    Serial.println(F("[FILES] No SD card present"));
    return;
  }
  
  Serial.println(F("\n[FILES] ========== INITIALIZING FILE SYSTEM =========="));
  
  // Create directories if they don't exist
  createDirectory("/logs");
  createDirectory("/rx_data");
  createDirectory("/tx_data");
  createDirectory("/config");
  
  // Start new log file
  startNewLogFile();
  
  // List files
  listDirectory("/", 0);
  
  Serial.println(F("[FILES] File system ready"));
}

void createDirectory(const char* path) {
  if(!SD_MMC.exists(path)) {
    if(SD_MMC.mkdir(path)) {
      Serial.printf("[FILES] Created directory: %s\n", path);
    } else {
      Serial.printf("[FILES] Failed to create directory: %s\n", path);
    }
  } else {
    Serial.printf("[FILES] Directory exists: %s\n", path);
  }
}

void startNewLogFile() {
  char filename[32];
  sprintf(filename, "/logs/log_%lu.txt", millis());
  currentLogFile = String(filename);
  logStartTime = millis();
  logEntryCount = 0;
  
  File logFile = SD_MMC.open(filename, FILE_WRITE);
  if(logFile) {
    logFile.println(F("=== TPP Badge Log ==="));
    logFile.printf("Start time: %lu\n", logStartTime);
    logFile.printf("Frequency: %.2f MHz\n", frequency);
    logFile.printf("Module: %c\n", activeModule == 0 ? 'A' : 'B');
    logFile.println(F("====================="));
    logFile.close();
    
    Serial.printf("[FILES] Started new log: %s\n", filename);
  } else {
    Serial.printf("[FILES] Failed to create log: %s\n", filename);
  }
}

// Remove the default parameter from the implementation
void logEvent(const char* event, const char* details) {
  if(!sdCardPresent || currentLogFile.length() == 0) return;
  
  File logFile = SD_MMC.open(currentLogFile, FILE_APPEND);
  if(logFile) {
    unsigned long timestamp = millis() - logStartTime;
    logFile.printf("[%lu] %s", timestamp, event);
    if(details && strlen(details) > 0) {
      logFile.printf(" - %s", details);
    }
    logFile.println();
    logFile.close();
    
    logEntryCount++;
    
    // Start new log file every 1000 entries
    if(logEntryCount >= 1000) {
      startNewLogFile();
    }
  }
}

void saveRXData(const char* filename) {
  if(!sdCardPresent || samplecount == 0) {
    Serial.println(F("[FILES] Cannot save RX data"));
    return;
  }
  
  Serial.printf("[FILES] Saving RX data to %s\n", filename);
  
  File dataFile = SD_MMC.open(filename, FILE_WRITE);
  if(!dataFile) {
    Serial.println(F("[FILES] Failed to create RX file"));
    debugPrint("Save failed!", true, true, 2000);
    return;
  }
  
  // Write header
  dataFile.println(F("# TPP Badge RX Data"));
  dataFile.printf("# Timestamp: %lu\n", millis());
  dataFile.printf("Frequency: %.2f\n", frequency);
  dataFile.printf("Modulation: %d\n", mod);
  dataFile.printf("Bandwidth: %.2f\n", setrxbw);
  dataFile.printf("DataRate: %d\n", datarate);
  dataFile.printf("RSSI: %d\n", ELECHOUSE_cc1101.getRssi());
  dataFile.printf("Count: %d\n", samplecount);
  dataFile.print("Data: ");
  
  // Write timing data
  for(int i = 0; i < samplecount; i++) {
    dataFile.print(sample[i]);
    if(i < samplecount - 1) dataFile.print(",");
  }
  dataFile.println();
  
  // Write binary interpretation
  dataFile.print("Binary: ");
  for(int i = 0; i < min(samplecount, 100); i++) {
    dataFile.print(sample[i] > 1000 ? "1" : "0");
  }
  dataFile.println();
  
  dataFile.close();
  
  Serial.printf("[FILES] Saved %d samples\n", samplecount);
  logEvent("RX_SAVE", filename);
  
  // Show success
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(F("RX Data Saved!"));
  display.println(filename);
  display.printf("%d samples\n", samplecount);
  display.display();
  delay(1500);
}

void listDirectory(const char* dirname, uint8_t levels) {
  Serial.printf("[FILES] Listing directory: %s\n", dirname);
  
  File root = SD_MMC.open(dirname);
  if(!root) {
    Serial.println(F("[FILES] Failed to open directory"));
    return;
  }
  
  if(!root.isDirectory()) {
    Serial.println(F("[FILES] Not a directory"));
    return;
  }
  
  File file = root.openNextFile();
  while(file) {
    if(file.isDirectory()) {
      Serial.print(F("  DIR : "));
      Serial.println(file.name());
      if(levels) {
        listDirectory(file.name(), levels - 1);
      }
    } else {
      Serial.print(F("  FILE: "));
      Serial.print(file.name());
      Serial.print(F("  SIZE: "));
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void displayFileMenu() {
  Serial.println(F("\n[FILES] ========== FILE MENU =========="));
  
  bool inMenu = true;
  int menuOption = 0;
  const int numOptions = 5;
  
  while(inMenu) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ FILES ]="));
    display.println(F(""));
    
    const char* options[] = {
      "View RX Files",
      "View TX Files", 
      "View Logs",
      "Card Info",
      "Back"
    };
    
    for(int i = 0; i < numOptions; i++) {
      if(i == menuOption) {
        display.fillRect(0, 16 + i*10, 128, 10, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      } else {
        display.setTextColor(SH110X_WHITE);
      }
      
      display.setCursor(2, 17 + i*10);
      display.print(options[i]);
    }
    
    display.setTextColor(SH110X_WHITE);
    display.display();
    
    // Handle input
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        menuOption = (menuOption - 1 + numOptions) % numOptions;
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        menuOption = (menuOption + 1) % numOptions;
      }
      else if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        
        switch(menuOption) {
          case 0:
            viewFiles("/rx_data");
            break;
          case 1:
            viewFiles("/tx_data");
            break;
          case 2:
            viewFiles("/logs");
            break;
          case 3:
            showCardInfo();
            break;
          case 4:
            inMenu = false;
            break;
        }
      }
      else if(digitalRead(BTN_LEFT) == LOW) {
        lastButtonPress = millis();
        inMenu = false;
      }
    }
    
    delay(10);
  }
}

void viewFiles(const char* path) {
  Serial.printf("[FILES] Viewing files in %s\n", path);
  
  File root = SD_MMC.open(path);
  if(!root || !root.isDirectory()) {
    debugPrint("Cannot open dir!", true, true, 2000);
    return;
  }
  
  // Count files
  int fileCount = 0;
  String fileNames[50];
  
  File file = root.openNextFile();
  while(file && fileCount < 50) {
    if(!file.isDirectory()) {
      fileNames[fileCount] = String(file.name());
      fileCount++;
    }
    file = root.openNextFile();
  }
  root.close();
  
  if(fileCount == 0) {
    debugPrint("No files found!", true, true, 2000);
    return;
  }
  
  // Display file list
  int selectedFile = 0;
  int fileOffset = 0;
  bool viewing = true;
  
  while(viewing) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(F("=[ "));
    display.print(path);
    display.println(F(" ]="));
    display.setCursor(90, 0);
    display.printf("%d/%d", selectedFile + 1, fileCount);
    display.println(F(""));
    
    // Show files
    int maxVisible = 5;
    for(int i = 0; i < maxVisible && (fileOffset + i) < fileCount; i++) {
      if(fileOffset + i == selectedFile) {
        display.fillRect(0, 16 + i*10, 128, 10, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      } else {
        display.setTextColor(SH110X_WHITE);
      }
      
      display.setCursor(2, 17 + i*10);
      String shortName = fileNames[fileOffset + i];
      if(shortName.length() > 20) {
        shortName = shortName.substring(0, 17) + "...";
      }
      display.print(shortName);
    }
    
    display.setTextColor(SH110X_WHITE);
    display.display();
    
    // Handle input
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        if(selectedFile > 0) {
          selectedFile--;
          if(selectedFile < fileOffset) {
            fileOffset = selectedFile;
          }
        }
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        if(selectedFile < fileCount - 1) {
          selectedFile++;
          if(selectedFile >= fileOffset + maxVisible) {
            fileOffset = selectedFile - maxVisible + 1;
          }
        }
      }
      else if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        String fullPath = String(path) + "/" + fileNames[selectedFile];
        fileOptions(fullPath.c_str());
      }
      else if(digitalRead(BTN_LEFT) == LOW) {
        lastButtonPress = millis();
        viewing = false;
      }
    }
    
    delay(10);
  }
}

void fileOptions(const char* filename) {
  Serial.printf("[FILES] File options for %s\n", filename);
  
  bool inOptions = true;
  int option = 0;
  const int numOptions = 4;
  
  while(inOptions) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ FILE OPS ]="));
    display.println(F(""));
    
    // Show filename (truncated)
    String shortName = String(filename);
    int lastSlash = shortName.lastIndexOf('/');
    if(lastSlash >= 0) {
      shortName = shortName.substring(lastSlash + 1);
    }
    display.println(shortName);
    display.println(F(""));
    
    const char* options[] = {
      "View",
      "Transmit",
      "Delete",
      "Back"
    };
    
    for(int i = 0; i < numOptions; i++) {
      if(i == option) {
        display.fillRect(0, 40 + i*10, 128, 10, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      } else {
        display.setTextColor(SH110X_WHITE);
      }
      
      display.setCursor(2, 41 + i*10);
      display.print(options[i]);
    }
    
    display.setTextColor(SH110X_WHITE);
    display.display();
    
    // Handle input
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        option = (option - 1 + numOptions) % numOptions;
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        option = (option + 1) % numOptions;
      }
      else if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        
        switch(option) {
          case 0:
            viewFileContent(filename);
            break;
          case 1:
            if(String(filename).indexOf("rx_") >= 0) {
              loadAndTransmitFile(filename);
            } else {
              debugPrint("Not RX file!", true, true, 2000);
            }
            break;
          case 2:
            deleteFileConfirm(filename);
            inOptions = false;
            break;
          case 3:
            inOptions = false;
            break;
        }
      }
      else if(digitalRead(BTN_LEFT) == LOW) {
        lastButtonPress = millis();
        inOptions = false;
      }
    }
    
    delay(10);
  }
}

void viewFileContent(const char* filename) {
  Serial.printf("[FILES] Viewing content of %s\n", filename);
  
  File file = SD_MMC.open(filename, FILE_READ);
  if(!file) {
    debugPrint("Cannot open file!", true, true, 2000);
    return;
  }
  
  // For small files, show content
  if(file.size() < 1024) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    
    int line = 0;
    while(file.available() && line < 8) {
      String textLine = file.readStringUntil('\n');
      if(textLine.length() > 21) {
        textLine = textLine.substring(0, 21);
      }
      display.println(textLine);
      line++;
    }
    
    display.display();
    file.close();
    
    // Wait for button
    while(digitalRead(BTN_LEFT) == HIGH && 
          digitalRead(BTN_SELECT) == HIGH) {
      delay(10);
    }
    delay(buttonDebounce);
  } else {
    // For large files, show info
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ FILE INFO ]="));
    display.println(F(""));
    display.printf("Size: %d bytes\n", file.size());
    display.println(F(""));
    display.println(F("File too large"));
    display.println(F("to display"));
    display.display();
    file.close();
    delay(3000);
  }
}

void deleteFileConfirm(const char* filename) {
  Serial.printf("[FILES] Delete confirmation for %s\n", filename);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ DELETE? ]="));
  display.println(F(""));
  
  String shortName = String(filename);
  int lastSlash = shortName.lastIndexOf('/');
  if(lastSlash >= 0) {
    shortName = shortName.substring(lastSlash + 1);
  }
  display.println(shortName);
  display.println(F(""));
  display.println(F("RIGHT = Delete"));
  display.println(F("LEFT = Cancel"));
  display.display();
  
  bool waiting = true;
  unsigned long startTime = millis();
  
  while(waiting && (millis() - startTime < 10000)) {
    if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      deleteFile(SD_MMC, filename);
      waiting = false;
    }
    else if(digitalRead(BTN_LEFT) == LOW) {
      debugPrint("Cancelled", true, true, 1000);
      waiting = false;
    }
    delay(10);
  }
}

void deleteFile(fs::FS &fs, const char* path) {
  Serial.printf("[FILES] Deleting %s\n", path);
  
  if(fs.remove(path)) {
    Serial.println(F("[FILES] File deleted"));
    logEvent("FILE_DELETE", path);
    debugPrint("File deleted!", true, true, 1500);
  } else {
    Serial.println(F("[FILES] Delete failed"));
    debugPrint("Delete failed!", true, true, 2000);
  }
}

void showCardInfo() {
  if(!sdCardPresent) {
    debugPrint("No SD card!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[FILES] ========== SD CARD INFO =========="));
  
  uint8_t cardType = SD_MMC.cardType();
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);
  uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
  
  const char* typeStr = "Unknown";
  if(cardType == CARD_MMC) typeStr = "MMC";
  else if(cardType == CARD_SD) typeStr = "SDSC";
  else if(cardType == CARD_SDHC) typeStr = "SDHC";
  
  Serial.printf("Card Type: %s\n", typeStr);
  Serial.printf("Card Size: %lluMB\n", cardSize);
  Serial.printf("Used: %lluMB\n", usedBytes);
  Serial.printf("Total: %lluMB\n", totalBytes);
  Serial.printf("Free: %lluMB\n", totalBytes - usedBytes);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ SD INFO ]="));
  display.println(F(""));
  display.printf("Type: %s\n", typeStr);
  display.printf("Size: %lluMB\n", cardSize);
  display.printf("Used: %lluMB\n", usedBytes);
  display.printf("Free: %lluMB\n", totalBytes - usedBytes);
  display.println(F(""));
  display.println(F("Press any button"));
  display.display();
  
  // Wait for button
  while(digitalRead(BTN_UP) == HIGH && 
        digitalRead(BTN_DOWN) == HIGH &&
        digitalRead(BTN_LEFT) == HIGH && 
        digitalRead(BTN_RIGHT) == HIGH &&
        digitalRead(BTN_SELECT) == HIGH) {
    delay(10);
  }
  delay(buttonDebounce);
}

// Utility functions for main code
void appendFile(fs::FS &fs, const char* path, const char* message, String messagestring) {
  logs = fs.open(path, FILE_APPEND);
  if(!logs) {
    Serial.println(F("[FILES] Failed to open file for append"));
    return;
  }
  if(message) logs.print(message);
  if(messagestring.length() > 0) logs.print(messagestring);
  logs.close();
}

void appendFileLong(fs::FS &fs, const char* path, unsigned long messagechar) {
  logs = fs.open(path, FILE_APPEND);
  if(!logs) {
    Serial.println(F("[FILES] Failed to open file for append"));
    return;
  }
  logs.print(messagechar);
  logs.close();
}

//========================================================================
// HARDWARE.INO

// Hardware.ino - Hardware initialization, startup animation, about, credits

const float commonFrequencies[] = {
  300.00, 303.87, 304.25, 310.00, 315.00, 318.00,
  330.00, 336.00, 340.00, 350.00,
  387.00, 390.00, 418.00, 433.05, 433.42, 433.92,
  434.00, 434.78, 438.90,
  868.00, 868.30, 868.95, 869.85,
  902.00, 903.00, 904.00, 905.00, 906.00, 907.00,
  908.00, 909.00, 910.00, 911.00, 912.00, 913.00,
  914.00, 915.00, 916.00, 917.00, 918.00, 919.00,
  920.00, 921.00, 922.00, 923.00, 924.00, 925.00,
  926.00, 927.00
};
const int numFrequencies = sizeof(commonFrequencies) / sizeof(commonFrequencies[0]);

// Forward declarations
void initializeButtons();
void initializeSDCard(bool displayOK);
void initializeCC1101(bool displayOK);
void showHardwareSummary(bool displayOK);
void loadSettings();
void testCC1101SPI();

void initializeHardware() {
  // Initialize NeoPixels first for visual feedback
  pixels.begin();
  pixels.setBrightness(50);
  pixels.clear();
  pixels.show();
  
  Serial.println(F("\n[TPP Badge] Booting..."));
  Serial.println(F("[+] Starting hardware initialization"));
  
  // Initialize I2C for display
  Serial.println(F("[+] Initializing I2C..."));
  Wire.begin(8, 6); // SDA, SCL
  delay(100);
  
  // Initialize display
  Serial.println(F("[+] Initializing SH1106 OLED..."));
  bool displayOK = false;
  if(!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println(F("[!] SH1106 at 0x3C failed, trying 0x3D"));
    if(!display.begin(0x3D, true)) {
      Serial.println(F("[!] Display initialization failed"));
    } else {
      displayOK = true;
    }
  } else {
    displayOK = true;
  }
  
  if(displayOK) {
    Serial.println(F("[+] Display initialized successfully"));
    showStartupAnimation();
  }
  
  // Now show boot progress
  debugPrint("Initializing...", true, displayOK, 500);
  
  // Initialize buttons
  initializeButtons();
  
  // Initialize EEPROM
  debugPrint("[+] Init EEPROM...", true, displayOK, 500);
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Initialize SD Card
  initializeSDCard(displayOK);
  
  // Test CC1101 SPI before full initialization
  testCC1101SPI();
  
  // Initialize CC1101 modules
  initializeCC1101(displayOK);
  
  // Show hardware summary
  showHardwareSummary(displayOK);
}

void initializeButtons() {
  debugPrint("[+] Init Buttons...", true, true, 500);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
}

void initializeSDCard(bool displayOK) {
  debugPrint("[+] Init SD Card...", true, displayOK, 500);
  
  if(!SD_MMC.begin("/sdcard", true)) { // 1-bit mode
    Serial.println(F("[!] SD Card Mount Failed"));
    if(!SD_MMC.begin()) {
      Serial.println(F("[!] SD Card initialization failed"));
      debugPrint("No SD Card", true, displayOK, 1000);
      sdCardPresent = false;
    } else {
      Serial.println(F("[+] SD Card initialized (4-bit mode)"));
      debugPrint("SD Card OK!", true, displayOK, 500);
      sdCardPresent = true;
    }
  } else {
    Serial.println(F("[+] SD Card initialized (1-bit mode)"));
    debugPrint("SD Card OK!", true, displayOK, 500);
    sdCardPresent = true;
  }
  
  if(sdCardPresent) {
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE) {
      Serial.println(F("[!] No SD card attached"));
      sdCardPresent = false;
    } else {
      Serial.print(F("SD Card Type: "));
      if(cardType == CARD_MMC) Serial.println(F("MMC"));
      else if(cardType == CARD_SD) Serial.println(F("SDSC"));
      else if(cardType == CARD_SDHC) Serial.println(F("SDHC"));
      else Serial.println(F("UNKNOWN"));
      
      uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
      Serial.printf("SD Card Size: %lluMB\n", cardSize);
    }
  }
}

void testCC1101SPI() {
  Serial.println(F("[CC1101] Testing SPI communication..."));
  debugPrint("[+] Test CC1101 SPI...", true, true, 500);
  
  // Configure SPI pins for both modules
  ELECHOUSE_cc1101.addSpiPin(CC1101_CLK_A, CC1101_MISO_A, CC1101_MOSI_A, CC1101_CS_A, 0);
  ELECHOUSE_cc1101.addSpiPin(CC1101_CLK_B, CC1101_MISO_B, CC1101_MOSI_B, CC1101_CS_B, 1);
  
  // Test module A
  Serial.println(F("[CC1101] Testing Module A..."));
  ELECHOUSE_cc1101.setModul(0);
  
  // Try to read the VERSION register (address 0x31 in status registers)
  // CC1101 should return 0x14 (version) or 0x04 (partnum)
  pinMode(CC1101_CS_A, OUTPUT);
  digitalWrite(CC1101_CS_A, HIGH);
  delay(10);
  
  SPI.begin(CC1101_CLK_A, CC1101_MISO_A, CC1101_MOSI_A, CC1101_CS_A);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); // 1MHz for testing
  
  // Reset sequence
  digitalWrite(CC1101_CS_A, LOW);
  delay(1);
  digitalWrite(CC1101_CS_A, HIGH);
  delay(1);
  digitalWrite(CC1101_CS_A, LOW);
  
  // Wait for MISO to go low (chip ready)
  unsigned long timeout = millis() + 100;
  while(digitalRead(CC1101_MISO_A) && millis() < timeout);
  
  if(millis() >= timeout) {
    Serial.println(F("[CC1101] Module A timeout waiting for MISO"));
    cc1101APresent = false;
  } else {
    // Send SRES command (reset)
    SPI.transfer(0x30);
    delay(1);
    
    // Read VERSION register (0x31 | 0xC0 for status register burst read)
    SPI.transfer(0x31 | 0xC0);
    byte version = SPI.transfer(0x00);
    
    digitalWrite(CC1101_CS_A, HIGH);
    SPI.endTransaction();
    SPI.end();
    
    Serial.printf("[CC1101] Module A VERSION register: 0x%02X\n", version);
    
    if(version == 0x14 || version == 0x04 || (version > 0 && version < 0xFF)) {
      Serial.println(F("[CC1101] Module A detected!"));
      cc1101APresent = true;
    } else {
      Serial.println(F("[CC1101] Module A not detected"));
      cc1101APresent = false;
    }
  }
  
  // Test module B
  Serial.println(F("[CC1101] Testing Module B..."));
  ELECHOUSE_cc1101.setModul(1);
  
  pinMode(CC1101_CS_B, OUTPUT);
  digitalWrite(CC1101_CS_B, HIGH);
  delay(10);
  
  SPI.begin(CC1101_CLK_B, CC1101_MISO_B, CC1101_MOSI_B, CC1101_CS_B);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  // Reset sequence
  digitalWrite(CC1101_CS_B, LOW);
  delay(1);
  digitalWrite(CC1101_CS_B, HIGH);
  delay(1);
  digitalWrite(CC1101_CS_B, LOW);
  
  // Wait for MISO to go low
  timeout = millis() + 100;
  while(digitalRead(CC1101_MISO_B) && millis() < timeout);
  
  if(millis() >= timeout) {
    Serial.println(F("[CC1101] Module B timeout waiting for MISO"));
    cc1101BPresent = false;
  } else {
    // Send SRES command
    SPI.transfer(0x30);
    delay(1);
    
    // Read VERSION register
    SPI.transfer(0x31 | 0xC0);
    byte version = SPI.transfer(0x00);
    
    digitalWrite(CC1101_CS_B, HIGH);
    SPI.endTransaction();
    SPI.end();
    
    Serial.printf("[CC1101] Module B VERSION register: 0x%02X\n", version);
    
    if(version == 0x14 || version == 0x04 || (version > 0 && version < 0xFF)) {
      Serial.println(F("[CC1101] Module B detected!"));
      cc1101BPresent = true;
    } else {
      Serial.println(F("[CC1101] Module B not detected"));
      cc1101BPresent = false;
    }
  }
  
  Serial.printf("[CC1101] Test complete. A=%d, B=%d\n", cc1101APresent, cc1101BPresent);
}

void initializeCC1101(bool displayOK) {
  debugPrint("[+] Init CC1101s...", true, displayOK, 500);
  
  Serial.println(F("[CC1101] Starting initialization"));
  
  // Configure GDO0 pins for both modules
  ELECHOUSE_cc1101.addGDO0(CC1101_GDO0_A, 0);
  ELECHOUSE_cc1101.addGDO0(CC1101_GDO0_B, 1);
  
  // Only mark modules as ready if they passed the SPI test
  // We DON'T call Init() here - it will be called when actually using the module
  if(cc1101APresent) {
    debugPrint("CC1101-A Ready", true, displayOK, 500);
    Serial.println(F("[CC1101] Module A ready (will init on use)"));
  }
  
  if(cc1101BPresent) {
    debugPrint("CC1101-B Ready", true, displayOK, 500);
    Serial.println(F("[CC1101] Module B ready (will init on use)"));
  }
  
  // Set default module
  if(cc1101APresent) {
    activeModule = 0;
    ELECHOUSE_cc1101.setModul(0);
    Serial.println(F("[CC1101] Using Module A as default"));
  } else if(cc1101BPresent) {
    activeModule = 1;
    ELECHOUSE_cc1101.setModul(1);
    Serial.println(F("[CC1101] Using Module B as default"));
  } else {
    Serial.println(F("[CC1101] WARNING: No CC1101 modules detected!"));
    debugPrint("No CC1101 Found", true, displayOK, 1000);
  }
  
  Serial.printf("[CC1101] Initialization complete. A=%d, B=%d\n", cc1101APresent, cc1101BPresent);
}

void showHardwareSummary(bool displayOK) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ HARDWARE ]="));
  display.println(F(""));
  display.print(F("Display: "));
  display.println(displayOK ? F("OK") : F("FAIL"));
  display.print(F("SD Card: "));
  display.println(sdCardPresent ? F("OK") : F("NO"));
  display.print(F("CC1101-A: "));
  display.println(cc1101APresent ? F("OK") : F("NO"));
  display.print(F("CC1101-B: "));
  display.println(cc1101BPresent ? F("OK") : F("NO"));
  display.println(F(""));
  display.println(F("Ready!"));
  display.display();
  
  Serial.println(F("[+] TPP Badge Ready!"));
  
  // Quick flash to indicate ready
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  }
  pixels.show();
  
  delay(2000);
  
  pixels.clear();
  pixels.show();
  
  display.clearDisplay();
  display.display();
  
  currentMenu = MENU_MAIN;
  previousMenu = MENU_MAIN;
  menuSelection = 0;
  menuOffset = 0;
  
  lastButtonPress = millis();
}

void showStartupAnimation() {
  // ========== PHASE 1: ANIMATION ==========
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 10);
  display.println(F("TPP"));
  display.setTextSize(1);
  display.setCursor(20, 35);
  display.println(F("DEFCON 33"));
  display.setCursor(15, 50);
  display.println(F("Pirates Plunder"));
  display.display();
  
  // Red flashing effect on LEDs
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < NEOPIXEL_COUNT; j++) {
      pixels.setPixelColor(j, pixels.Color(255, 0, 0));
    }
    pixels.show();
    delay(200);
    pixels.clear();
    pixels.show();
    delay(800);
  }
   
  // ========== PHASE 2: SKULL AND CROSSBONES ON WAVES ==========
  for(int frame = 0; frame < 30; frame++) {
    display.clearDisplay();
    
    // Draw skull (matching original exactly)
    int skullY = 20 + sin(frame * 0.2) * 3;
    
    // Draw crossbones first (behind skull)
    display.drawLine(25, skullY + 10, 103, skullY + 10, SH110X_WHITE);
    display.drawLine(25, skullY + 11, 103, skullY + 11, SH110X_WHITE);
    display.drawLine(25, skullY + 15, 103, skullY + 5, SH110X_WHITE);
    display.drawLine(25, skullY + 16, 103, skullY + 6, SH110X_WHITE);
    
    // Bone ends
    display.fillCircle(25, skullY + 10, 3, SH110X_WHITE);
    display.fillCircle(103, skullY + 10, 3, SH110X_WHITE);
    display.fillCircle(25, skullY + 15, 3, SH110X_WHITE);
    display.fillCircle(103, skullY + 5, 3, SH110X_WHITE);
    
    // Skull head
    display.fillCircle(64, skullY, 20, SH110X_WHITE);
    
    // Eye sockets (larger, matching original)
    display.fillCircle(56, skullY - 5, 5, SH110X_BLACK);
    display.fillCircle(72, skullY - 5, 5, SH110X_BLACK);
    
    // Evil eye dots
    display.fillCircle(56, skullY - 5, 2, SH110X_WHITE);
    display.fillCircle(72, skullY - 5, 2, SH110X_WHITE);
    
    // Nose (triangular)
    display.fillTriangle(64, skullY + 2, 61, skullY + 6, 67, skullY + 6, SH110X_BLACK);
    
    // Evil grin
    display.drawLine(56, skullY + 10, 72, skullY + 10, SH110X_BLACK);
    display.drawLine(55, skullY + 9, 57, skullY + 10, SH110X_BLACK);
    display.drawLine(71, skullY + 10, 73, skullY + 9, SH110X_BLACK);
    
    // Teeth
    for(int i = 0; i < 5; i++) {
      display.drawLine(58 + i*3, skullY + 10, 58 + i*3, skullY + 13, SH110X_BLACK);
    }
    
    // Draw waves at bottom
    for(int x = 0; x < 128; x += 10) {
      int waveY = 48 + sin((x + frame * 5) * 0.1) * 3;
      display.drawLine(x, waveY, x + 10, waveY, SH110X_WHITE);
    }
    
    // Text
    display.setTextSize(1);
    display.setCursor(40, 55);
    display.print(F("TPP BADGE"));
    
    display.display();
    
    // Animate LEDs - pirate ship sailing
    pixels.clear();
    int shipPos = (frame * 2) % NEOPIXEL_COUNT;
    
    // Ship hull (red)
    pixels.setPixelColor(shipPos, pixels.Color(255, 0, 0));
    if(shipPos > 0) pixels.setPixelColor(shipPos - 1, pixels.Color(128, 0, 0));
    if(shipPos < NEOPIXEL_COUNT - 1) pixels.setPixelColor(shipPos + 1, pixels.Color(128, 0, 0));
    
    // Mast (white)
    int mastPos = (shipPos + 2) % NEOPIXEL_COUNT;
    pixels.setPixelColor(mastPos, pixels.Color(255, 255, 255));
    
    // Wake (blue)
    for(int j = 3; j < 8; j++) {
      int wakePos = (shipPos - j + NEOPIXEL_COUNT) % NEOPIXEL_COUNT;
      int brightness = 255 - (j * 30);
      if(brightness > 0) {
        pixels.setPixelColor(wakePos, pixels.Color(0, 0, brightness));
      }
    }
    
    pixels.show();
    delay(50);
  }
  
  // ========== PHASE 3: MATRIX RAIN TRANSITION (not working?) ==========
  uint8_t drops[16]; // 16 columns for matrix effect
  uint8_t dropSpeed[16];
  for(int i = 0; i < 16; i++) {
    drops[i] = random(64);
    dropSpeed[i] = random(1, 4);
  }
  
  int skullY = 20; // Final skull position
  
  for(int frame = 0; frame < 60; frame++) {
    display.clearDisplay();
    
    // Matrix rain effect
    for(int i = 0; i < 16; i++) {
      for(int j = 0; j < 8; j++) {
        int y = drops[i] - j * 8;
        if(y >= 0 && y < 64) {
          display.setCursor(i * 8, y);
          display.print(random(2));
        }
      }
      
      if(frame % dropSpeed[i] == 0) {
        drops[i]++;
        if(drops[i] > 70) drops[i] = 0;
      }
    }
    
    // Skull starts to fall after frame 20
    if(frame > 20) {
      skullY += 2;
      if(skullY > 48) skullY = 48;
      
      // Draw falling skull (smaller)
      display.fillCircle(64, skullY, 10, SH110X_WHITE);
      display.fillCircle(60, skullY - 2, 3, SH110X_BLACK);
      display.fillCircle(68, skullY - 2, 3, SH110X_BLACK);
      display.drawPixel(60, skullY - 2, SH110X_WHITE);
      display.drawPixel(68, skullY - 2, SH110X_WHITE);
    }
    
    display.display();
    
    // Matrix LEDs
    pixels.clear();
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      if(random(10) < 3) {
        pixels.setPixelColor(i, pixels.Color(0, random(50, 255), 0));
      }
    }
    pixels.show();
    delay(50);
  }
  
  // ========== PHASE 4: SKULL DROPS INTO WATER ==========
  for(int frame = 0; frame < 20; frame++) {
    display.clearDisplay();
    
    // Water splash effect
    if(frame < 10) {
      int splashRadius = frame * 3;
      display.drawCircle(64, 48, splashRadius, SH110X_WHITE);
      
      // Splash droplets
      for(int i = 0; i < 8; i++) {
        float angle = (i * 45) * PI / 180;
        int x = 64 + cos(angle) * (splashRadius + 5);
        int y = 48 + sin(angle) * (splashRadius + 5) - frame;
        display.fillCircle(x, y, 2, SH110X_WHITE);
      }
    }
    
    // Ripples
    for(int r = 0; r < 3; r++) {
      int rippleRadius = (frame * 2) + (r * 10);
      if(rippleRadius < 64) {
        display.drawCircle(64, 48, rippleRadius, SH110X_WHITE);
      }
    }
    
    display.display();
    
    // Water splash LEDs
    pixels.clear();
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      int brightness = random(0, 255 - (frame * 10));
      if(brightness > 0) {
        pixels.setPixelColor(i, pixels.Color(0, 0, brightness));
      }
    }
    pixels.show();
    delay(50);
  }
  
  // ========== FINAL: RETURN TO ORIGINAL LOGO ==========
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 10);
  display.println(F("TPP"));
  display.setTextSize(1);
  display.setCursor(20, 35);
  display.println(F("DEFCON 33"));
  display.setCursor(15, 50);
  display.println(F("Pirates Plunder"));
  display.display();
  
  // Final LED effect - pirate red wave
  for(int wave = 0; wave < 2; wave++) {
    for(int i = 0; i < NEOPIXEL_COUNT + 10; i++) {
      pixels.clear();
      for(int j = 0; j < 10; j++) {
        int pos = i - j;
        if(pos >= 0 && pos < NEOPIXEL_COUNT) {
          int brightness = 255 - (j * 25);
          pixels.setPixelColor(pos, pixels.Color(brightness, 0, 0));
        }
      }
      pixels.show();
      delay(30);
    }
  }
  
  pixels.clear();
  pixels.show();
  
  delay(1000);
}

// Credits handling
unsigned long lastCreditScroll = 0;
int creditScrollPos = 0;
const char* creditLines[] = {
  "=[ CREDITS ]=",
  "",
  "Hardware Design:",
  "  TehRabbitt",
  "  rabbit-labs.com",
  "",
  "Firmware Dev:",
  "  RocketGod",
  "  betaskynet.com",
  "",
  "Discord:",
  "  discord.gg/",
  "  thepirates",
  "",
  "Special Thanks:",
  "  TPP Crew",
  "  Argh mateys!",
  "",
  "DEF CON 33",
  "2025",
  "",
  "Now fuck off!",
  "",
  ""
};
const int numCreditLines = sizeof(creditLines) / sizeof(creditLines[0]);

void drawAbout() {
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ ABOUT ]="));
  display.println(F(""));
  display.println(F("TPP Badge v1.0"));
  display.println(F("DEF CON 33"));
  display.println(F(""));
  display.println(F("The Pirates'"));
  display.println(F("Plunder"));
}

void drawCredits() {
  if (millis() - lastCreditScroll > 500) {
    lastCreditScroll = millis();
    creditScrollPos++;
    if (creditScrollPos >= numCreditLines - 6) {
      creditScrollPos = 0;
    }
  }
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  for (int i = 0; i < 8 && (creditScrollPos + i) < numCreditLines; i++) {
    display.setCursor(0, i * 8);
    display.println(creditLines[creditScrollPos + i]);
  }
}

const char* getModulationName(int modulation) {
  switch(modulation) {
    case 0: return "2-FSK";
    case 1: return "GFSK";
    case 2: return "ASK/OOK";
    case 3: return "4-FSK";
    case 4: return "MSK";
    default: return "Unknown";
  }
}

//======================================================================
// JAMMER.INO

// Jammer.ino - RF Jammer functions

unsigned long jammerStartTime = 0;
float jammerStartFreq = 433.0;
float jammerEndFreq = 434.0;
float jammerCurrentFreq = 433.0;
unsigned long lastJammerUpdate = 0;

// CC1101 Direct SPI functions for Jammer
byte writeJammerRegister(byte addr, byte value, int module) {
  int csPin = (module == 0) ? CC1101_CS_A : CC1101_CS_B;
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  
  int misoPin = (module == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  while(digitalRead(misoPin));
  
  SPI.transfer(addr);
  SPI.transfer(value);
  
  digitalWrite(csPin, HIGH);
  return value;
}

void strobeJammer(byte cmd, int module) {
  int csPin = (module == 0) ? CC1101_CS_A : CC1101_CS_B;
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  
  int misoPin = (module == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  while(digitalRead(misoPin));
  
  SPI.transfer(cmd);
  
  digitalWrite(csPin, HIGH);
}

void setJammerFrequency(float freq, int module) {
  // Calculate frequency registers
  byte freq2, freq1, freq0;
  
  // Simplified frequency calculation for common bands
  if(freq >= 299 && freq <= 301) {
    freq2 = 0x0B; freq1 = 0x89; freq0 = 0x9A; // ~300 MHz
  } else if(freq >= 314 && freq <= 316) {
    freq2 = 0x0C; freq1 = 0x4E; freq0 = 0xC4; // ~315 MHz
  } else if(freq >= 432 && freq <= 435) {
    float offset = (freq - 433.92) * 255.0;
    freq2 = 0x10; 
    freq1 = 0xB0 + (int)(offset / 256);
    freq0 = 0x7A + (int)offset % 256;
  } else if(freq >= 867 && freq <= 869) {
    freq2 = 0x21; freq1 = 0x65; freq0 = 0x6A; // ~868 MHz
  } else if(freq >= 914 && freq <= 916) {
    freq2 = 0x23; freq1 = 0x31; freq0 = 0x3B; // ~915 MHz
  } else {
    // Default to requested frequency (rough approximation)
    long freqCalc = (long)(freq * 65536.0 / 26.0);
    freq2 = (freqCalc >> 16) & 0xFF;
    freq1 = (freqCalc >> 8) & 0xFF;
    freq0 = freqCalc & 0xFF;
  }
  
  writeJammerRegister(0x0D, freq2, module);
  writeJammerRegister(0x0E, freq1, module);
  writeJammerRegister(0x0F, freq0, module);
}

void startJammer() {
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[JAMMER] ========== STARTING JAMMER =========="));
  Serial.println(F("[JAMMER] WARNING: Check local regulations!"));
  
  debugPrint("Starting Jammer!", true, true, 1000);
  
  jammer_tx = "1";
  pixelMode = PIXEL_JAMMER;
  jammerStartTime = millis();
  
  // Use whichever module is available
  if(activeModule == 0 && !cc1101APresent) {
    activeModule = 1;
    Serial.println(F("[JAMMER] Module A not present, switching to B"));
  } else if(activeModule == 1 && !cc1101BPresent) {
    activeModule = 0;
    Serial.println(F("[JAMMER] Module B not present, switching to A"));
  }
  
  Serial.printf("[JAMMER] Configuration:\n");
  Serial.printf("         Module: %c\n", activeModule == 0 ? 'A' : 'B');
  Serial.printf("         Frequency: %.2f MHz\n", frequency);
  Serial.printf("         Power: %d dBm\n", jammerPower);
  Serial.printf("         Mode: %s\n", jammerSweep ? "Sweep" : "Fixed");
  
  // Set TX pin mode
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, LOW);
  
  // Initialize SPI for the selected module
  int sckPin = (activeModule == 0) ? CC1101_CLK_A : CC1101_CLK_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  int mosiPin = (activeModule == 0) ? CC1101_MOSI_A : CC1101_MOSI_B;
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  
  SPI.begin(sckPin, misoPin, mosiPin, csPin);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  
  // Reset CC1101
  strobeJammer(0x30, activeModule); // SRES
  delay(10);
  
  // Set initial frequency
  setJammerFrequency(frequency, activeModule);
  
  // Configure for ASK/OOK modulation (best for jamming)
  writeJammerRegister(0x10, 0x00, activeModule); // MDMCFG4 - Max BW
  writeJammerRegister(0x11, 0x22, activeModule); // MDMCFG3
  writeJammerRegister(0x12, 0x30, activeModule); // MDMCFG2 - ASK/OOK, no sync
  writeJammerRegister(0x07, 0x04, activeModule); // PKTCTRL1
  writeJammerRegister(0x08, 0x00, activeModule); // PKTCTRL0
  writeJammerRegister(0x1B, 0x40, activeModule); // AGCCTRL2
  writeJammerRegister(0x1C, 0x00, activeModule); // AGCCTRL1
  writeJammerRegister(0x1D, 0x91, activeModule); // AGCCTRL0
  
  // Set TX power based on jammerPower setting
  byte patable = 0xC0; // Default max
  if(jammerPower <= -30) patable = 0x12;
  else if(jammerPower <= -20) patable = 0x0E;
  else if(jammerPower <= -15) patable = 0x1D;
  else if(jammerPower <= -10) patable = 0x34;
  else if(jammerPower <= 0) patable = 0x60;
  else if(jammerPower <= 5) patable = 0x84;
  else if(jammerPower <= 7) patable = 0xC8;
  else if(jammerPower <= 10) patable = 0xC0;
  else patable = 0xC0; // Max power for anything above 10
  
  writeJammerRegister(0x3E, patable, activeModule); // PATABLE
  
  // Configure GDO0 for TX
  writeJammerRegister(0x02, 0x0C, activeModule); // IOCFG0 - Serial TX data
  
  // Calibrate and enter TX
  strobeJammer(0x33, activeModule); // SCAL
  delay(5);
  strobeJammer(0x35, activeModule); // STX
  delay(5);
  
  // Set up sweep parameters if enabled
  if(jammerSweep) {
    jammerStartFreq = frequency - 1.0;
    jammerEndFreq = frequency + 1.0;
    jammerCurrentFreq = jammerStartFreq;
    
    // Ensure frequencies are in valid range
    if(jammerStartFreq < 300.0) jammerStartFreq = 300.0;
    if(jammerEndFreq > 928.0) jammerEndFreq = 928.0;
  }
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("!!! JAMMER !!!"));
  display.drawRect(0, 0, 128, 10, SH110X_WHITE);
  display.setCursor(0,15);
  display.printf("Freq: %.2f MHz\n", frequency);
  display.printf("Power: %d dBm\n", jammerPower);
  display.printf("Module: %c\n", activeModule == 0 ? 'A' : 'B');
  display.setCursor(0,50);
  display.println(F("Press any button"));
  display.println(F("to stop"));
  display.display();
  
  Serial.println(F("[JAMMER] Jammer active - Press any button to stop"));
}

void stopJammer() {
  Serial.println(F("\n[JAMMER] ========== STOPPING JAMMER =========="));
  
  debugPrint("Stopping Jammer", true, true, 1000);
  
  jammer_tx = "0";
  pixelMode = PIXEL_MENU;
  
  if(cc1101APresent || cc1101BPresent) {
    strobeJammer(0x36, activeModule); // SIDLE
    Serial.println(F("[JAMMER] CC1101 set to idle"));
    
    SPI.endTransaction();
    SPI.end();
  }
  
  // Set TX pin low
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  digitalWrite(txPin, LOW);
  
  unsigned long jamTime = (millis() - jammerStartTime) / 1000;
  Serial.printf("[JAMMER] Total jam time: %lu seconds\n", jamTime);
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("Jammer Stopped!"));
  display.printf("Duration: %lu sec\n", jamTime);
  display.display();
  delay(1500);
  
  // Return to jammer menu
  currentMenu = MENU_JAMMER;
  updateDisplay();
}

void runJammer() {
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  
  // Check for button press to stop
  if(digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
     digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW || 
     digitalRead(BTN_SELECT) == LOW) {
    if(millis() - lastButtonPress > buttonDebounce) {
      lastButtonPress = millis();
      stopJammer();
      return;
    }
  }
  
  // Different jamming patterns
  static int pattern = 0;
  static unsigned long patternTime = 0;
  
  if(millis() - patternTime > 100) {
    patternTime = millis();
    pattern = (pattern + 1) % 4;
  }
  
  switch(pattern) {
    case 0:
      // Fast pulse jamming
      for(int i = 0; i < 10; i++) {
        digitalWrite(txPin, HIGH);
        delayMicroseconds(100);
        digitalWrite(txPin, LOW);
        delayMicroseconds(100);
      }
      break;
      
    case 1:
      // Random noise
      for(int i = 0; i < 20; i++) {
        digitalWrite(txPin, random(2));
        delayMicroseconds(random(50, 200));
      }
      break;
      
    case 2:
      // Continuous carrier
      digitalWrite(txPin, HIGH);
      delay(5);
      digitalWrite(txPin, LOW);
      break;
      
    case 3:
      // Sweep pattern
      for(int i = 0; i < 50; i++) {
        digitalWrite(txPin, HIGH);
        delayMicroseconds(i * 10);
        digitalWrite(txPin, LOW);
        delayMicroseconds(i * 10);
      }
      break;
  }
  
  // Update display periodically
  if(millis() - lastJammerUpdate > 1000) {
    lastJammerUpdate = millis();
    updateJammerDisplay();
    
    // Frequency sweep mode
    if(jammerSweep) {
      jammerCurrentFreq += 0.1;
      if(jammerCurrentFreq > jammerEndFreq) {
        jammerCurrentFreq = jammerStartFreq;
      }
      
      // Update frequency
      strobeJammer(0x36, activeModule); // SIDLE
      setJammerFrequency(jammerCurrentFreq, activeModule);
      strobeJammer(0x33, activeModule); // SCAL
      delay(1);
      strobeJammer(0x35, activeModule); // STX
      
      Serial.printf("[JAMMER] Sweep frequency: %.2f MHz\n", jammerCurrentFreq);
    }
  }
}

void updateJammerDisplay() {
  unsigned long runtime = (millis() - jammerStartTime) / 1000;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("!!! JAMMING !!!"));
  display.drawRect(0, 0, 128, 10, SH110X_WHITE);
  
  // Flashing warning
  if((millis() / 500) % 2) {
    display.fillRect(1, 1, 126, 8, SH110X_WHITE);
    display.setTextColor(SH110X_BLACK);
    display.setCursor(35, 1);
    display.print(F("JAMMING"));
    display.setTextColor(SH110X_WHITE);
  }
  
  display.setCursor(0,15);
  if(jammerSweep) {
    display.printf("Freq: %.2f MHz\n", jammerCurrentFreq);
    display.printf("Range: %.1f-%.1f\n", jammerStartFreq, jammerEndFreq);
  } else {
    display.printf("Freq: %.2f MHz\n", frequency);
  }
  display.printf("Power: %d dBm\n", jammerPower);
  display.printf("Time: %02lu:%02lu\n", runtime / 60, runtime % 60);
  
  // Activity indicator
  display.setCursor(0,45);
  for(int i = 0; i < 16; i++) {
    display.print((millis() / 100 + i) % 2 ? "=" : "-");
  }
  
  display.setCursor(0,55);
  display.println(F("Any button = stop"));
  display.display();
}

void configureJammer() {
  Serial.println(F("\n[JAMMER] ===== JAMMER CONFIG ====="));
  
  bool configuring = true;
  int configOption = 0;
  const int numOptions = 4;
  
  while(configuring) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ JAM CONFIG ]="));
    display.println(F(""));
    
    // Menu options
    const char* options[] = {
      "Frequency",
      "Power",
      "Mode",
      "Back"
    };
    
    // Draw menu
    for(int i = 0; i < numOptions; i++) {
      if(i == configOption) {
        display.fillRect(0, 16 + i*10, 128, 10, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      } else {
        display.setTextColor(SH110X_WHITE);
      }
      
      display.setCursor(2, 17 + i*10);
      display.print(options[i]);
      
      // Show current values
      display.setCursor(70, 17 + i*10);
      switch(i) {
        case 0:
          display.printf("%.2f", frequency);
          break;
        case 1:
          display.printf("%d dBm", jammerPower);
          break;
        case 2:
          display.print(jammerSweep ? "Sweep" : "Fixed");
          break;
      }
    }
    
    display.setTextColor(SH110X_WHITE);
    display.display();
    
    // Handle input
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        configOption = (configOption - 1 + numOptions) % numOptions;
        Serial.printf("[JAMMER] Config option: %d\n", configOption);
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        configOption = (configOption + 1) % numOptions;
        Serial.printf("[JAMMER] Config option: %d\n", configOption);
      }
      else if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        
        switch(configOption) {
          case 0: // Frequency
            adjustJammerFrequency();
            break;
          case 1: // Power
            adjustJammerPower();
            break;
          case 2: // Mode
            jammerSweep = !jammerSweep;
            if(jammerSweep) {
              setupJammerSweep();
            }
            Serial.printf("[JAMMER] Mode: %s\n", jammerSweep ? "Sweep" : "Fixed");
            break;
          case 3: // Back
            configuring = false;
            break;
        }
      }
      else if(digitalRead(BTN_LEFT) == LOW) {
        lastButtonPress = millis();
        configuring = false;
      }
    }
    
    delay(10);
  }
  
  saveSettings();
  Serial.println(F("[JAMMER] Configuration saved"));
}

void adjustJammerFrequency() {
  Serial.println(F("[JAMMER] Adjusting frequency"));
  
  bool adjusting = true;
  
  while(adjusting) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ FREQUENCY ]="));
    display.println(F(""));
    display.setCursor(10,20);
    display.setTextSize(2);
    display.printf("%.2f", frequency);
    display.setTextSize(1);
    display.println(F(" MHz"));
    display.setCursor(0,45);
    display.println(F("UP/DOWN: +/- 0.25"));
    display.println(F("LEFT: Back"));
    display.display();
    
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        frequency += 0.25;
        if(frequency > 928.0) frequency = 300.0;
        Serial.printf("[JAMMER] Frequency: %.2f MHz\n", frequency);
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        frequency -= 0.25;
        if(frequency < 300.0) frequency = 928.0;
        Serial.printf("[JAMMER] Frequency: %.2f MHz\n", frequency);
      }
      else if(digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        adjusting = false;
      }
    }
    
    delay(10);
  }
}

void adjustJammerPower() {
  Serial.println(F("[JAMMER] Adjusting power"));
  
  bool adjusting = true;
  
  while(adjusting) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ TX POWER ]="));
    display.println(F(""));
    display.setCursor(20,20);
    display.setTextSize(2);
    display.printf("%d dBm", jammerPower);
    display.setTextSize(1);
    display.setCursor(0,45);
    display.println(F("UP/DOWN: +/- 1dBm"));
    display.println(F("LEFT: Back"));
    display.display();
    
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        jammerPower++;
        if(jammerPower > 12) jammerPower = 12;
        Serial.printf("[JAMMER] Power: %d dBm\n", jammerPower);
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        jammerPower--;
        if(jammerPower < -30) jammerPower = -30;
        Serial.printf("[JAMMER] Power: %d dBm\n", jammerPower);
      }
      else if(digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        adjusting = false;
      }
    }
    
    delay(10);
  }
}

void setupJammerSweep() {
  Serial.println(F("[JAMMER] Setting up frequency sweep"));
  
  jammerStartFreq = frequency - 1.0;
  jammerEndFreq = frequency + 1.0;
  jammerCurrentFreq = jammerStartFreq;
  
  // Ensure frequencies are in valid range
  if(jammerStartFreq < 300.0) jammerStartFreq = 300.0;
  if(jammerEndFreq > 928.0) jammerEndFreq = 928.0;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ SWEEP MODE ]="));
  display.println(F(""));
  display.printf("Start: %.2f MHz\n", jammerStartFreq);
  display.printf("End: %.2f MHz\n", jammerEndFreq);
  display.println(F(""));
  display.println(F("Sweep enabled!"));
  display.display();
  delay(2000);
  
  Serial.printf("[JAMMER] Sweep range: %.2f - %.2f MHz\n", jammerStartFreq, jammerEndFreq);
}

//========================================================================================
// MENU.INO

// Menu.ino - Menu system and navigation with serial command support

// Serial command buffer
String serialCommand = "";

void handleButtons() {
  // Handle serial commands
  handleSerialCommands();
  
  if (millis() - lastButtonPress < buttonDebounce) return;
  
  // Special handling for LEFT button during RX mode to change visualization
  if (raw_rx == "1" && digitalRead(BTN_LEFT) == LOW) {
    lastButtonPress = millis();
    cycleRXVisualization();
    return;
  }
  
  // If in RX mode and any other button is pressed, stop RX and go back to menu
  if (raw_rx == "1") {
    if (digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
        digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      lastButtonPress = millis();
      stopRX();
      return;
    }
  }
  
  // Special handling for TX Binary menu
  if (currentMenu == MENU_TX_BINARY) {
    handleTXBinaryMenu();
    return;
  }
  
  // Normal menu navigation when not in RX mode
  if (digitalRead(BTN_UP) == LOW) {
    lastButtonPress = millis();
    handleMenuUp();
  }
  else if (digitalRead(BTN_DOWN) == LOW) {
    lastButtonPress = millis();
    handleMenuDown();
  }
  else if (digitalRead(BTN_LEFT) == LOW) {
    lastButtonPress = millis();
    handleMenuBack();
  }
  else if (digitalRead(BTN_RIGHT) == LOW) {
    lastButtonPress = millis();
    handleMenuSelect();
  }
  else if (digitalRead(BTN_SELECT) == LOW) {
    lastButtonPress = millis();
    handleMenuSelect();
  }
}

void handleSerialCommands() {
  while(Serial.available()) {
    char c = Serial.read();
    
    if(c == '\n' || c == '\r') {
      // Process command when Enter is pressed
      if(serialCommand.length() > 0) {
        processSerialCommand(serialCommand);
        serialCommand = "";
      }
    } else if(c >= 32 && c <= 126) { // Printable ASCII
      serialCommand += c;
      if(serialCommand.length() > 100) {
        serialCommand = ""; // Prevent overflow
      }
    }
  }
}

void processSerialCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  
  Serial.print(F("[SERIAL] Command: "));
  Serial.println(cmd);
  
  // Navigation commands
  if(cmd == "up") {
    Serial.println(F("[SERIAL] UP"));
    lastButtonPress = millis();
    handleMenuUp();
  }
  else if(cmd == "down") {
    Serial.println(F("[SERIAL] DOWN"));
    lastButtonPress = millis();
    handleMenuDown();
  }
  else if(cmd == "left") {
    Serial.println(F("[SERIAL] LEFT"));
    lastButtonPress = millis();
    if (raw_rx == "1") {
      cycleRXVisualization();
    } else {
      handleMenuBack();
    }
  }
  else if(cmd == "right") {
    Serial.println(F("[SERIAL] RIGHT"));
    lastButtonPress = millis();
    handleMenuSelect();
  }
  else if(cmd == "select" || cmd == "enter") {
    Serial.println(F("[SERIAL] SELECT"));
    lastButtonPress = millis();
    handleMenuSelect();
  }
  else if(cmd == "back") {
    Serial.println(F("[SERIAL] BACK"));
    lastButtonPress = millis();
    handleMenuBack();
  }
  // System commands
  else if(cmd == "help" || cmd == "?") {
    printSerialHelp();
  }
  else if(cmd == "reboot" || cmd == "restart") {
    Serial.println(F("[SERIAL] Rebooting..."));
    delay(100);
    ESP.restart();
  }
  else if(cmd == "reset") {
    Serial.println(F("[SERIAL] Factory reset..."));
    resetToFactory();
    delay(100);
    ESP.restart();
  }
  else if(cmd == "status") {
    printStatus();
  }
  else if(cmd == "info") {
    printSystemInfo();
  }
  // Operation commands
  else if(cmd == "rx") {
    Serial.println(F("[SERIAL] Starting RX mode"));
    if(cc1101APresent || cc1101BPresent) {
      startRX();
    } else {
      Serial.println(F("[SERIAL] No CC1101 detected!"));
    }
  }
  else if(cmd == "stop") {
    Serial.println(F("[SERIAL] Stopping current operation"));
    if(raw_rx == "1") stopRX();
    if(jammer_tx == "1") stopJammer();
    currentMenu = MENU_MAIN;
    updateDisplay();
  }
  else if(cmd == "menu" || cmd == "home") {
    currentMenu = MENU_MAIN;
    menuSelection = 0;
    menuOffset = 0;
    updateDisplay();
    Serial.println(F("[SERIAL] Returned to main menu"));
  }
  else if(cmd == "tesla") {
    if(cc1101APresent || cc1101BPresent) {
      Serial.println(F("[SERIAL] Sending Tesla signal"));
      sendTeslaSignal();
    } else {
      Serial.println(F("[SERIAL] No CC1101 detected!"));
    }
  }
  else if(cmd == "jammer") {
    if(cc1101APresent || cc1101BPresent) {
      Serial.println(F("[SERIAL] Starting jammer"));
      startJammer();
    } else {
      Serial.println(F("[SERIAL] No CC1101 detected!"));
    }
  }
  // Configuration commands with parameters
  else if(cmd.startsWith("freq ")) {
    float newFreq = cmd.substring(5).toFloat();
    if(newFreq >= 300 && newFreq <= 928) {
      frequency = newFreq;
      Serial.printf("[SERIAL] Frequency set to %.2f MHz\n", frequency);
      saveSettings();
    } else {
      Serial.println(F("[SERIAL] Invalid frequency (300-928 MHz)"));
    }
  }
  else if(cmd.startsWith("mod ")) {
    int newMod = cmd.substring(4).toInt();
    if(newMod >= 0 && newMod <= 4) {
      mod = newMod;
      Serial.printf("[SERIAL] Modulation set to %s\n", getModulationName(mod));
      saveSettings();
    } else {
      Serial.println(F("[SERIAL] Invalid modulation (0-4)"));
    }
  }
  else if(cmd.startsWith("led ")) {
    String ledCmd = cmd.substring(4);
    if(ledCmd == "off") {
      pixelMode = PIXEL_OFF;
      pixels.clear();
      pixels.show();
      Serial.println(F("[SERIAL] LEDs turned off"));
    }
    else if(ledCmd == "on") {
      pixelMode = PIXEL_IDLE;
      Serial.println(F("[SERIAL] LEDs turned on"));
    }
    else if(ledCmd.startsWith("bright ")) {
      int brightness = ledCmd.substring(7).toInt();
      if(brightness >= 0 && brightness <= 255) {
        ledBrightness = brightness;
        pixels.setBrightness(ledBrightness);
        Serial.printf("[SERIAL] LED brightness set to %d\n", ledBrightness);
        saveSettings();
      } else {
        Serial.println(F("[SERIAL] Invalid brightness (0-255)"));
      }
    }
  }
  else if(cmd == "") {
    // Empty command, ignore
  }
  else {
    Serial.print(F("[SERIAL] Unknown command: '"));
    Serial.print(cmd);
    Serial.println(F("'. Type 'help' for commands"));
  }
}

void printSerialHelp() {
  Serial.println(F("\n================================="));
  Serial.println(F("   TPP Badge Serial Commands"));
  Serial.println(F("================================="));
  Serial.println(F("Navigation:"));
  Serial.println(F("  up        - Navigate up"));
  Serial.println(F("  down      - Navigate down"));
  Serial.println(F("  left      - Go back (or cycle RX view)"));
  Serial.println(F("  right     - Go forward/select"));
  Serial.println(F("  select    - Select current item"));
  Serial.println(F("  back      - Go back"));
  Serial.println(F(""));
  Serial.println(F("Commands:"));
  Serial.println(F("  help      - Show this help"));
  Serial.println(F("  status    - Show current status"));
  Serial.println(F("  info      - Show system info"));
  Serial.println(F("  reboot    - Restart the badge"));
  Serial.println(F("  reset     - Factory reset"));
  Serial.println(F("  menu      - Return to main menu"));
  Serial.println(F("  rx        - Start RX mode"));
  Serial.println(F("  tesla     - Send Tesla signal"));
  Serial.println(F("  jammer    - Start jammer"));
  Serial.println(F("  stop      - Stop current operation"));
  Serial.println(F(""));
  Serial.println(F("Configuration:"));
  Serial.println(F("  freq <MHz>    - Set frequency"));
  Serial.println(F("                  Example: freq 433.92"));
  Serial.println(F("  mod <0-4>     - Set modulation"));
  Serial.println(F("                  0=2-FSK, 1=GFSK"));
  Serial.println(F("                  2=ASK/OOK, 3=4-FSK, 4=MSK"));
  Serial.println(F("  led off       - Turn LEDs off"));
  Serial.println(F("  led on        - Turn LEDs on"));
  Serial.println(F("  led bright <0-255> - Set brightness"));
  Serial.println(F("                  Example: led bright 128"));
  Serial.println(F("=================================\n"));
}

void printStatus() {
  Serial.println(F("\n=== Badge Status ==="));
  Serial.printf("Frequency: %.2f MHz\n", frequency);
  Serial.printf("Modulation: %s\n", getModulationName(mod));
  Serial.printf("Module: %c\n", activeModule == 0 ? 'A' : 'B');
  Serial.printf("RX Active: %s\n", raw_rx == "1" ? "Yes" : "No");
  Serial.printf("Jammer Active: %s\n", jammer_tx == "1" ? "Yes" : "No");
  Serial.printf("LED Mode: ");
  switch(pixelMode) {
    case PIXEL_OFF: Serial.println("OFF"); break;
    case PIXEL_IDLE: Serial.println("IDLE"); break;
    case PIXEL_RX: Serial.println("RX"); break;
    case PIXEL_TX: Serial.println("TX"); break;
    case PIXEL_JAMMER: Serial.println("JAMMER"); break;
    case PIXEL_TESLA: Serial.println("TESLA"); break;
    case PIXEL_MENU: Serial.println("MENU"); break;
    default: Serial.println("UNKNOWN"); break;
  }
  Serial.printf("LED Brightness: %d\n", ledBrightness);
  Serial.printf("Current Menu: ");
  switch(currentMenu) {
    case MENU_MAIN: Serial.println("MAIN"); break;
    case MENU_RX: Serial.println("RX"); break;
    case MENU_TX: Serial.println("TX"); break;
    case MENU_JAMMER: Serial.println("JAMMER"); break;
    case MENU_TESLA: Serial.println("TESLA"); break;
    case MENU_SETTINGS: Serial.println("SETTINGS"); break;
    case MENU_ABOUT: Serial.println("ABOUT"); break;
    case MENU_CREDITS: Serial.println("CREDITS"); break;
    case MENU_TX_BINARY: Serial.println("TX BINARY"); break;
    default: Serial.printf("%d\n", currentMenu); break;
  }
  Serial.println(F("==================\n"));
}

void printSystemInfo() {
  Serial.println(F("\n=== System Info ==="));
  Serial.printf("ESP32 Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash Size: %d KB\n", ESP.getFlashChipSize() / 1024);
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
  Serial.printf("Sketch Size: %d bytes\n", ESP.getSketchSize());
  Serial.printf("Free Sketch Space: %d bytes\n", ESP.getFreeSketchSpace());
  
  Serial.println(F("\nHardware Status:"));
  Serial.printf("Display: %s\n", display.width() > 0 ? "OK" : "FAIL");
  Serial.printf("SD Card: %s\n", sdCardPresent ? "OK" : "NO");
  Serial.printf("CC1101-A: %s\n", cc1101APresent ? "OK" : "NO");
  Serial.printf("CC1101-B: %s\n", cc1101BPresent ? "OK" : "NO");
  Serial.printf("NeoPixels: %d\n", NEOPIXEL_COUNT);
  
  Serial.println(F("\nBoot Info:"));
  Serial.printf("Reset Reason: ");
  switch(esp_reset_reason()) {
    case ESP_RST_POWERON: Serial.println("Power-on"); break;
    case ESP_RST_SW: Serial.println("Software"); break;
    case ESP_RST_PANIC: Serial.println("Panic"); break;
    case ESP_RST_WDT: Serial.println("Watchdog"); break;
    case ESP_RST_BROWNOUT: Serial.println("Brownout"); break;
    default: Serial.println("Unknown"); break;
  }
  
  uint64_t chipid = ESP.getEfuseMac();
  Serial.printf("Chip ID: %04X%08X\n", (uint16_t)(chipid>>32), (uint32_t)chipid);
  Serial.println(F("==================\n"));
}

void handleMenuUp() {
  debugPrint("Menu Up", true, false, 0);
  
  if (currentMenu == MENU_LED_BRIGHTNESS) {
    if (ledBrightness < 255) {
      ledBrightness += 5;
      if (ledBrightness > 255) ledBrightness = 255;
      pixels.setBrightness(ledBrightness);
      saveSettings();
      Serial.printf("[Menu] LED Brightness: %d\n", ledBrightness);
    }
  } else if (currentMenu == MENU_FREQUENCY_SELECT) {
    if (frequencyIndex > 0) {
      frequencyIndex--;
      frequency = commonFrequencies[frequencyIndex];
      Serial.printf("[Menu] Frequency: %.2f MHz\n", frequency);
    }
  } else {
    if (menuSelection > 0) {
      menuSelection--;
      if (menuSelection < menuOffset) {
        menuOffset = menuSelection;
      }
      Serial.printf("[Menu] Selection: %d\n", menuSelection);
    }
  }
  updateDisplay();
}

void handleMenuDown() {
  debugPrint("Menu Down", true, false, 0);
  
  if (currentMenu == MENU_LED_BRIGHTNESS) {
    if (ledBrightness > 5) {
      ledBrightness -= 5;
      if (ledBrightness < 5) ledBrightness = 5;
      pixels.setBrightness(ledBrightness);
      saveSettings();
      Serial.printf("[Menu] LED Brightness: %d\n", ledBrightness);
    }
  } else if (currentMenu == MENU_FREQUENCY_SELECT) {
    if (frequencyIndex < numFrequencies - 1) {
      frequencyIndex++;
      frequency = commonFrequencies[frequencyIndex];
      Serial.printf("[Menu] Frequency: %.2f MHz\n", frequency);
    }
  } else if (currentMenu == MENU_CREDITS) {
    // Credits auto-scroll, no manual control
  } else {
    int maxItems = getMaxMenuItems();
    if (menuSelection < maxItems - 1) {
      menuSelection++;
      if (menuSelection >= menuOffset + maxMenuItems) {
        menuOffset = menuSelection - maxMenuItems + 1;
      }
      Serial.printf("[Menu] Selection: %d\n", menuSelection);
    }
  }
  updateDisplay();
}

void handleMenuBack() {
  Serial.println(F("[Menu] Back button pressed"));
  
  switch(currentMenu) {
    case MENU_MAIN:
      Serial.println(F("[Menu] Already at main menu"));
      break;
    case MENU_RX:
    case MENU_TX:
    case MENU_JAMMER:
    case MENU_TESLA:
    case MENU_SETTINGS:
    case MENU_ABOUT:
    case MENU_CREDITS:
      currentMenu = MENU_MAIN;
      menuSelection = 0;
      menuOffset = 0;
      creditScrollPos = 0;
      Serial.println(F("[Menu] Returning to main menu"));
      break;
    case MENU_RX_CONFIG:
      currentMenu = MENU_RX;
      menuSelection = 0;
      menuOffset = 0;
      Serial.println(F("[Menu] Returning to RX menu"));
      break;
    case MENU_TX_CONFIG:
    case MENU_TX_BINARY:
      currentMenu = MENU_TX;
      menuSelection = 0;
      menuOffset = 0;
      Serial.println(F("[Menu] Returning to TX menu"));
      break;
    case MENU_DISPLAY_SETTINGS:
      currentMenu = MENU_SETTINGS;
      menuSelection = 0;
      menuOffset = 0;
      Serial.println(F("[Menu] Returning to settings menu"));
      break;
    case MENU_LED_PATTERN:
    case MENU_LED_BRIGHTNESS:
      if (currentMenu == MENU_LED_PATTERN) {
        exitPatternMenu(false);  // Don't save, restore original
      }
      currentMenu = MENU_DISPLAY_SETTINGS;
      menuSelection = 0;
      menuOffset = 0;
      break;
    case MENU_FREQUENCY_SELECT:
      currentMenu = previousMenu;
      saveSettings();
      menuSelection = 0;
      menuOffset = 0;
      Serial.printf("[Menu] Frequency saved: %.2f MHz, returning to %d\n", frequency, previousMenu);
      break;
    case MENU_TESLA_CONFIG:
      currentMenu = MENU_TESLA;
      menuSelection = 0;
      menuOffset = 0;
      Serial.println(F("[Menu] Returning to Tesla menu"));
      break;
    default:
      currentMenu = MENU_MAIN;
      menuSelection = 0;
      menuOffset = 0;
      Serial.println(F("[Menu] Unknown menu, returning to main"));
      break;
  }
  pixelMode = PIXEL_MENU;
  updateDisplay();
}

void handleMenuSelect() {
  Serial.printf("[Menu] Select pressed in menu %d, selection %d\n", currentMenu, menuSelection);
  
  switch(currentMenu) {
    case MENU_MAIN:
      switch(menuSelection) {
        case 0: 
          currentMenu = MENU_RX;
          Serial.println(F("[Menu] Opening RX menu"));
          break;
        case 1: 
          currentMenu = MENU_TX;
          Serial.println(F("[Menu] Opening TX menu"));
          break;
        case 2: 
          currentMenu = MENU_JAMMER;
          Serial.println(F("[Menu] Opening Jammer menu"));
          break;
        case 3: 
          currentMenu = MENU_TESLA;
          Serial.println(F("[Menu] Opening Tesla menu"));
          break;
        case 4: 
          currentMenu = MENU_SETTINGS;
          Serial.println(F("[Menu] Opening Settings menu"));
          break;
        case 5: 
          currentMenu = MENU_ABOUT;
          Serial.println(F("[Menu] Opening About"));
          break;
        case 6: 
          currentMenu = MENU_CREDITS;
          Serial.println(F("[Menu] Opening Credits"));
          break;
      }
      menuSelection = 0;
      menuOffset = 0;
      break;
      
    case MENU_RX:
      switch(menuSelection) {
        case 0:  
          if(cc1101APresent || cc1101BPresent) {
            Serial.println(F("[Menu] Starting RX mode"));
            startRX();
            return; 
          } else {
            debugPrint("No CC1101!", true, true, 2000);
          }
          break;
        case 1: 
          currentMenu = MENU_RX_CONFIG;
          Serial.println(F("[Menu] Opening RX config"));
          break;
        case 2:  
          Serial.println(F("[Menu] Stopping RX"));
          stopRX();
          break;
      }
      break;
      
    case MENU_TX:
      switch(menuSelection) {
        case 0:
          Serial.println(F("[Menu] Starting simple TX"));
          startTX();  // This will run demo mode if TX_DEMO_MODE is true
          break;
        case 1:
          currentMenu = MENU_TX_BINARY;
          Serial.println(F("[Menu] Opening Binary TX"));
          // Don't call handleTXBinaryMenu here, it will be called in handleButtons()
          break;
        case 2:
          Serial.println(F("[Menu] TX Last RX"));
          sendLastRX();  // This will run demo mode if TX_DEMO_MODE is true
          break;
        case 3:
          Serial.println(F("[Menu] TX from file"));
          loadTXFromFile();  // This will run demo mode if TX_DEMO_MODE is true
          break;
      }
      break;
      
    case MENU_JAMMER:
      switch(menuSelection) {
        case 0:
          if(cc1101APresent || cc1101BPresent) {
            Serial.println(F("[Menu] Starting Jammer"));
            startJammer();
            return;
          } else {
            debugPrint("No CC1101!", true, true, 2000);
          }
          break;
        case 1:
          Serial.println(F("[Menu] Stopping Jammer"));
          stopJammer();
          break;
      }
      break;
      
    case MENU_TESLA:
      switch(menuSelection) {
        case 0:
          if(cc1101APresent || cc1101BPresent) {
            Serial.println(F("[Menu] Sending Tesla signal"));
            sendTeslaSignal();
          } else {
            debugPrint("No CC1101!", true, true, 2000);
          }
          break;
        case 1:
          currentMenu = MENU_TESLA_CONFIG;
          Serial.println(F("[Menu] Opening Tesla config"));
          break;
      }
      break;
      
    case MENU_TESLA_CONFIG:
      // Toggle frequency
      if (teslaFrequency == 433.92) {
        teslaFrequency = 315.00;
      } else {
        teslaFrequency = 433.92;
      }
      saveSettings();
      Serial.printf("[Menu] Tesla frequency toggled to %.2f MHz\n", teslaFrequency);
      break;
      
    case MENU_SETTINGS:
      switch(menuSelection) {
        case 0: // Module selection
          if(cc1101APresent && cc1101BPresent) {
            activeModule = (activeModule == 0) ? 1 : 0;
            ELECHOUSE_cc1101.setModul(activeModule);
            saveSettings();
            Serial.printf("[Menu] Active module changed to %c\n", activeModule == 0 ? 'A' : 'B');
          } else {
            debugPrint("Only 1 module!", true, true, 2000);
          }
          break;
        case 1: // Frequency
          previousMenu = MENU_SETTINGS;
          currentMenu = MENU_FREQUENCY_SELECT;
          for(int i = 0; i < numFrequencies; i++) {
            if(abs(commonFrequencies[i] - frequency) < 0.01) {
              frequencyIndex = i;
              break;
            }
          }
          menuSelection = 0;
          menuOffset = 0;
          Serial.println(F("[Menu] Opening frequency selector"));
          break;
        case 2: // Modulation
          mod = (mod + 1) % 5;
          saveSettings();
          Serial.printf("[Menu] Modulation changed to %d\n", mod);
          break;
        case 3: // Display Settings
          currentMenu = MENU_DISPLAY_SETTINGS;
          menuSelection = 0;
          menuOffset = 0;
          Serial.println(F("[Menu] Opening display settings"));
          break;
        case 4: // Save Config
          saveSettings();
          Serial.println(F("[Menu] Settings saved"));
          break;
      }
      break;
      
    case MENU_RX_CONFIG:
      switch(menuSelection) {
        case 0: // Frequency
          previousMenu = MENU_RX_CONFIG;
          currentMenu = MENU_FREQUENCY_SELECT;
          for(int i = 0; i < numFrequencies; i++) {
            if(abs(commonFrequencies[i] - frequency) < 0.01) {
              frequencyIndex = i;
              break;
            }
          }
          menuSelection = 0;
          menuOffset = 0;
          Serial.println(F("[Menu] Opening frequency selector from RX config"));
          break;
      }
      break;
      
    case MENU_TX_CONFIG:
      switch(menuSelection) {
        case 0: // Frequency
          previousMenu = MENU_TX_CONFIG;
          currentMenu = MENU_FREQUENCY_SELECT;
          for(int i = 0; i < numFrequencies; i++) {
            if(abs(commonFrequencies[i] - frequency) < 0.01) {
              frequencyIndex = i;
              break;
            }
          }
          menuSelection = 0;
          menuOffset = 0;
          Serial.println(F("[Menu] Opening frequency selector from TX config"));
          break;
      }
      break;
      
    case MENU_DISPLAY_SETTINGS:
      switch(menuSelection) {
        case 0:
          currentMenu = MENU_LED_PATTERN;
          Serial.println(F("[Menu] Opening LED pattern selector"));
          break;
        case 1:
          currentMenu = MENU_LED_BRIGHTNESS;
          Serial.println(F("[Menu] Opening LED brightness control"));
          break;
      }
      menuSelection = 0;
      menuOffset = 0;
      break;
      
    case MENU_LED_PATTERN:
      exitPatternMenu(true);  // Save the previewed pattern
      saveSettings();
      currentMenu = MENU_DISPLAY_SETTINGS;
      menuSelection = 0;
      menuOffset = 0;
      break;
      
    case MENU_FREQUENCY_SELECT:
      frequency = commonFrequencies[frequencyIndex];
      saveSettings();
      currentMenu = previousMenu;
      menuSelection = 0;
      menuOffset = 0;
      Serial.printf("[Menu] Frequency selected: %.2f MHz\n", frequency);
      break;
  }
  
  updateDisplay();
}

int getMaxMenuItems() {
  switch(currentMenu) {
    case MENU_MAIN: return 7;
    case MENU_RX: return 4;
    case MENU_TX: return 4;  // Simple TX, Binary TX, TX Last RX, TX from File
    case MENU_JAMMER: return 3;
    case MENU_TESLA: return 2;
    case MENU_SETTINGS: return 5;
    case MENU_DISPLAY_SETTINGS: return 3;
    case MENU_LED_PATTERN: return 35;
    case MENU_RX_CONFIG: return 4;
    case MENU_TX_CONFIG: return 4;
    case MENU_TX_BINARY: return 4;  // Added for binary TX submenu
    default: return 1;
  }
}

// Handler for TX Binary submenu
void handleTXBinaryMenu() {
  static int binaryOption = 0;
  static unsigned long lastBinaryDraw = 0;
  
  // Only redraw if needed
  if(millis() - lastBinaryDraw > 100) {
    lastBinaryDraw = millis();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    display.println(F("=[ BINARY TX ]="));
    display.println(F(""));
    
    const char* options[] = {
      "Manual Input",
      "Test Pattern",
      "From File",
      "Back"
    };
    
    for(int i = 0; i < 4; i++) {
      if(i == binaryOption) {
        display.fillRect(0, 16 + i*10, 128, 10, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
      } else {
        display.setTextColor(SH110X_WHITE);
      }
      
      display.setCursor(2, 17 + i*10);
      display.print(options[i]);
    }
    
    display.setTextColor(SH110X_WHITE);
    display.display();
  }
  
  // Handle input
  if(millis() - lastButtonPress > buttonDebounce) {
    if(digitalRead(BTN_UP) == LOW) {
      lastButtonPress = millis();
      binaryOption = (binaryOption - 1 + 4) % 4;
      Serial.printf("[TX Binary] Option: %d\n", binaryOption);
    }
    else if(digitalRead(BTN_DOWN) == LOW) {
      lastButtonPress = millis();
      binaryOption = (binaryOption + 1) % 4;
      Serial.printf("[TX Binary] Option: %d\n", binaryOption);
    }
    else if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      lastButtonPress = millis();
      
      switch(binaryOption) {
        case 0: // Manual Input
          Serial.println(F("[TX Binary] Manual Input selected"));
          sendBinaryData();
          currentMenu = MENU_TX;  // Return to TX menu after
          binaryOption = 0;  // Reset selection
          break;
          
        case 1: // Test Pattern
          Serial.println(F("[TX Binary] Test Pattern selected"));
          sendTestPattern();
          currentMenu = MENU_TX;  // Return to TX menu after
          binaryOption = 0;  // Reset selection
          break;
          
        case 2: // From File
          Serial.println(F("[TX Binary] From File selected"));
          loadTXFromFile();
          currentMenu = MENU_TX;  // Return to TX menu after
          binaryOption = 0;  // Reset selection
          break;
          
        case 3: // Back
          Serial.println(F("[TX Binary] Back selected"));
          currentMenu = MENU_TX;
          binaryOption = 0;  // Reset selection
          break;
      }
      
      menuSelection = 0;
      menuOffset = 0;
      updateDisplay();
    }
    else if(digitalRead(BTN_LEFT) == LOW) {
      lastButtonPress = millis();
      Serial.println(F("[TX Binary] Left/Back pressed"));
      currentMenu = MENU_TX;
      binaryOption = 0;  // Reset selection
      menuSelection = 0;
      menuOffset = 0;
      updateDisplay();
    }
  }
}

//==================================================================
// PIXELS.INO

// Pixels.ino - NeoPixel LED animation functions

// Animation variables
unsigned long lastPixelUpdate = 0;
uint8_t pixelHue = 0;
uint8_t fireAnimation[NEOPIXEL_COUNT];
uint8_t breathePhase = 0;
int8_t breatheDirection = 1;
bool lavaInit = false;

// Pattern-specific variables
uint8_t plasmaTime = 0;
uint8_t wavePosition = 0;
float sparkleDecay[NEOPIXEL_COUNT];
uint8_t meteorPosition = 0;
uint8_t twinkleState[NEOPIXEL_COUNT];
uint8_t stripeOffset = 0;
uint8_t theaterChaseQ = 0;
uint8_t runningLightsPos = 0;
float bouncingBallPos[3] = {0, 10, 20};
float bouncingBallVel[3] = {1.0, 1.5, 2.0};
uint8_t heartbeatPhase = 0;
uint8_t strobeCount = 0;
uint8_t cylonPos = 0;
int8_t cylonDir = 1;
uint8_t snowSparkleTime = 0;
uint8_t pacificaTime = 0;
uint8_t lavaHeat[NEOPIXEL_COUNT] = {0};  // Initialize to ensure it's not garbage

// Helper function declarations
uint8_t qsub8(uint8_t x, uint8_t b);
uint8_t qadd8(uint8_t i, uint8_t j);
int8_t random8(uint8_t min = 0, uint8_t max = 255);

// Forward declarations for animations
void breatheAnimation(uint8_t r, uint8_t g, uint8_t b);
void rainbowCycle();
void scannerAnimation(uint8_t r, uint8_t g, uint8_t b);
void sparkleAnimation();
void fireEffect();
void oceanWave();
void matrixRain();
void superGayAnimation();
void solidRed();
void solidGreen();
void solidBlue();
void solidWhite();
void solidMagenta();
void solidCyan();
void solidYellow();
void solidOrange();
void neonPulse();
void plasmaAnimation();
void meteorRain();
void twinkleStars();
void vaporwave();
void policeStrobe();
void pirateShip();
void theaterChase();
void runningLights();
void bouncingBalls();
void heartbeat();
void strobeEffect();
void cylonBounce();
void snowSparkle();
void pacifica();
void lavaLamp();
void lightning();
void confetti();
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
        case PATTERN_SUPERGAY:
          superGayAnimation();
          break;
        case PATTERN_SOLID_RED:
          solidRed();
          break;
        case PATTERN_SOLID_GREEN:
          solidGreen();
          break;
        case PATTERN_SOLID_BLUE:
          solidBlue();
          break;
        case PATTERN_SOLID_WHITE:
          solidWhite();
          break;
        case PATTERN_SOLID_MAGENTA:
          solidMagenta();
          break;
        case PATTERN_SOLID_CYAN:
          solidCyan();
          break;
        case PATTERN_SOLID_YELLOW:
          solidYellow();
          break;
        case PATTERN_SOLID_ORANGE:
          solidOrange();
          break;
        case PATTERN_NEON:
          neonPulse();
          break;
        case PATTERN_PLASMA:
          plasmaAnimation();
          break;
        case PATTERN_METEOR:
          meteorRain();
          break;
        case PATTERN_TWINKLE:
          twinkleStars();
          break;
        case PATTERN_VAPORWAVE:
          vaporwave();
          break;
        case PATTERN_POLICE:
          policeStrobe();
          break;
        case PATTERN_PIRATE:
          pirateShip();
          break;
        case PATTERN_THEATER:
          theaterChase();
          break;
        case PATTERN_RUNNING:
          runningLights();
          break;
        case PATTERN_BOUNCING:
          bouncingBalls();
          break;
        case PATTERN_HEARTBEAT:
          heartbeat();
          break;
        case PATTERN_STROBE:
          strobeEffect();
          break;
        case PATTERN_CYLON:
          cylonBounce();
          break;
        case PATTERN_SNOW:
          snowSparkle();
          break;
        case PATTERN_PACIFICA:
          pacifica();
          break;
        case PATTERN_LAVA:
          lavaLamp();
          break;
        case PATTERN_LIGHTNING:
          lightning();
          break;
        case PATTERN_CONFETTI:
          confetti();
          break;
        case PATTERN_OFF:
          pixels.clear();
          break;
      }
      break;
      
    case PIXEL_RX:
      scannerAnimation(0, 255, 0);
      break;
      
    case PIXEL_TX:
      pulseAnimation(255, 0, 0);
      break;
      
    case PIXEL_JAMMER:
      chaosAnimation();
      break;
      
    case PIXEL_TESLA:
      lightningAnimation();
      break;
      
    case PIXEL_MENU:
      rainbowCycle();
      break;
  }
  
  pixels.show();
}

void breatheAnimation(uint8_t r, uint8_t g, uint8_t b) {
  float breath = (sin(breathePhase * 0.0174533) + 1.0) / 2.0;
  uint8_t brightness = 20 + (breath * 235);
  
  uint32_t color = pixels.Color((r * brightness) / 255, 
                                (g * brightness) / 255, 
                                (b * brightness) / 255);
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, color);
  }
  
  breathePhase += 3;
  if(breathePhase >= 360) breathePhase = 0;
}

void fireEffect() {
  // Cool down every cell a little
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    fireAnimation[i] = qsub8(fireAnimation[i], random8(0, ((55 * 10) / NEOPIXEL_COUNT) + 2));
  }
  
  // Heat from each cell drifts UP (higher index = up)
  for(int k = NEOPIXEL_COUNT - 1; k >= 2; k--) {
    fireAnimation[k] = (fireAnimation[k - 1] + fireAnimation[k - 2] + fireAnimation[k - 2]) / 3;
  }
  
  // Randomly ignite new sparks at the BOTTOM (index 0)
  if(random8() < 120) {
    int y = random8(0, 3);  // Only ignite at bottom 3 LEDs
    fireAnimation[y] = qadd8(fireAnimation[y], random8(160, 255));
  }
  
  // Convert heat to LED colors
  for(int j = 0; j < NEOPIXEL_COUNT; j++) {
    uint8_t heat = fireAnimation[j];
    
    if(heat < 85) {
      pixels.setPixelColor(j, pixels.Color(heat * 3, 0, 0));
    } else if(heat < 170) {
      pixels.setPixelColor(j, pixels.Color(255, (heat - 85) * 3, 0));
    } else {
      pixels.setPixelColor(j, pixels.Color(255, 255, (heat - 170) * 3));
    }
  }
}

void superGayAnimation() {
  static uint16_t gayOffset = 0;
  
  uint32_t prideColors[] = {
    pixels.Color(255, 0, 0),
    pixels.Color(255, 127, 0),
    pixels.Color(255, 255, 0),
    pixels.Color(0, 255, 0),
    pixels.Color(0, 0, 255),
    pixels.Color(75, 0, 130),
    pixels.Color(148, 0, 211),
    pixels.Color(255, 20, 147)
  };
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    int colorIndex = ((i * 8 / NEOPIXEL_COUNT) + (gayOffset / 20)) % 8;
    pixels.setPixelColor(i, prideColors[colorIndex]);
    
    if(random(100) < 10) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
  }
  
  gayOffset += 5;
}

// Solid colors
void solidRed() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
}

void solidGreen() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  }
}

void solidBlue() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
  }
}

void solidWhite() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
}

void solidMagenta() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 255));
  }
}

void solidCyan() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 255));
  }
}

void solidYellow() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 0));
  }
}

void solidOrange() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 100, 0));
  }
}

void theaterChase() {
  pixels.clear();
  
  for(int i = 0; i < NEOPIXEL_COUNT; i += 3) {
    if(i + theaterChaseQ < NEOPIXEL_COUNT) {
      pixels.setPixelColor(i + theaterChaseQ, pixels.Color(255, 255, 255));
    }
  }
  
  theaterChaseQ++;
  if(theaterChaseQ >= 3) theaterChaseQ = 0;
}

void runningLights() {
  runningLightsPos++;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    float level = sin((i + runningLightsPos) * 0.5) * 127 + 128;
    pixels.setPixelColor(i, pixels.Color(level, 0, 0));
  }
}

void bouncingBalls() {
  pixels.clear();
  
  uint32_t colors[] = {
    pixels.Color(255, 0, 0),
    pixels.Color(0, 255, 0),
    pixels.Color(0, 0, 255)
  };
  
  for(int i = 0; i < 3; i++) {
    bouncingBallPos[i] += bouncingBallVel[i];
    
    if(bouncingBallPos[i] >= NEOPIXEL_COUNT - 1 || bouncingBallPos[i] <= 0) {
      bouncingBallVel[i] = -bouncingBallVel[i];
    }
    
    int pos = round(bouncingBallPos[i]);
    if(pos >= 0 && pos < NEOPIXEL_COUNT) {
      pixels.setPixelColor(pos, colors[i]);
    }
  }
}

void heartbeat() {
  float val = (exp(sin(heartbeatPhase * 0.0174533)) - 0.36787944) * 108.0;
  uint8_t brightness = val;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(brightness, 0, 0));
  }
  
  heartbeatPhase += 2;
  if(heartbeatPhase >= 360) heartbeatPhase = 0;
}

void strobeEffect() {
  if(strobeCount < 10) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
  } else {
    pixels.clear();
  }
  
  strobeCount++;
  if(strobeCount > 20) strobeCount = 0;
}

void cylonBounce() {
  pixels.clear();
  
  for(int i = 0; i < 5; i++) {
    int pos = cylonPos - (i * cylonDir);
    if(pos >= 0 && pos < NEOPIXEL_COUNT) {
      int brightness = 255 - (i * 50);
      pixels.setPixelColor(pos, pixels.Color(brightness, 0, 0));
    }
  }
  
  cylonPos += cylonDir;
  if(cylonPos >= NEOPIXEL_COUNT - 1 || cylonPos <= 0) {
    cylonDir = -cylonDir;
  }
}

void snowSparkle() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(10, 10, 10));
  }
  
  if(snowSparkleTime % 5 == 0) {
    int pos = random(NEOPIXEL_COUNT);
    pixels.setPixelColor(pos, pixels.Color(255, 255, 255));
  }
  
  snowSparkleTime++;
}

void pacifica() {
  static uint16_t sTime = 0;
  sTime++;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    // Create waves of blues and greens
    uint8_t wave1 = (sin((i * 3 + sTime) * 0.1) + 1) * 100;
    uint8_t wave2 = (sin((i * 5 - sTime) * 0.05) + 1) * 50;
    
    uint8_t r = wave2 / 8;
    uint8_t g = wave1 / 4 + wave2 / 4;
    uint8_t b = wave1 + wave2;
    
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}

void lavaLamp() {
  static bool lavaInit = false;
  
  // Initialize with some random heat values
  if(!lavaInit) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      lavaHeat[i] = random8(50, 150);  // Start with some heat
    }
    lavaInit = true;
  }
  
  // Cool down every cell just a tiny bit
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    if(lavaHeat[i] > 0) {
      lavaHeat[i] = qsub8(lavaHeat[i], random8(0, 2));  // Much slower cooling
    }
  }
  
  // Heat drifts up and diffuses
  for(int k = NEOPIXEL_COUNT - 1; k >= 2; k--) {
    lavaHeat[k] = (lavaHeat[k - 1] + lavaHeat[k - 2] + lavaHeat[k - 2]) / 3;
  }
  
  // Create new bubbles more frequently
  if(random8() < 80) {  // Increased chance
    int y = random8(NEOPIXEL_COUNT / 2);  // Start bubbles in lower half
    lavaHeat[y] = qadd8(lavaHeat[y], random8(180, 255));
  }
  
  // Add medium bubbles too
  if(random8() < 50) {
    int y = random8(NEOPIXEL_COUNT);
    lavaHeat[y] = qadd8(lavaHeat[y], random8(100, 180));
  }
  
  // Convert heat to LED colors
  for(int j = 0; j < NEOPIXEL_COUNT; j++) {
    uint8_t temperature = lavaHeat[j];
    
    if(temperature > 20) {  // Only show if there's some heat
      if(temperature < 85) {
        // Black to dark red
        pixels.setPixelColor(j, pixels.Color(temperature * 3, 0, 0));
      } else if(temperature < 170) {
        // Dark red to bright orange
        pixels.setPixelColor(j, pixels.Color(255, (temperature - 85) * 3, 0));
      } else {
        // Orange to yellow
        pixels.setPixelColor(j, pixels.Color(255, 200, (temperature - 170) * 3));
      }
    } else {
      // Very low heat = dark/black
      pixels.setPixelColor(j, pixels.Color(temperature, 0, 0));
    }
  }
}

void lightning() {
  static uint8_t flashCount = 0;
  static uint8_t dimmer = 1;
  
  if(flashCount == 0) {
    dimmer = 5;
    if(random8() < 16) {
      flashCount = random8(3, 12);
    }
  }
  
  pixels.clear();
  
  if(flashCount) {
    if(dimmer) {
      for(int i = 0; i < NEOPIXEL_COUNT; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 255, 255));
      }
      dimmer--;
    } else {
      flashCount--;
      dimmer = 5;
    }
  } else {
    // Ambient blue between strikes
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      if(random8() < 2) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 48));
      }
    }
  }
}

void confetti() {
  fadeToBlackBy(10);
  
  int pos = random16(NEOPIXEL_COUNT);
  uint8_t hue = random8();
  pixels.setPixelColor(pos, pixels.ColorHSV(hue * 256, 200, 255));
}

void neonPulse() {
  static uint8_t neonPhase = 0;
  static uint8_t colorMode = 0;
  
  float pulse = (sin(neonPhase * 0.0174533) + 1.0) / 2.0;
  
  uint32_t neonColors[] = {
    pixels.Color(255, 0, 255),
    pixels.Color(0, 255, 255),
    pixels.Color(255, 0, 128),
    pixels.Color(128, 0, 255)
  };
  
  uint32_t color1 = neonColors[colorMode % 4];
  uint32_t color2 = neonColors[(colorMode + 1) % 4];
  
  uint8_t r1 = (color1 >> 16) & 0xFF;
  uint8_t g1 = (color1 >> 8) & 0xFF;
  uint8_t b1 = color1 & 0xFF;
  
  uint8_t r2 = (color2 >> 16) & 0xFF;
  uint8_t g2 = (color2 >> 8) & 0xFF;
  uint8_t b2 = color2 & 0xFF;
  
  uint8_t r = r1 + (pulse * (r2 - r1));
  uint8_t g = g1 + (pulse * (g2 - g1));
  uint8_t b = b1 + (pulse * (b2 - b1));
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  
  neonPhase += 3;
  if(neonPhase >= 360) {
    neonPhase = 0;
    colorMode++;
  }
}

void plasmaAnimation() {
  plasmaTime++;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint8_t x = i;
    
    int plasma = 128 + (127 * sin((x / 2.0) + (plasmaTime / 10.0)));
    plasma += 128 + (127 * sin((x / 3.0) - (plasmaTime / 8.0)));
    plasma += 128 + (127 * sin((x / 5.0) + (plasmaTime / 12.0)));
    plasma /= 3;
    
    uint8_t hue = map(plasma, 0, 255, 0, 255);
    pixels.setPixelColor(i, pixels.ColorHSV(hue * 256));
  }
}

void meteorRain() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint32_t oldColor = pixels.getPixelColor(i);
    uint8_t r = (oldColor >> 16) & 0xFF;
    uint8_t g = (oldColor >> 8) & 0xFF;
    uint8_t b = oldColor & 0xFF;
    
    r = (r * 240) / 256;
    g = (g * 240) / 256;
    b = (b * 240) / 256;
    
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  
  int meteorSize = 5;
  for(int j = 0; j < meteorSize; j++) {
    if((meteorPosition - j >= 0) && (meteorPosition - j < NEOPIXEL_COUNT)) {
      int brightness = 255 - (j * 40);
      pixels.setPixelColor(meteorPosition - j, pixels.Color(brightness, brightness, brightness));
    }
  }
  
  meteorPosition++;
  if(meteorPosition >= NEOPIXEL_COUNT + meteorSize) {
    meteorPosition = 0;
    
    if(random(10) < 3) {
      meteorPosition = -meteorSize * 2;
    }
  }
}

void twinkleStars() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    if(twinkleState[i] == 0) {
      if(random(100) < 5) {
        twinkleState[i] = 255;
      }
    } else {
      twinkleState[i] = (twinkleState[i] * 9) / 10;
      if(twinkleState[i] < 10) twinkleState[i] = 0;
    }
    
    uint8_t brightness = twinkleState[i];
    pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness * 0.8));
  }
}

void vaporwave() {
  static uint8_t vaporOffset = 0;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    float wave = sin((i + vaporOffset) * 0.2);
    
    if(wave > 0.3) {
      pixels.setPixelColor(i, pixels.Color(255, 20, 147));
    } else if(wave > -0.3) {
      pixels.setPixelColor(i, pixels.Color(138, 43, 226));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 255, 255));
    }
    
    if(random(200) < 2) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
  }
  
  vaporOffset++;
}

void policeStrobe() {
  static uint8_t strobePhase = 0;
  static unsigned long lastStrobe = 0;
  
  if(millis() - lastStrobe > 50) {
    lastStrobe = millis();
    strobePhase++;
    
    pixels.clear();
    
    if(strobePhase % 8 < 2) {
      for(int i = 0; i < NEOPIXEL_COUNT / 2; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      }
    } else if(strobePhase % 8 >= 4 && strobePhase % 8 < 6) {
      for(int i = NEOPIXEL_COUNT / 2; i < NEOPIXEL_COUNT; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
      }
    }
  }
}

void pirateShip() {
  static int shipPos = 0;
  static uint8_t wavePhase = 0;
  
  pixels.clear();
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint8_t waveHeight = 30 + (25 * sin((i * 0.3) + (wavePhase * 0.05)));
    pixels.setPixelColor(i, pixels.Color(0, 0, waveHeight));
  }
  
  if(shipPos >= 0 && shipPos < NEOPIXEL_COUNT) {
    pixels.setPixelColor(shipPos, pixels.Color(139, 0, 0));
    if(shipPos > 0) pixels.setPixelColor(shipPos - 1, pixels.Color(100, 0, 0));
    if(shipPos < NEOPIXEL_COUNT - 1) pixels.setPixelColor(shipPos + 1, pixels.Color(100, 0, 0));
    
    if(shipPos > 1) pixels.setPixelColor(shipPos - 2, pixels.Color(255, 255, 255));
    if(shipPos > 2) pixels.setPixelColor(shipPos - 3, pixels.Color(255, 0, 0));
  }
  
  for(int j = 1; j < 5; j++) {
    int wakePos = shipPos + j + 1;
    if(wakePos >= 0 && wakePos < NEOPIXEL_COUNT) {
      uint8_t wakeBright = 100 - (j * 20);
      pixels.setPixelColor(wakePos, pixels.Color(wakeBright, wakeBright, 255));
    }
  }
  
  shipPos--;
  if(shipPos < -10) shipPos = NEOPIXEL_COUNT + 10;
  
  wavePhase += 2;
}

void scannerAnimation(uint8_t r, uint8_t g, uint8_t b) {
  static int scanPos = 0;
  static int scanDir = 1;
  
  pixels.clear();
  
  pixels.setPixelColor(scanPos, pixels.Color(r, g, b));
  
  for(int i = 1; i <= 5; i++) {
    int brightness = 255 - (i * 40);
    if(brightness < 0) brightness = 0;
    
    int trailPos1 = scanPos - (i * scanDir);
    int trailPos2 = scanPos + (i * scanDir);
    
    if(trailPos1 >= 0 && trailPos1 < NEOPIXEL_COUNT) {
      pixels.setPixelColor(trailPos1, pixels.Color((r * brightness) / 255,
                                                   (g * brightness) / 255,
                                                   (b * brightness) / 255));
    }
    if(trailPos2 >= 0 && trailPos2 < NEOPIXEL_COUNT && i <= 2) {
      pixels.setPixelColor(trailPos2, pixels.Color((r * brightness) / 255,
                                                   (g * brightness) / 255,
                                                   (b * brightness) / 255));
    }
  }
  
  scanPos += scanDir;
  if(scanPos >= NEOPIXEL_COUNT - 1 || scanPos <= 0) {
    scanDir = -scanDir;
  }
}

void sparkleAnimation() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    if(sparkleDecay[i] > 0) {
      sparkleDecay[i] *= 0.9;
      if(sparkleDecay[i] < 1) sparkleDecay[i] = 0;
      
      uint8_t brightness = sparkleDecay[i];
      pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness));
    }
  }
  
  for(int j = 0; j < 3; j++) {
    if(random(100) < 30) {
      int pos = random(NEOPIXEL_COUNT);
      sparkleDecay[pos] = 255;
    }
  }
}

void oceanWave() {
  wavePosition += 2;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    float wave1 = sin((i * 0.2) + (wavePosition * 0.02));
    float wave2 = sin((i * 0.3) + (wavePosition * 0.03));
    float combinedWave = (wave1 + wave2) / 2;
    
    uint8_t blue = 100 + (80 * combinedWave);
    uint8_t green = 20 + (40 * combinedWave);
    uint8_t white = 0;
    
    // Add foam on wave peaks
    if(combinedWave > 0.8) {
      white = (combinedWave - 0.8) * 255;
    }
    
    pixels.setPixelColor(i, pixels.Color(white, green + white, blue + white));
  }
}

void matrixRain() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint32_t color = pixels.getPixelColor(i);
    uint8_t g = (color >> 8) & 0xFF;
    
    if(g > 0) {
      g = (g * 230) / 256;
      pixels.setPixelColor(i, pixels.Color(0, g, 0));
    }
  }
  
  for(int j = 0; j < 3; j++) {
    if(random(10) < 4) {
      int pos = random(NEOPIXEL_COUNT);
      uint8_t brightness = random(128, 255);
      pixels.setPixelColor(pos, pixels.Color(0, brightness, 0));
    }
  }
}

void rainbowCycle() {
  pixelHue += 256;
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    int pixelHueOffset = (i * 65536L / NEOPIXEL_COUNT) + pixelHue;
    pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHueOffset)));
  }
}

void pulseAnimation(uint8_t r, uint8_t g, uint8_t b) {
  static uint8_t pulsePhase = 0;
  
  pulsePhase += 5;
  
  uint8_t intensity = (sin(pulsePhase * 0.0174533) + 1) * 127;
  uint32_t color = pixels.Color((r * intensity) / 255,
                                (g * intensity) / 255,
                                (b * intensity) / 255);
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, color);
  }
}

void chaosAnimation() {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    if(random(10) < 3) {
      pixels.setPixelColor(i, pixels.Color(random(256), random(256), random(256)));
    } else {
      uint32_t oldColor = pixels.getPixelColor(i);
      uint8_t r = ((oldColor >> 16) & 0xFF) * 0.9;
      uint8_t g = ((oldColor >> 8) & 0xFF) * 0.9;
      uint8_t b = (oldColor & 0xFF) * 0.9;
      pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
  }
}

void lightningAnimation() {
  static unsigned long lastStrike = 0;
  
  if(millis() - lastStrike > random(100, 500)) {
    lastStrike = millis();
    
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
    pixels.show();
    delay(50);
    
    for(int fade = 255; fade > 0; fade -= 15) {
      for(int i = 0; i < NEOPIXEL_COUNT; i++) {
        pixels.setPixelColor(i, pixels.Color(fade * 0.7, fade * 0.7, fade));
      }
      pixels.show();
      delay(10);
    }
  } else {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      if(random(100) < 5) {
        uint8_t flicker = random(50, 150);
        pixels.setPixelColor(i, pixels.Color(0, 0, flicker));
      } else {
        pixels.setPixelColor(i, 0);
      }
    }
  }
}

void successAnimation() {
  Serial.println(F("[LED] Success animation"));
  
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
  
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();
    delay(30);
  }
  
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
  int ledsToLight = map(rssi, -120, -30, 0, NEOPIXEL_COUNT);
  ledsToLight = constrain(ledsToLight, 0, NEOPIXEL_COUNT);
  
  pixels.clear();
  
  for(int i = 0; i < ledsToLight; i++) {
    uint32_t color;
    if(i < NEOPIXEL_COUNT * 0.3) {
      color = pixels.Color(255, 0, 0);
    } else if(i < NEOPIXEL_COUNT * 0.7) {
      color = pixels.Color(255, 255, 0);
    } else {
      color = pixels.Color(0, 255, 0);
    }
    pixels.setPixelColor(i, color);
  }
  
  pixels.show();
}

void teslaSuccessAnimation() {
  Serial.println(F("[LED] Tesla success animation"));
  
  for(int flash = 0; flash < 5; flash++) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 255));
    }
    pixels.show();
    delay(50);
    
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 50));
    }
    pixels.show();
    delay(50);
  }
  
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

// Helper function implementations
uint8_t qsub8(uint8_t x, uint8_t b) {
  if(x < b) return 0;
  return x - b;
}

uint8_t qadd8(uint8_t i, uint8_t j) {
  unsigned int t = i + j;
  if(t > 255) return 255;
  return t;
}

int8_t random8(uint8_t min, uint8_t max) {
  if(min >= max) return min;
  return random(min, max + 1);
}

uint8_t sin8(uint8_t theta) {
  uint16_t angle = theta * 2;
  return (sin(angle * 0.0174533) + 1) * 127.5;
}

uint8_t cos8(uint8_t theta) {
  uint16_t angle = theta * 2;
  return (cos(angle * 0.0174533) + 1) * 127.5;
}

uint8_t scale8(uint8_t i, uint8_t scale) {
  return ((uint16_t)i * scale) >> 8;
}

void fadeToBlackBy(uint8_t fadeBy) {
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    uint32_t oldColor = pixels.getPixelColor(i);
    uint8_t r = (oldColor >> 16) & 0xFF;
    uint8_t g = (oldColor >> 8) & 0xFF;
    uint8_t b = oldColor & 0xFF;
    
    r = scale8(r, 255 - fadeBy);
    g = scale8(g, 255 - fadeBy);
    b = scale8(b, 255 - fadeBy);
    
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}

uint16_t random16(uint16_t max) {
  return random(max);
}

//=============================================================
// RX.INO

// RX.ino - RX mode with waterfall display and reactive NeoPixels

// RX state variables
bool rxActive = false;
unsigned long rxStartTime = 0;
unsigned long lastSignalTime = 0;
int signalCount = 0;
unsigned long lastRXRefresh = 0;  // Periodic refresh

// Waterfall display
#define WATERFALL_WIDTH 128
#define WATERFALL_HEIGHT 50
uint8_t waterfallBuffer[WATERFALL_HEIGHT][WATERFALL_WIDTH / 8];

// Signal processing
int currentRSSI = -100;
int instantRSSI = -100;  // Immediate signal detection
int baselineRSSI = -100;
bool baselineSet = false;
unsigned long baselineStartTime = 0;
int baselineSamples = 0;
long baselineSum = 0;

// Global RSSI variables (used elsewhere in code)
float avgRSSI = -100;
int peakRSSI = -120;

// NeoPixel animation for RX
uint8_t rxPixelHue = 0;
unsigned long lastRXPixelUpdate = 0;
int signalPixelBrightness = 0;

// CC1101 Direct SPI functions
byte readCC1101Register(byte addr) {
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  while(digitalRead(misoPin));
  
  SPI.transfer(addr | 0x80);
  byte value = SPI.transfer(0x00);
  
  digitalWrite(csPin, HIGH);
  return value;
}

void writeCC1101Register(byte addr, byte value) {
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  while(digitalRead(misoPin));
  
  SPI.transfer(addr);
  SPI.transfer(value);
  
  digitalWrite(csPin, HIGH);
}

void strobeCC1101(byte cmd) {
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  while(digitalRead(misoPin));
  
  SPI.transfer(cmd);
  
  digitalWrite(csPin, HIGH);
}

byte readCC1101Status(byte addr) {
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  while(digitalRead(misoPin));
  
  SPI.transfer(addr | 0xC0);  // Status register burst read
  byte value = SPI.transfer(0x00);
  
  digitalWrite(csPin, HIGH);
  return value;
}

void refreshRXState() {
  // Check current state
  byte marcState = readCC1101Status(0x35);  // MARCSTATE register
  
  // If not in RX state (0x0D), put it back
  if((marcState & 0x1F) != 0x0D) {
    Serial.printf("[RX] CC1101 not in RX (state: 0x%02X), resetting...\n", marcState);
    
    // Flush RX FIFO
    strobeCC1101(0x3A); // SFRX
    
    // Go back to RX
    strobeCC1101(0x34); // SRX
    delay(1);
  }
}

void startRX() {
  Serial.println(F("[RX] Starting RX mode"));
  
  // Check which module to use
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  // Use whichever module is available
  if(activeModule == 0 && !cc1101APresent) {
    activeModule = 1;
    Serial.println(F("[RX] Module A not present, switching to B"));
  } else if(activeModule == 1 && !cc1101BPresent) {
    activeModule = 0;
    Serial.println(F("[RX] Module B not present, switching to A"));
  }
  
  Serial.printf("[RX] Using module: %c\n", activeModule == 0 ? 'A' : 'B');
  
  // Initialize variables
  raw_rx = "1";
  rxActive = true;
  rxStartTime = millis();
  lastRXRefresh = millis();
  signalCount = 0;
  currentRSSI = -100;
  instantRSSI = -100;
  baselineRSSI = -100;
  baselineSet = false;
  baselineStartTime = millis();
  baselineSamples = 0;
  baselineSum = 0;
  avgRSSI = -100;
  peakRSSI = -120;
  
  // Clear waterfall buffer
  memset(waterfallBuffer, 0, sizeof(waterfallBuffer));
  
  // Show initializing message
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(F("Calibrating..."));
  display.println(F("Please wait"));
  display.printf("Module: %c", activeModule == 0 ? 'A' : 'B');
  display.display();
  
  // Initialize SPI for the selected module
  int sckPin = (activeModule == 0) ? CC1101_CLK_A : CC1101_CLK_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  int mosiPin = (activeModule == 0) ? CC1101_MOSI_A : CC1101_MOSI_B;
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  
  SPI.begin(sckPin, misoPin, mosiPin, csPin);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  
  // Reset CC1101
  strobeCC1101(0x30); // SRES
  delay(10);
  
  // Configure frequency based on setting
  byte freq2, freq1, freq0;
  if(frequency >= 433 && frequency <= 435) {
    freq2 = 0x10; freq1 = 0xB0; freq0 = 0x7A; // ~433.92 MHz
  } else if(frequency >= 314 && frequency <= 316) {
    freq2 = 0x0C; freq1 = 0x4E; freq0 = 0xC4; // ~315 MHz
  } else if(frequency >= 867 && frequency <= 869) {
    freq2 = 0x21; freq1 = 0x65; freq0 = 0x6A; // ~868 MHz
  } else if(frequency >= 914 && frequency <= 916) {
    freq2 = 0x23; freq1 = 0x31; freq0 = 0x3B; // ~915 MHz
  } else {
    freq2 = 0x10; freq1 = 0xB0; freq0 = 0x7A; // Default to 433.92
  }
  
  writeCC1101Register(0x0D, freq2); // FREQ2
  writeCC1101Register(0x0E, freq1); // FREQ1  
  writeCC1101Register(0x0F, freq0); // FREQ0
  
  // Configure for basic RX with selected modulation
  writeCC1101Register(0x10, 0x00); // MDMCFG4 - Max BW
  writeCC1101Register(0x11, 0x22); // MDMCFG3
  
  // Set modulation based on mod variable
  byte mdmcfg2 = 0x00;
  switch(mod) {
    case 0: mdmcfg2 = 0x00; break; // 2-FSK
    case 1: mdmcfg2 = 0x10; break; // GFSK
    case 2: mdmcfg2 = 0x30; break; // ASK/OOK
    case 3: mdmcfg2 = 0x40; break; // 4-FSK
    case 4: mdmcfg2 = 0x70; break; // MSK
  }
  writeCC1101Register(0x12, mdmcfg2); // MDMCFG2 - Modulation
  
  writeCC1101Register(0x07, 0x04); // PKTCTRL1
  writeCC1101Register(0x08, 0x00); // PKTCTRL0
  
  // AGC settings
  writeCC1101Register(0x1B, 0x40); // AGCCTRL2
  writeCC1101Register(0x1C, 0x00); // AGCCTRL1
  writeCC1101Register(0x1D, 0x91); // AGCCTRL0
  
  // Flush RX FIFO
  strobeCC1101(0x3A); // SFRX
  
  // Calibrate and enter RX
  strobeCC1101(0x33); // SCAL
  delay(5);
  strobeCC1101(0x34); // SRX
  delay(5);
  
  Serial.println(F("[RX] CC1101 configured, calibrating baseline..."));
  Serial.printf("[RX] Frequency: %.2f MHz, Modulation: %s\n", frequency, getModulationName(mod));
  
  // Set pixel mode
  pixelMode = PIXEL_RX;
  pixels.clear();
  pixels.show();
}

void stopRX() {
  Serial.println(F("[RX] Stopping RX mode"));
  
  raw_rx = "0";
  rxActive = false;
  pixelMode = PIXEL_MENU;
  
  strobeCC1101(0x36); // SIDLE
  
  SPI.endTransaction();
  SPI.end();
  
  pixels.clear();
  pixels.show();
  
  // Show summary
  unsigned long rxDuration = (millis() - rxStartTime) / 1000;
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("=[ RX STOPPED ]="));
  display.println();
  display.printf("Duration: %lu sec\n", rxDuration);
  display.printf("Signals: %d\n", signalCount);
  display.printf("Baseline: %d dBm\n", baselineRSSI);
  display.printf("Avg: %d dBm\n", (int)avgRSSI);
  display.printf("Peak: %d dBm\n", peakRSSI);
  display.display();
  delay(2000);
  
  currentMenu = MENU_RX;
  updateDisplay();
}

void handleRXMode() {
  if(!rxActive) return;
  
  // Check for stop condition
  if(digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
     digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW || 
     digitalRead(BTN_SELECT) == LOW) {
    if(millis() - lastButtonPress > buttonDebounce) {
      lastButtonPress = millis();
      stopRX();
      return;
    }
  }
  
  // Periodically refresh RX state (every 5 seconds)
  if(millis() - lastRXRefresh > 5000) {
    lastRXRefresh = millis();
    refreshRXState();
  }
  
  // Update displays
  updateWaterfall();
  if(baselineSet) {
    drawWaterfall();
  }
  updateRXPixels();
}

void updateWaterfall() {
  static unsigned long lastUpdate = 0;
  static int rssiSamples = 0;
  static long rssiSum = 0;
  
  // Update rate: 20 times per second
  if(millis() - lastUpdate < 50) return;
  lastUpdate = millis();
  
  // Read RSSI
  byte rawRSSI = readCC1101Register(0x34 | 0x40);
  
  // Convert to dBm and store as INSTANT RSSI
  if (rawRSSI >= 128) {
    instantRSSI = ((int)(rawRSSI - 256) / 2) - 74;
  } else {
    instantRSSI = (rawRSSI / 2) - 74;
  }
  
  // Keep currentRSSI for display (smoothed)
  currentRSSI = instantRSSI;
  
  // Update running average for display only
  rssiSum += instantRSSI;
  rssiSamples++;
  if(rssiSamples >= 20) {
    avgRSSI = (float)rssiSum / rssiSamples;
    rssiSum = 0;
    rssiSamples = 0;
  }
  
  // Update peak based on instant RSSI
  if(instantRSSI > peakRSSI) {
    peakRSSI = instantRSSI;
  }
  
  // Calibration phase - first 1 second
  if(!baselineSet) {
    if(millis() - baselineStartTime < 1000) {
      baselineSum += instantRSSI;
      baselineSamples++;
      
      // Update calibration display
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println(F("Calibrating..."));
      display.printf("RSSI: %d dBm\n", instantRSSI);
      display.printf("Samples: %d\n", baselineSamples);
      display.printf("Module: %c", activeModule == 0 ? 'A' : 'B');
      display.display();
      return;
    } else {
      // Set baseline
      baselineRSSI = baselineSum / baselineSamples;
      baselineSet = true;
      avgRSSI = baselineRSSI; // Initialize average
      Serial.printf("[RX] Baseline set: %d dBm (%d samples)\n", baselineRSSI, baselineSamples);
      
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println(F("Ready!"));
      display.printf("Baseline: %d dBm\n", baselineRSSI);
      display.display();
      delay(500);
    }
  }
  
  // ALWAYS shift waterfall down
  for(int y = WATERFALL_HEIGHT - 1; y > 0; y--) {
    memcpy(waterfallBuffer[y], waterfallBuffer[y-1], WATERFALL_WIDTH / 8);
  }
  
  // ALWAYS clear top line first
  memset(waterfallBuffer[0], 0, WATERFALL_WIDTH / 8);
  
  // Check for signal using INSTANT RSSI (not averaged!)
  int spike = instantRSSI - baselineRSSI;
  
  if(spike > 3) {  // Signal detected
    signalCount++;
    lastSignalTime = millis();
    
    // Map spike to width
    int width = map(spike, 3, 40, 1, WATERFALL_WIDTH);
    width = constrain(width, 1, WATERFALL_WIDTH);
    
    // Draw centered horizontal bar on TOP LINE ONLY
    int start = (WATERFALL_WIDTH - width) / 2;
    int end = start + width;
    
    for(int x = start; x < end; x++) {
      if(x >= 0 && x < WATERFALL_WIDTH) {
        waterfallBuffer[0][x / 8] |= (1 << (7 - (x % 8)));
      }
    }
    
    signalPixelBrightness = map(spike, 3, 40, 50, 255);
    
    Serial.printf("[RX] Signal! Instant RSSI: %d, Spike: +%d dB, Width: %d\n", 
                  instantRSSI, spike, width);
    
    // After detecting a strong signal, flush the RX FIFO to prevent overflow
    if(spike > 20) {
      strobeCC1101(0x3A); // SFRX - flush RX FIFO
      strobeCC1101(0x34); // SRX - back to RX mode
    }
  } else {
    // No signal - the line is already cleared above
    // Add minimal noise for visual feedback
    for(int i = 0; i < 2; i++) {
      int x = random(WATERFALL_WIDTH);
      waterfallBuffer[0][x / 8] |= (1 << (7 - (x % 8)));
    }
  }
  
  // Slowly drift baseline if needed (but use instant RSSI)
  static int noSignalCount = 0;
  if(spike <= 3) {
    noSignalCount++;
    if(noSignalCount > 200) {  // After 10 seconds of no signal
      // Slowly adjust baseline toward current instant RSSI
      baselineRSSI = (baselineRSSI * 19 + instantRSSI) / 20;
      noSignalCount = 100;  // Don't reset completely, keep some count
    }
  } else {
    noSignalCount = 0;  // Reset on signal detection
  }
}

void drawWaterfall() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print(frequency, 1);
  display.print(F("MHz "));
  display.print(activeModule == 0 ? 'A' : 'B');
  
  // Show instant RSSI delta (not averaged)
  display.setCursor(50, 0);
  int delta = instantRSSI - baselineRSSI;
  if(delta > 0) {
    display.printf("+%d", delta);
  } else {
    display.print(delta);
  }
  
  // Signal count
  display.setCursor(90, 0);
  display.printf("S:%d", signalCount);
  
  // Signal indicator (based on instant signal detection)
  if(millis() - lastSignalTime < 100) {
    display.fillCircle(120, 4, 3, SH110X_WHITE);
  }
  
  // Draw separator
  display.drawLine(0, 9, 127, 9, SH110X_WHITE);
  
  // Draw waterfall
  for(int y = 0; y < WATERFALL_HEIGHT && (y + 11) < 64; y++) {
    for(int byteX = 0; byteX < WATERFALL_WIDTH / 8; byteX++) {
      uint8_t byte = waterfallBuffer[y][byteX];
      if(byte != 0) {
        for(int bit = 0; bit < 8; bit++) {
          if(byte & (1 << (7 - bit))) {
            int x = byteX * 8 + bit;
            display.drawPixel(x, y + 11, SH110X_WHITE);
          }
        }
      }
    }
  }
  
  display.display();
}

// NeoPixel functions
void updateRXPixels() {
  if(!rxActive) return;
  if(millis() - lastRXPixelUpdate < 20) return;
  lastRXPixelUpdate = millis();
  
  // Fade signal brightness
  if(signalPixelBrightness > 0) {
    signalPixelBrightness -= 5;
    if(signalPixelBrightness < 0) signalPixelBrightness = 0;
  }
  
  // Simple scanner with signal flash
  rxScannerEffect(signalPixelBrightness);
  pixels.show();
}

void rxScannerEffect(int signalBrightness) {
  static int scanPos = 0;
  static int scanDir = 1;
  
  pixels.clear();
  
  // Base scanner
  for(int i = 0; i < 5; i++) {
    int pos = scanPos - (i * scanDir);
    if(pos >= 0 && pos < NEOPIXEL_COUNT) {
      int brightness = 100 - (i * 20);
      pixels.setPixelColor(pos, pixels.Color(0, 0, brightness));
    }
  }
  
  // Signal flash overlay
  if(signalBrightness > 0) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      uint32_t current = pixels.getPixelColor(i);
      uint8_t b = current & 0xFF;
      pixels.setPixelColor(i, pixels.Color(
        signalBrightness / 4,
        signalBrightness / 2,
        min(255, b + signalBrightness)
      ));
    }
  }
  
  scanPos += scanDir;
  if(scanPos >= NEOPIXEL_COUNT - 1 || scanPos <= 0) {
    scanDir = -scanDir;
  }
}

// Stub functions
void rxBreathingEffect(int strength) {}
void rxSpectrumAnalyzer(int strength) {}
void signalDetectedAnimation() {}
void enableReceive() {}
void receiver() {}
bool checkReceived() { return false; }
void processSignal() {}
void cycleRXVisualization() {}
void printReceived() {}
void signalanalyse() {}
void drawRFMatrix() {}
void drawRFScope() {}
void drawSignalMeter() {}
void showError(const char* error) {}

//============================================================
// SETTINGS.INO

// Settings.ino - EEPROM settings management and persistence

// EEPROM memory map
#define EEPROM_MAGIC 0xDEF33     // Magic number to verify valid data
#define EEPROM_VERSION 1          // Settings version

// EEPROM addresses
#define ADDR_MAGIC 0              // 4 bytes
#define ADDR_VERSION 4            // 1 byte
#define ADDR_MODULE 5             // 1 byte
#define ADDR_MOD 6                // 1 byte
#define ADDR_PATTERN 7            // 1 byte
#define ADDR_BRIGHTNESS 8         // 1 byte
#define ADDR_FREQUENCY 10         // 4 bytes (float)
#define ADDR_DEVIATION 14         // 4 bytes (float)
#define ADDR_RXBW 18              // 4 bytes (float)
#define ADDR_DATARATE 22          // 4 bytes (int)
#define ADDR_TESLA_FREQ 26        // 4 bytes (float)
#define ADDR_JAMMER_POWER 30      // 1 byte
#define ADDR_JAMMER_SWEEP 31      // 1 byte
#define ADDR_TESLA_COUNT 32       // 1 byte
#define ADDR_TESLA_DELAY 33       // 1 byte
#define ADDR_CHECKSUM 100         // 2 bytes

// Forward declarations
void setDefaultSettings();
void validateSettings();
uint16_t calculateChecksum();

void saveSettings() {
  Serial.println(F("\n[SETTINGS] ========== SAVING SETTINGS =========="));
  
  // Show saving message
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(15, 25);
  display.println(F("Saving settings..."));
  display.display();
  
  // Calculate checksum
  uint16_t checksum = calculateChecksum();
  
  // Write magic number and version
  EEPROM.put(ADDR_MAGIC, (uint32_t)EEPROM_MAGIC);
  EEPROM.write(ADDR_VERSION, EEPROM_VERSION);
  
  // Write settings
  EEPROM.write(ADDR_MODULE, activeModule);
  EEPROM.write(ADDR_MOD, mod);
  EEPROM.write(ADDR_PATTERN, idlePattern);
  EEPROM.write(ADDR_BRIGHTNESS, ledBrightness);
  EEPROM.put(ADDR_FREQUENCY, frequency);
  EEPROM.put(ADDR_DEVIATION, deviation);
  EEPROM.put(ADDR_RXBW, setrxbw);
  EEPROM.put(ADDR_DATARATE, datarate);
  EEPROM.put(ADDR_TESLA_FREQ, teslaFrequency);
  EEPROM.write(ADDR_JAMMER_POWER, jammerPower);
  EEPROM.write(ADDR_JAMMER_SWEEP, jammerSweep ? 1 : 0);
  EEPROM.write(ADDR_TESLA_COUNT, teslaTransmissions);
  EEPROM.write(ADDR_TESLA_DELAY, teslaDelay);
  
  // Write checksum
  EEPROM.put(ADDR_CHECKSUM, checksum);
  
  // Commit changes
  EEPROM.commit();
  
  // Log saved values
  Serial.println(F("[SETTINGS] Saved values:"));
  Serial.printf("  Module: %d\n", activeModule);
  Serial.printf("  Modulation: %d\n", mod);
  Serial.printf("  LED Pattern: %d\n", idlePattern);
  Serial.printf("  LED Brightness: %d\n", ledBrightness);
  Serial.printf("  Frequency: %.2f MHz\n", frequency);
  Serial.printf("  Deviation: %.2f kHz\n", deviation);
  Serial.printf("  RX BW: %.2f kHz\n", setrxbw);
  Serial.printf("  Data Rate: %d kBaud\n", datarate);
  Serial.printf("  Tesla Freq: %.2f MHz\n", teslaFrequency);
  Serial.printf("  Jammer Power: %d dBm\n", jammerPower);
  Serial.printf("  Jammer Sweep: %s\n", jammerSweep ? "ON" : "OFF");
  Serial.printf("  Tesla Count: %d\n", teslaTransmissions);
  Serial.printf("  Tesla Delay: %d ms\n", teslaDelay);
  Serial.printf("  Checksum: 0x%04X\n", checksum);
  
  // Success animation
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 20);
  display.println(F("SAVED!"));
  display.setTextSize(1);
  display.setCursor(15, 45);
  display.printf("Checksum: %04X", checksum);
  display.display();
  
  // Flash LEDs green
  for(int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  }
  pixels.show();
  delay(500);
  pixels.clear();
  pixels.show();
  
  delay(500);
}

void loadSettings() {
  Serial.println(F("\n[SETTINGS] ========== LOADING SETTINGS =========="));
  
  // Check magic number
  uint32_t magic;
  EEPROM.get(ADDR_MAGIC, magic);
  
  if(magic != EEPROM_MAGIC) {
    Serial.println(F("[SETTINGS] No valid settings found, using defaults"));
    setDefaultSettings();
    saveSettings();
    return;
  }
  
  // Check version
  uint8_t version = EEPROM.read(ADDR_VERSION);
  if(version != EEPROM_VERSION) {
    Serial.printf("[SETTINGS] Version mismatch (found %d, expected %d)\n", 
                  version, EEPROM_VERSION);
    setDefaultSettings();
    saveSettings();
    return;
  }
  
  // Load settings
  activeModule = EEPROM.read(ADDR_MODULE);
  mod = EEPROM.read(ADDR_MOD);
  idlePattern = (IdlePattern)EEPROM.read(ADDR_PATTERN);
  ledBrightness = EEPROM.read(ADDR_BRIGHTNESS);
  
  if (ledBrightness == 0 || ledBrightness > 255) {
    ledBrightness = 50;
  }
  
  EEPROM.get(ADDR_FREQUENCY, frequency);
  EEPROM.get(ADDR_DEVIATION, deviation);
  EEPROM.get(ADDR_RXBW, setrxbw);
  EEPROM.get(ADDR_DATARATE, datarate);
  EEPROM.get(ADDR_TESLA_FREQ, teslaFrequency);
  jammerPower = EEPROM.read(ADDR_JAMMER_POWER);
  jammerSweep = EEPROM.read(ADDR_JAMMER_SWEEP) == 1;
  teslaTransmissions = EEPROM.read(ADDR_TESLA_COUNT);
  teslaDelay = EEPROM.read(ADDR_TESLA_DELAY);
  
  // Verify checksum
  uint16_t storedChecksum;
  EEPROM.get(ADDR_CHECKSUM, storedChecksum);
  uint16_t calculatedChecksum = calculateChecksum();
  
  if(storedChecksum != calculatedChecksum) {
    Serial.printf("[SETTINGS] Checksum mismatch (stored: 0x%04X, calc: 0x%04X)\n", 
                  storedChecksum, calculatedChecksum);
    setDefaultSettings();
    saveSettings();
    return;
  }
  
  // Validate loaded values
  validateSettings();
  
  // Apply settings
  pixels.setBrightness(ledBrightness);
  
  Serial.println(F("[SETTINGS] Settings loaded successfully:"));
  Serial.printf("  Module: %d\n", activeModule);
  Serial.printf("  Frequency: %.2f MHz\n", frequency);
  Serial.printf("  Modulation: %d\n", mod);
  Serial.printf("  LED Pattern: %d\n", idlePattern);
  Serial.printf("  LED Brightness: %d\n", ledBrightness);
  Serial.printf("  Tesla Freq: %.2f MHz\n", teslaFrequency);
  Serial.printf("  Jammer Power: %d dBm\n", jammerPower);
  Serial.printf("  Checksum: 0x%04X\n", storedChecksum);
}

void setDefaultSettings() {
  Serial.println(F("[SETTINGS] Setting defaults"));
  
  activeModule = 0;
  mod = 2; // ASK/OOK
  idlePattern = PATTERN_BREATHE;
  ledBrightness = 50;
  frequency = 433.92;
  deviation = 47.60;
  setrxbw = 812.50;
  datarate = 99;
  teslaFrequency = 433.92;
  jammerPower = 12;
  jammerSweep = false;
  teslaTransmissions = 5;
  teslaDelay = 23;
}

void validateSettings() {
  bool needsSave = false;
  
  // Validate frequency
  if (frequency < 300 || frequency > 928) {
    Serial.printf("[SETTINGS] Invalid frequency: %.2f, resetting to 433.92\n", frequency);
    frequency = 433.92;
    needsSave = true;
  }
  
  // Validate Tesla frequency
  if (teslaFrequency != 315.00 && teslaFrequency != 433.92) {
    Serial.printf("[SETTINGS] Invalid Tesla freq: %.2f, resetting to 433.92\n", teslaFrequency);
    teslaFrequency = 433.92;
    needsSave = true;
  }
  
  // Validate deviation
  if (deviation < 0 || deviation > 500) {
    Serial.printf("[SETTINGS] Invalid deviation: %.2f, resetting to 47.60\n", deviation);
    deviation = 47.60;
    needsSave = true;
  }
  
  // Validate RX bandwidth
  if (setrxbw < 50 || setrxbw > 1000) {
    Serial.printf("[SETTINGS] Invalid RX BW: %.2f, resetting to 812.50\n", setrxbw);
    setrxbw = 812.50;
    needsSave = true;
  }
  
  // Validate data rate
  if (datarate < 0 || datarate > 2000) {
    Serial.printf("[SETTINGS] Invalid data rate: %d, resetting to 99\n", datarate);
    datarate = 99;
    needsSave = true;
  }
  
  // Validate module
  if (activeModule > 1) {
    Serial.printf("[SETTINGS] Invalid module: %d, resetting to 0\n", activeModule);
    activeModule = 0;
    needsSave = true;
  }
  
  // Validate modulation
  if (mod > 4) {
    Serial.printf("[SETTINGS] Invalid modulation: %d, resetting to 2\n", mod);
    mod = 2;
    needsSave = true;
  }
  
  // Validate LED pattern
  if (idlePattern > PATTERN_OFF) {
    Serial.printf("[SETTINGS] Invalid LED pattern: %d, resetting to BREATHE\n", idlePattern);
    idlePattern = PATTERN_BREATHE;
    needsSave = true;
  }
  
  // Validate jammer power
  if (jammerPower < -30 || jammerPower > 12) {
    Serial.printf("[SETTINGS] Invalid jammer power: %d, resetting to 12\n", jammerPower);
    jammerPower = 12;
    needsSave = true;
  }
  
  // Validate Tesla settings
  if (teslaTransmissions < 1 || teslaTransmissions > 10) {
    Serial.printf("[SETTINGS] Invalid Tesla count: %d, resetting to 5\n", teslaTransmissions);
    teslaTransmissions = 5;
    needsSave = true;
  }
  
  if (teslaDelay < 10 || teslaDelay > 100) {
    Serial.printf("[SETTINGS] Invalid Tesla delay: %d, resetting to 23\n", teslaDelay);
    teslaDelay = 23;
    needsSave = true;
  }
  
  if (needsSave) {
    Serial.println(F("[SETTINGS] Validation failed, will save corrected values"));
  }
}

uint16_t calculateChecksum() {
  uint16_t sum = 0;
  
  // Sum all setting values
  sum += activeModule;
  sum += mod;
  sum += idlePattern;
  sum += ledBrightness;
  sum += (uint16_t)(frequency * 100);
  sum += (uint16_t)(deviation * 100);
  sum += (uint16_t)(setrxbw * 10);
  sum += datarate;
  sum += (uint16_t)(teslaFrequency * 100);
  sum += jammerPower;
  sum += jammerSweep ? 1 : 0;
  sum += teslaTransmissions;
  sum += teslaDelay;
  
  // Simple checksum
  return sum ^ 0xDEF3;
}

void resetToFactory() {
  Serial.println(F("\n[SETTINGS] ========== FACTORY RESET =========="));
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(F("Factory Reset?"));
  display.println(F(""));
  display.println(F("RIGHT = Confirm"));
  display.println(F("LEFT = Cancel"));
  display.display();
  
  bool waiting = true;
  unsigned long startTime = millis();
  
  while(waiting && (millis() - startTime < 10000)) { // 10 second timeout
    if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      // Confirmed
      Serial.println(F("[SETTINGS] Factory reset confirmed"));
      
      display.clearDisplay();
      display.setCursor(10, 25);
      display.println(F("Resetting..."));
      display.display();
      
      // Clear EEPROM
      for(int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0xFF);
      }
      EEPROM.commit();
      
      // Set defaults
      setDefaultSettings();
      saveSettings();
      
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(15, 25);
      display.println(F("RESET!"));
      display.display();
      
      // Red flash
      for(int i = 0; i < NEOPIXEL_COUNT; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      }
      pixels.show();
      delay(1000);
      pixels.clear();
      pixels.show();
      
      // Restart
      Serial.println(F("[SETTINGS] Restarting..."));
      delay(1000);
      ESP.restart();
    }
    else if(digitalRead(BTN_LEFT) == LOW) {
      // Cancelled
      Serial.println(F("[SETTINGS] Factory reset cancelled"));
      waiting = false;
    }
    
    delay(10);
  }
  
  if(millis() - startTime >= 10000) {
    Serial.println(F("[SETTINGS] Factory reset timeout"));
  }
}

void exportSettings() {
  if(!sdCardPresent) {
    debugPrint("No SD card!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[SETTINGS] ========== EXPORT SETTINGS =========="));
  
  char filename[32];
  sprintf(filename, "/settings_%lu.cfg", millis());
  
  File configFile = SD_MMC.open(filename, FILE_WRITE);
  if(!configFile) {
    debugPrint("Export failed!", true, true, 2000);
    return;
  }
  
  // Write settings in readable format
  configFile.println(F("# TPP Badge Configuration"));
  configFile.printf("# Exported: %lu\n", millis());
  configFile.println(F("# Version: 1.0"));
  configFile.println();
  
  configFile.printf("module=%d\n", activeModule);
  configFile.printf("frequency=%.2f\n", frequency);
  configFile.printf("modulation=%d\n", mod);
  configFile.printf("deviation=%.2f\n", deviation);
  configFile.printf("bandwidth=%.2f\n", setrxbw);
  configFile.printf("datarate=%d\n", datarate);
  configFile.printf("tesla_freq=%.2f\n", teslaFrequency);
  configFile.printf("tesla_count=%d\n", teslaTransmissions);
  configFile.printf("tesla_delay=%d\n", teslaDelay);
  configFile.printf("jammer_power=%d\n", jammerPower);
  configFile.printf("jammer_sweep=%d\n", jammerSweep ? 1 : 0);
  configFile.printf("led_pattern=%d\n", idlePattern);
  configFile.printf("led_brightness=%d\n", ledBrightness);
  
  configFile.close();
  
  Serial.printf("[SETTINGS] Exported to %s\n", filename);
  
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(F("Settings exported!"));
  display.println(filename);
  display.display();
  delay(2000);
}

//===================================================================
// TESLA.INO

// Tesla.ino - Tesla charging port signal functions

// Tesla signal parameters
const uint16_t pulseWidth = 400;
const uint16_t messageDistance = 23;
const uint8_t transmtesla = 5;
const uint8_t messageLength = 43;
const uint8_t sequence[messageLength] = { 
  0x02,0xAA,0xAA,0xAA,
  0x2B,
  0x2C,0xCB,0x33,0x33,0x2D,0x34,0xB5,0x2B,0x4D,0x32,0xAD,0x2C,0x56,0x59,0x96,0x66,
  0x66,0x5A,0x69,0x6A,0x56,0x9A,0x65,0x5A,0x58,0xAC,0xB3,0x2C,0xCC,0xCC,0xB4,0xD2,
  0xD4,0xAD,0x34,0xCA,0xB4,0xA0
};

// Additional Tesla configurations
bool teslaRepeat = false;
unsigned long teslaStartTime = 0;

// CC1101 Direct SPI functions (matching RX.ino style)
byte writeCC1101Register(byte addr, byte value, int module) {
  int csPin = (module == 0) ? CC1101_CS_A : CC1101_CS_B;
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  
  int misoPin = (module == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  while(digitalRead(misoPin));
  
  SPI.transfer(addr);
  SPI.transfer(value);
  
  digitalWrite(csPin, HIGH);
  return value;
}

void strobeCC1101TX(byte cmd, int module) {
  int csPin = (module == 0) ? CC1101_CS_A : CC1101_CS_B;
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  
  int misoPin = (module == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  while(digitalRead(misoPin));
  
  SPI.transfer(cmd);
  
  digitalWrite(csPin, HIGH);
}

void sendTeslaSignal() {
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[TESLA] ========== SENDING TESLA SIGNAL =========="));
  
  debugPrint("Sending Tesla!", true, true, 500);
  
  pixelMode = PIXEL_TESLA;
  teslaStartTime = millis();
  
  // Use whichever module is available
  if(activeModule == 0 && !cc1101APresent) {
    activeModule = 1;
    Serial.println(F("[TESLA] Module A not present, switching to B"));
  } else if(activeModule == 1 && !cc1101BPresent) {
    activeModule = 0;
    Serial.println(F("[TESLA] Module B not present, switching to A"));
  }
  
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, LOW);
  
  Serial.printf("[TESLA] Configuration:\n");
  Serial.printf("        Module: %c\n", activeModule == 0 ? 'A' : 'B');
  Serial.printf("        Frequency: %.2f MHz\n", teslaFrequency);
  Serial.printf("        Transmissions: %d\n", teslaTransmissions);
  Serial.printf("        Pulse Width: %d us\n", pulseWidth);
  Serial.printf("        Message Distance: %d ms\n", teslaDelay);
  Serial.printf("        Signal Length: %d bytes\n", messageLength);
  
  // Initialize SPI for the selected module
  int sckPin = (activeModule == 0) ? CC1101_CLK_A : CC1101_CLK_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  int mosiPin = (activeModule == 0) ? CC1101_MOSI_A : CC1101_MOSI_B;
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  
  SPI.begin(sckPin, misoPin, mosiPin, csPin);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  
  // Reset CC1101
  strobeCC1101TX(0x30, activeModule); // SRES
  delay(10);
  
  // Configure for Tesla TX
  // Set frequency for Tesla (315 or 433.92 MHz)
  if(teslaFrequency > 400) {
    // 433.92 MHz
    writeCC1101Register(0x0D, 0x10, activeModule); // FREQ2
    writeCC1101Register(0x0E, 0xB0, activeModule); // FREQ1  
    writeCC1101Register(0x0F, 0x7A, activeModule); // FREQ0
  } else {
    // 315 MHz
    writeCC1101Register(0x0D, 0x0C, activeModule); // FREQ2
    writeCC1101Register(0x0E, 0x4E, activeModule); // FREQ1  
    writeCC1101Register(0x0F, 0xC4, activeModule); // FREQ0
  }
  
  // Configure for ASK/OOK modulation
  writeCC1101Register(0x10, 0x00, activeModule); // MDMCFG4 - Max BW
  writeCC1101Register(0x11, 0x22, activeModule); // MDMCFG3
  writeCC1101Register(0x12, 0x30, activeModule); // MDMCFG2 - ASK/OOK, no sync
  writeCC1101Register(0x07, 0x04, activeModule); // PKTCTRL1
  writeCC1101Register(0x08, 0x00, activeModule); // PKTCTRL0
  writeCC1101Register(0x1B, 0x40, activeModule); // AGCCTRL2
  writeCC1101Register(0x1C, 0x00, activeModule); // AGCCTRL1
  writeCC1101Register(0x1D, 0x91, activeModule); // AGCCTRL0
  
  // Set TX power to maximum
  writeCC1101Register(0x3E, 0xC0, activeModule); // PATABLE - max power
  
  // Configure GDO0 for TX
  writeCC1101Register(0x02, 0x0C, activeModule); // IOCFG0 - Serial TX data
  
  // Calibrate and enter TX
  strobeCC1101TX(0x33, activeModule); // SCAL
  delay(5);
  strobeCC1101TX(0x35, activeModule); // STX
  delay(5);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ TESLA TX ]="));
  display.drawRect(0, 0, 128, 10, SH110X_WHITE);
  display.setCursor(0,15);
  display.print(F("Freq: "));
  display.print(teslaFrequency, 2);
  display.println(F(" MHz"));
  display.println(F(""));
  display.println(F("Sending signal..."));
  display.display();
  
  Serial.println(F("[TESLA] Signal sequence:"));
  for(int i = 0; i < messageLength; i++) {
    Serial.printf("0x%02X ", sequence[i]);
    if((i + 1) % 16 == 0) Serial.println();
  }
  Serial.println();
  
  // Send the signal
  for (uint8_t t = 0; t < teslaTransmissions; t++) {
    Serial.printf("[TESLA] Transmission %d/%d\n", t + 1, teslaTransmissions);
    
    // Update display with progress
    display.fillRect(0, 35, 128, 10, SH110X_BLACK);
    display.setCursor(0, 35);
    display.print(F("TX: "));
    display.print(t + 1);
    display.print(F("/"));
    display.print(teslaTransmissions);
    
    // Progress bar
    int progress = map(t, 0, teslaTransmissions - 1, 0, 100);
    display.drawRect(10, 48, 102, 8, SH110X_WHITE);
    display.fillRect(11, 49, progress, 6, SH110X_WHITE);
    display.display();
    
    // Send each byte using direct pin control
    for (uint8_t i = 0; i < messageLength; i++) {
      sendByte(sequence[i], txPin);
    }
    
    digitalWrite(txPin, LOW);
    
    // Animation during delay
    if(t < teslaTransmissions - 1) {
      animateTeslaDelay(teslaDelay);
    }
  }
  
  // Return to idle
  strobeCC1101TX(0x36, activeModule); // SIDLE
  
  SPI.endTransaction();
  SPI.end();
  
  pixelMode = PIXEL_MENU;
  
  unsigned long txTime = millis() - teslaStartTime;
  Serial.printf("[TESLA] Transmission complete in %lu ms\n", txTime);
  
  // Success display
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 20);
  display.println(F("SENT!"));
  display.setTextSize(1);
  display.setCursor(20, 45);
  display.printf("Time: %lu ms", txTime);
  display.display();
  
  // Success animation on LEDs (function is defined in Pixels.ino)
  teslaSuccessAnimation();
  
  delay(1500);
  updateDisplay();
}

void sendByte(uint8_t dataByte, int txPin) {
  for (int8_t bit = 7; bit >= 0; bit--) {
    digitalWrite(txPin, (dataByte & (1 << bit)) != 0 ? HIGH : LOW);
    delayMicroseconds(pulseWidth);
  }
}

void animateTeslaDelay(int delayTime) {
  // Animate during the delay between transmissions
  unsigned long startDelay = millis();
  
  while(millis() - startDelay < delayTime) {
    // Check for abort
    if(digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      Serial.println(F("[TESLA] Transmission aborted by user"));
      return;
    }
    
    // Update LED animation
    if(millis() - lastPixelUpdate > 20) {
      lastPixelUpdate = millis();
      lightningAnimation();
      pixels.show();
    }
    
    delay(1);
  }
}

void configureTesla() {
  Serial.println(F("\n[TESLA] ===== TESLA CONFIG ====="));
  
  bool configuring = true;
  int configOption = 0;
  const int numOptions = 5;
  
  while(configuring) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ TESLA CFG ]="));
    display.println(F(""));
    
    // Menu options with current values
    display.setCursor(0, 16);
    display.print(configOption == 0 ? ">" : " ");
    display.print(F("Freq: "));
    display.print(teslaFrequency, 2);
    display.println(F(" MHz"));
    
    display.print(configOption == 1 ? ">" : " ");
    display.print(F("Count: "));
    display.println(teslaTransmissions);
    
    display.print(configOption == 2 ? ">" : " ");
    display.print(F("Delay: "));
    display.print(teslaDelay);
    display.println(F(" ms"));
    
    display.print(configOption == 3 ? ">" : " ");
    display.print(F("Repeat: "));
    display.println(teslaRepeat ? "ON" : "OFF");
    
    display.print(configOption == 4 ? ">" : " ");
    display.println(F("Back"));
    
    display.display();
    
    // Handle input
    if(millis() - lastButtonPress > buttonDebounce) {
      if(digitalRead(BTN_UP) == LOW) {
        lastButtonPress = millis();
        configOption = (configOption - 1 + numOptions) % numOptions;
        Serial.printf("[TESLA] Config option: %d\n", configOption);
      }
      else if(digitalRead(BTN_DOWN) == LOW) {
        lastButtonPress = millis();
        configOption = (configOption + 1) % numOptions;
        Serial.printf("[TESLA] Config option: %d\n", configOption);
      }
      else if(digitalRead(BTN_RIGHT) == LOW || digitalRead(BTN_SELECT) == LOW) {
        lastButtonPress = millis();
        
        switch(configOption) {
          case 0: // Frequency toggle
            if (teslaFrequency == 433.92) {
              teslaFrequency = 315.00;
            } else {
              teslaFrequency = 433.92;
            }
            Serial.printf("[TESLA] Frequency: %.2f MHz\n", teslaFrequency);
            break;
            
          case 1: // Transmission count
            teslaTransmissions++;
            if(teslaTransmissions > 10) teslaTransmissions = 1;
            Serial.printf("[TESLA] Transmissions: %d\n", teslaTransmissions);
            break;
            
          case 2: // Delay
            teslaDelay += 5;
            if(teslaDelay > 100) teslaDelay = 10;
            Serial.printf("[TESLA] Delay: %d ms\n", teslaDelay);
            break;
            
          case 3: // Repeat mode
            teslaRepeat = !teslaRepeat;
            Serial.printf("[TESLA] Repeat: %s\n", teslaRepeat ? "ON" : "OFF");
            break;
            
          case 4: // Back
            configuring = false;
            break;
        }
        
        if(configOption < 4) {
          saveSettings();
        }
      }
      else if(digitalRead(BTN_LEFT) == LOW) {
        lastButtonPress = millis();
        configuring = false;
      }
    }
    
    delay(10);
  }
  
  Serial.println(F("[TESLA] Configuration complete"));
}

void sendTeslaRepeat() {
  if(!teslaRepeat) {
    sendTeslaSignal();
    return;
  }
  
  Serial.println(F("\n[TESLA] ===== REPEAT MODE ====="));
  
  bool repeating = true;
  int repeatCount = 0;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ TESLA RPT ]="));
  display.println(F(""));
  display.println(F("Repeat mode ON"));
  display.println(F(""));
  display.println(F("Any button stops"));
  display.display();
  delay(1000);
  
  while(repeating) {
    // Check for stop
    if(digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
       digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW || 
       digitalRead(BTN_SELECT) == LOW) {
      if(millis() - lastButtonPress > buttonDebounce) {
        lastButtonPress = millis();
        repeating = false;
        break;
      }
    }
    
    repeatCount++;
    Serial.printf("[TESLA] Repeat transmission %d\n", repeatCount);
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ TESLA RPT ]="));
    display.println(F(""));
    display.printf("Count: %d\n", repeatCount);
    display.printf("Freq: %.2f MHz\n", teslaFrequency);
    display.println(F(""));
    display.println(F("Sending..."));
    display.display();
    
    // Send the signal
    sendTeslaSignal();
    
    // Delay between repeats
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ TESLA RPT ]="));
    display.println(F(""));
    display.printf("Count: %d\n", repeatCount);
    display.println(F(""));
    display.println(F("Waiting..."));
    display.println(F(""));
    display.println(F("Any button stops"));
    display.display();
    
    // Wait with ability to cancel
    unsigned long waitStart = millis();
    while(millis() - waitStart < 5000) { // 5 second delay between repeats
      if(digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
         digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW || 
         digitalRead(BTN_SELECT) == LOW) {
        repeating = false;
        break;
      }
      delay(10);
    }
  }
  
  Serial.printf("[TESLA] Repeat mode stopped after %d transmissions\n", repeatCount);
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("Repeat stopped"));
  display.printf("Total: %d\n", repeatCount);
  display.display();
  delay(1500);
}

void analyzeTeslaSignal() {
  Serial.println(F("\n[TESLA] ===== SIGNAL ANALYSIS ====="));
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ TESLA INFO ]="));
  display.println(F(""));
  display.printf("Length: %d bytes\n", messageLength);
  display.printf("Pulse: %d us\n", pulseWidth);
  display.println(F(""));
  display.println(F("Header: 02AAAAAA"));
  display.println(F("Type: ASK/OOK"));
  display.display();
  
  Serial.println(F("[TESLA] Signal structure:"));
  Serial.printf("  Header: 0x%02X 0x%02X 0x%02X 0x%02X\n", 
                sequence[0], sequence[1], sequence[2], sequence[3]);
  Serial.printf("  Key byte: 0x%02X\n", sequence[4]);
  Serial.println(F("  Payload:"));
  
  for(int i = 5; i < messageLength; i++) {
    Serial.printf("  [%02d] 0x%02X - ", i, sequence[i]);
    for(int bit = 7; bit >= 0; bit--) {
      Serial.print((sequence[i] >> bit) & 1);
    }
    Serial.println();
  }
  
  delay(5000);
}

//=====================================================================
// TX.INO

// TX.ino - All TX mode functions

long data_to_send[2000];
int data_count = 0;
bool txActive = false;
bool TX_DEMO_MODE = true;  // Set to false when SD card and RX save are working

// CC1101 Direct SPI functions
byte writeTXRegister(byte addr, byte value, int module) {
  int csPin = (module == 0) ? CC1101_CS_A : CC1101_CS_B;
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  
  int misoPin = (module == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  while(digitalRead(misoPin));
  
  SPI.transfer(addr);
  SPI.transfer(value);
  
  digitalWrite(csPin, HIGH);
  return value;
}

void strobeTX(byte cmd, int module) {
  int csPin = (module == 0) ? CC1101_CS_A : CC1101_CS_B;
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  
  int misoPin = (module == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  while(digitalRead(misoPin));
  
  SPI.transfer(cmd);
  
  digitalWrite(csPin, HIGH);
}

void showComingSoonAnimation() {
  // Fun "coming soon" animation
  display.clearDisplay();
  
  // Pirate-themed animation
  for(int frame = 0; frame < 30; frame++) {
    display.clearDisplay();
    
    // Draw skull
    int skullY = 15 + sin(frame * 0.2) * 5;
    display.fillCircle(64, skullY, 8, SH110X_WHITE);
    display.fillCircle(61, skullY - 2, 2, SH110X_BLACK);
    display.fillCircle(67, skullY - 2, 2, SH110X_BLACK);
    
    // Text that fades in
    if(frame > 10) {
      display.setCursor(20, 35);
      display.println(F("COMING SOON"));
    }
    if(frame > 20) {
      display.setCursor(15, 45);
      display.println(F("Future Update!"));
    }
    
    display.display();
    
    // Rainbow effect on pixels
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      int hue = (frame * 10 + i * 20) % 360;
      pixels.setPixelColor(i, pixels.ColorHSV(hue * 182, 255, 100));
    }
    pixels.show();
    delay(50);
  }
  
  delay(1000);
  pixels.clear();
  pixels.show();
}

void runDemoTX(const char* modeName) {
  Serial.printf("[TX DEMO] Running demo for %s\n", modeName);
  
  // Simulate TX with fun animations
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(F("=[ "));
  display.print(modeName);
  display.println(F(" ]="));
  display.drawRect(0, 0, 128, 10, SH110X_WHITE);
  display.setCursor(0,15);
  display.printf("Freq: %.2f MHz\n", frequency);
  display.printf("Module: %c\n", activeModule == 0 ? 'A' : 'B');
  display.println(F(""));
  display.println(F("Demo Mode Active"));
  display.display();
  delay(1000);
  
  display.fillRect(0, 35, 128, 20, SH110X_BLACK);
  display.setCursor(0, 35);
  display.println(F("Transmitting..."));
  display.display();
  
  // Fun pixel patterns during "transmission"
  unsigned long demoStart = millis();
  int pattern = 0;
  
  while(millis() - demoStart < 5000) {  // 5 second demo
    // Check for abort
    if(digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      break;
    }
    
    // Update display with animated bars
    display.fillRect(0, 45, 128, 19, SH110X_BLACK);
    
    // Animated signal pattern
    for(int x = 0; x < 128; x += 4) {
      int height = 4 + sin((x + millis() / 50.0) * 0.1) * 4;
      display.drawLine(x, 64 - height, x, 64, SH110X_WHITE);
    }
    
    // Progress indicator
    int progress = map(millis() - demoStart, 0, 5000, 0, 100);
    display.fillRect(0, 52, 128, 10, SH110X_BLACK);
    display.setCursor(0, 52);
    display.printf("Progress: %d%%", progress);
    display.display();
    
    // Cycle through different LED patterns
    pattern = ((millis() - demoStart) / 500) % 4;
    
    switch(pattern) {
      case 0:  // Wave effect
        for(int i = 0; i < NEOPIXEL_COUNT; i++) {
          int brightness = 128 + 127 * sin((millis() / 100.0) + (i * 0.5));
          pixels.setPixelColor(i, pixels.Color(brightness, 0, 0));
        }
        break;
        
      case 1:  // Pulse effect
        {
          int pulse = (millis() / 10) % 255;
          for(int i = 0; i < NEOPIXEL_COUNT; i++) {
            pixels.setPixelColor(i, pixels.Color(0, pulse, 0));
          }
        }
        break;
        
      case 2:  // Chase effect
        pixels.clear();
        {
          int pos = (millis() / 50) % NEOPIXEL_COUNT;
          for(int i = 0; i < 3; i++) {
            int idx = (pos + i) % NEOPIXEL_COUNT;
            pixels.setPixelColor(idx, pixels.Color(0, 0, 255 - i * 80));
          }
        }
        break;
        
      case 3:  // Random sparkle
        for(int i = 0; i < NEOPIXEL_COUNT; i++) {
          if(random(10) < 3) {
            pixels.setPixelColor(i, pixels.Color(255, 255, 255));
          } else {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
          }
        }
        break;
    }
    
    pixels.show();
    delay(20);
  }
  
  // Success animation
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 15);
  display.println(F("SENT!"));
  display.setTextSize(1);
  display.setCursor(20, 35);
  display.println(F("(Demo Mode)"));
  display.setCursor(10, 50);
  display.print(modeName);
  display.display();
  
  // Victory rainbow sweep on LEDs
  for(int sweep = 0; sweep < 50; sweep++) {
    for(int i = 0; i < NEOPIXEL_COUNT; i++) {
      int hue = (sweep * 5 + i * 30) % 360;
      pixels.setPixelColor(i, pixels.ColorHSV(hue * 182, 255, 255));
    }
    pixels.show();
    delay(20);
  }
  
  pixels.clear();
  pixels.show();
  delay(1500);
}

void startTX() {
  if(TX_DEMO_MODE) {
    Serial.println(F("[TX] Demo mode active - Simple TX"));
    runDemoTX("TX MODE");
    return;
  }
  
  // Original TX code continues here...
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[TX] ========== STARTING TX MODE =========="));
  
  // Use whichever module is available
  if(activeModule == 0 && !cc1101APresent) {
    activeModule = 1;
    Serial.println(F("[TX] Module A not present, switching to B"));
  } else if(activeModule == 1 && !cc1101BPresent) {
    activeModule = 0;
    Serial.println(F("[TX] Module B not present, switching to A"));
  }
  
  Serial.printf("[TX] Configuration:\n");
  Serial.printf("     Module: %c\n", activeModule == 0 ? 'A' : 'B');
  Serial.printf("     Frequency: %.2f MHz\n", frequency);
  Serial.printf("     Modulation: %d (%s)\n", mod, getModulationName(mod));
  Serial.printf("     Deviation: %.2f kHz\n", deviation);
  Serial.printf("     Data Rate: %d kBaud\n", datarate);
  
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, LOW);
  
  // Initialize SPI for the selected module
  int sckPin = (activeModule == 0) ? CC1101_CLK_A : CC1101_CLK_B;
  int misoPin = (activeModule == 0) ? CC1101_MISO_A : CC1101_MISO_B;
  int mosiPin = (activeModule == 0) ? CC1101_MOSI_A : CC1101_MOSI_B;
  int csPin = (activeModule == 0) ? CC1101_CS_A : CC1101_CS_B;
  
  SPI.begin(sckPin, misoPin, mosiPin, csPin);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  
  // Reset CC1101
  strobeTX(0x30, activeModule); // SRES
  delay(10);
  
  // Configure frequency
  byte freq2, freq1, freq0;
  // Calculate frequency registers (simplified for common frequencies)
  if(frequency >= 433 && frequency <= 435) {
    freq2 = 0x10; freq1 = 0xB0; freq0 = 0x7A; // ~433.92 MHz
  } else if(frequency >= 314 && frequency <= 316) {
    freq2 = 0x0C; freq1 = 0x4E; freq0 = 0xC4; // ~315 MHz
  } else if(frequency >= 867 && frequency <= 869) {
    freq2 = 0x21; freq1 = 0x65; freq0 = 0x6A; // ~868 MHz
  } else if(frequency >= 914 && frequency <= 916) {
    freq2 = 0x23; freq1 = 0x31; freq0 = 0x3B; // ~915 MHz
  } else {
    // Default to 433.92
    freq2 = 0x10; freq1 = 0xB0; freq0 = 0x7A;
  }
  
  writeTXRegister(0x0D, freq2, activeModule); // FREQ2
  writeTXRegister(0x0E, freq1, activeModule); // FREQ1  
  writeTXRegister(0x0F, freq0, activeModule); // FREQ0
  
  // Configure modulation
  byte mdmcfg2 = 0x00;
  switch(mod) {
    case 0: mdmcfg2 = 0x00; break; // 2-FSK
    case 1: mdmcfg2 = 0x10; break; // GFSK
    case 2: mdmcfg2 = 0x30; break; // ASK/OOK
    case 3: mdmcfg2 = 0x40; break; // 4-FSK
    case 4: mdmcfg2 = 0x70; break; // MSK
  }
  
  writeTXRegister(0x10, 0x00, activeModule); // MDMCFG4 - Max BW
  writeTXRegister(0x11, 0x22, activeModule); // MDMCFG3
  writeTXRegister(0x12, mdmcfg2, activeModule); // MDMCFG2 - Modulation
  writeTXRegister(0x07, 0x04, activeModule); // PKTCTRL1
  writeTXRegister(0x08, 0x00, activeModule); // PKTCTRL0
  writeTXRegister(0x1B, 0x40, activeModule); // AGCCTRL2
  writeTXRegister(0x1C, 0x00, activeModule); // AGCCTRL1
  writeTXRegister(0x1D, 0x91, activeModule); // AGCCTRL0
  
  // Set deviation if using FSK
  if(mod != 2) { // Not ASK/OOK
    writeTXRegister(0x15, 0x47, activeModule); // DEVIATN
  }
  
  // Set TX power to maximum
  writeTXRegister(0x3E, 0xC0, activeModule); // PATABLE - max power
  
  // Configure GDO0 for TX
  writeTXRegister(0x02, 0x0C, activeModule); // IOCFG0 - Serial TX data
  
  // Calibrate and enter TX
  strobeTX(0x33, activeModule); // SCAL
  delay(5);
  strobeTX(0x35, activeModule); // STX
  delay(5);
  
  txActive = true;
  pixelMode = PIXEL_TX;
  
  Serial.println(F("[TX] TX mode initialized"));
}

void stopTX() {
  Serial.println(F("\n[TX] ========== STOPPING TX MODE =========="));
  
  txActive = false;
  pixelMode = PIXEL_MENU;
  
  if(!TX_DEMO_MODE && (cc1101APresent || cc1101BPresent)) {
    strobeTX(0x36, activeModule); // SIDLE
    Serial.println(F("[TX] CC1101 set to idle"));
    
    SPI.endTransaction();
    SPI.end();
    
    // Set TX pin low
    int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
    digitalWrite(txPin, LOW);
  }
  
  debugPrint("TX Stopped", true, true, 1000);
}

void sendBinaryData() {
  if(TX_DEMO_MODE) {
    Serial.println(F("[TX] Demo mode - Binary TX"));
    runDemoTX("BINARY TX");
    return;
  }
  
  // Original code continues...
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  if(!txActive) {
    startTX();
  }
  
  Serial.println(F("\n[TX] ===== BINARY TX MODE ====="));
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ BINARY TX ]="));
  display.println(F(""));
  display.println(F("Enter data via"));
  display.println(F("serial terminal"));
  display.println(F(""));
  display.println(F("Format: 0,1,0,1..."));
  display.println(F("Send 'x' to exit"));
  display.display();
  
  Serial.println(F("[TX] Binary TX mode - Enter binary data"));
  Serial.println(F("[TX] Format: 0,1,0,1,1,0,1,0..."));
  Serial.println(F("[TX] Send 'x' to exit"));
  Serial.println(F("[TX] Send 't' to transmit"));
  
  String binaryData = "";
  bool collecting = true;
  
  while(collecting) {
    // Check buttons
    if(digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_SELECT) == LOW) {
      Serial.println(F("[TX] Binary TX cancelled by button"));
      break;
    }
    
    if(Serial.available()) {
      char c = Serial.read();
      
      if(c == 'x' || c == 'X') {
        Serial.println(F("[TX] Exiting binary TX mode"));
        collecting = false;
      }
      else if(c == 't' || c == 'T') {
        if(binaryData.length() > 0) {
          Serial.println(F("[TX] Transmitting binary data..."));
          transmitBinaryString(binaryData);
          binaryData = "";
        } else {
          Serial.println(F("[TX] No data to transmit"));
        }
      }
      else if(c == '0' || c == '1') {
        binaryData += c;
        Serial.print(c);
        
        // Update display
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("=[ BINARY TX ]="));
        display.println(F(""));
        display.print(F("Bits: "));
        display.println(binaryData.length());
        display.println(F(""));
        
        // Show last 20 bits
        int startIdx = max(0, (int)binaryData.length() - 20);
        display.println(binaryData.substring(startIdx));
        display.display();
      }
      else if(c == ',') {
        // Ignore commas
      }
      else if(c == '\n' || c == '\r') {
        Serial.println();
      }
    }
    
    delay(10);
  }
  
  stopTX();
}

void transmitBinaryString(String data) {
  if(TX_DEMO_MODE) {
    // Demo version
    Serial.printf("[TX DEMO] Would transmit %d bits\n", data.length());
    
    display.clearDisplay();
    display.setCursor(0,20);
    display.println(F("Demo TX..."));
    display.printf("%d bits\n", data.length());
    display.display();
    
    // Show bits on LEDs
    for(int i = 0; i < min(NEOPIXEL_COUNT, (int)data.length()); i++) {
      pixels.setPixelColor(i, data[i] == '1' ? 
        pixels.Color(255, 0, 0) : pixels.Color(0, 0, 255));
    }
    pixels.show();
    delay(2000);
    pixels.clear();
    pixels.show();
    
    display.clearDisplay();
    display.setCursor(0,20);
    display.println(F("Demo Complete!"));
    display.display();
    delay(1000);
    return;
  }
  
  // Original code
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  
  Serial.printf("[TX] Transmitting %d bits\n", data.length());
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("Transmitting..."));
  display.printf("%d bits\n", data.length());
  display.display();
  
  // Make sure we're in TX mode
  strobeTX(0x35, activeModule); // STX
  
  // Transmit each bit
  for(int i = 0; i < data.length(); i++) {
    if(data[i] == '1') {
      digitalWrite(txPin, HIGH);
    } else {
      digitalWrite(txPin, LOW);
    }
    delayMicroseconds(1000); // 1ms per bit
  }
  
  digitalWrite(txPin, LOW);
  
  Serial.println(F("[TX] Transmission complete"));
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("TX Complete!"));
  display.display();
  delay(1000);
}

void sendLastRX() {
  if(TX_DEMO_MODE) {
    Serial.println(F("[TX] Demo mode - Last RX"));
    runDemoTX("TX LAST RX");
    return;
  }
  
  // Original code...
  if(samplecount == 0) {
    debugPrint("No RX data!", true, true, 2000);
    Serial.println(F("[TX] No received data to transmit"));
    return;
  }
  
  Serial.println(F("\n[TX] ===== TRANSMIT LAST RX ====="));
  Serial.printf("[TX] Transmitting %d samples\n", samplecount);
  
  if(!txActive) {
    startTX();
  }
  
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ TX LAST RX ]="));
  display.println(F(""));
  display.printf("Samples: %d\n", samplecount);
  display.printf("Freq: %.2f MHz\n", frequency);
  display.println(F(""));
  display.println(F("Transmitting..."));
  display.display();
  
  // Make sure we're in TX mode
  strobeTX(0x35, activeModule); // STX
  
  // Transmit the captured signal 3 times
  for(int repeat = 0; repeat < 3; repeat++) {
    Serial.printf("[TX] Transmission %d/3\n", repeat + 1);
    
    for(int i = 0; i < samplecount; i += 2) {
      digitalWrite(txPin, HIGH);
      delayMicroseconds(sample[i]);
      
      if(i + 1 < samplecount) {
        digitalWrite(txPin, LOW);
        delayMicroseconds(sample[i + 1]);
      }
    }
    
    digitalWrite(txPin, LOW);
    delay(500); // Delay between transmissions
  }
  
  Serial.println(F("[TX] Transmission complete"));
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("TX Complete!"));
  display.println(F("3 transmissions"));
  display.display();
  delay(1500);
  
  stopTX();
}

void sendRawData(long *data, int count, int transmissions) {
  if(TX_DEMO_MODE) {
    Serial.printf("[TX DEMO] Would send %d data points, %d times\n", count, transmissions);
    runDemoTX("RAW DATA");
    return;
  }
  
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[TX] ===== RAW DATA TX ====="));
  Serial.printf("[TX] Data points: %d, Transmissions: %d\n", count, transmissions);
  
  if(!txActive) {
    startTX();
  }
  
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ RAW TX ]="));
  display.println(F(""));
  display.printf("Points: %d\n", count);
  display.printf("Repeat: %dx\n", transmissions);
  display.display();
  
  for (int r = 0; r < transmissions; r++) {
    Serial.printf("[TX] Transmission %d/%d\n", r + 1, transmissions);
    
    display.setCursor(0, 40);
    display.printf("TX: %d/%d", r + 1, transmissions);
    display.display();
    
    for (int i = 0; i < count; i += 2) {
      digitalWrite(txPin, HIGH);
      delayMicroseconds(data[i]);
      digitalWrite(txPin, LOW);
      if(i + 1 < count) {
        delayMicroseconds(data[i + 1]);
      }
    }
    
    delay(2000); // Delay between retransmissions
  }
  
  Serial.println(F("[TX] Raw data transmission complete"));
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("TX Complete!"));
  display.display();
  delay(1500);
  
  stopTX();
}

void loadTXFromFile() {
  if(TX_DEMO_MODE) {
    Serial.println(F("[TX] Demo mode - TX from file"));
    runDemoTX("TX FROM FILE");
    return;
  }
  
  // Original code...
  if(!sdCardPresent) {
    debugPrint("No SD card!", true, true, 2000);
    return;
  }
  
  Serial.println(F("\n[TX] ===== TX FROM FILE ====="));
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ TX FILES ]="));
  display.println(F(""));
  
  // List RX files
  File root = SD_MMC.open("/");
  File file = root.openNextFile();
  int fileCount = 0;
  String fileNames[20];
  
  while(file && fileCount < 20) {
    String name = file.name();
    if(name.startsWith("rx_")) {
      fileNames[fileCount] = name;
      fileCount++;
      
      if(fileCount <= 5) {
        display.println(name);
      }
    }
    file = root.openNextFile();
  }
  
  if(fileCount == 0) {
    display.println(F("No RX files found"));
    display.display();
    delay(2000);
    return;
  }
  
  display.display();
  Serial.printf("[TX] Found %d RX files\n", fileCount);
  
  // For now, just use the first file
  // TODO: Add file selection menu
  if(fileCount > 0) {
    loadAndTransmitFile("/" + fileNames[0]);
  }
}

void loadAndTransmitFile(String filename) {
  if(TX_DEMO_MODE) {
    Serial.printf("[TX DEMO] Would load file: %s\n", filename.c_str());
    runDemoTX("FILE TX");
    return;
  }
  
  Serial.printf("[TX] Loading file: %s\n", filename.c_str());
  
  File dataFile = SD_MMC.open(filename, FILE_READ);
  if(!dataFile) {
    debugPrint("File open failed!", true, true, 2000);
    return;
  }
  
  // Parse file
  float fileFreq = frequency;
  int fileMod = mod;
  int fileCount = 0;
  
  while(dataFile.available()) {
    String line = dataFile.readStringUntil('\n');
    
    if(line.startsWith("Frequency:")) {
      fileFreq = line.substring(11).toFloat();
      Serial.printf("[TX] File frequency: %.2f MHz\n", fileFreq);
    }
    else if(line.startsWith("Modulation:")) {
      fileMod = line.substring(12).toInt();
      Serial.printf("[TX] File modulation: %d\n", fileMod);
    }
    else if(line.startsWith("Count:")) {
      fileCount = line.substring(7).toInt();
      Serial.printf("[TX] File sample count: %d\n", fileCount);
    }
    else if(line.startsWith("Data:")) {
      // Parse timing data
      String dataStr = line.substring(6);
      int idx = 0;
      int dataIdx = 0;
      
      while(idx < dataStr.length() && dataIdx < 2000) {
        int commaPos = dataStr.indexOf(',', idx);
        if(commaPos == -1) commaPos = dataStr.length();
        
        String valStr = dataStr.substring(idx, commaPos);
        data_to_send[dataIdx++] = valStr.toInt();
        
        idx = commaPos + 1;
      }
      
      data_count = dataIdx;
      Serial.printf("[TX] Loaded %d data points\n", data_count);
    }
  }
  
  dataFile.close();
  
  // Set frequency and modulation from file
  frequency = fileFreq;
  mod = fileMod;
  
  // Transmit the data
  sendRawData(data_to_send, data_count, 3);
}

void drawTXBinaryMenu() {
  Serial.println(F("[Display] Drawing TX Binary menu"));
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("=[ BINARY TX ]="));
  display.println(F(""));
  display.println(F("1. Manual Input"));
  display.println(F("2. Test Pattern"));
  display.println(F("3. From File"));
  display.println(F("4. Back"));
  
  display.setCursor(0, 55);
  display.print(F("Select option"));
  display.display();
}

void sendTestPattern() {
  if(TX_DEMO_MODE) {
    // Demo version with just animations
    Serial.println(F("[TX DEMO] Test pattern demo"));
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("=[ TEST DEMO ]="));
    display.println(F(""));
    
    const char* patterns[] = {
      "10101010", 
      "11110000",
      "10011001",
      "11111111"
    };
    
    for(int p = 0; p < 4; p++) {
      display.fillRect(0, 20, 128, 44, SH110X_BLACK);
      display.setCursor(0, 20);
      display.printf("Pattern %d/4\n", p + 1);
      display.println(patterns[p]);
      
      // Draw pattern visually
      for(int i = 0; i < 8; i++) {
        int x = i * 16;
        if(patterns[p][i] == '1') {
          display.fillRect(x, 40, 14, 20, SH110X_WHITE);
        } else {
          display.drawRect(x, 40, 14, 20, SH110X_WHITE);
        }
      }
      display.display();
      
      // LED pattern
      for(int i = 0; i < NEOPIXEL_COUNT; i++) {
        if(patterns[p][i % 8] == '1') {
          pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        } else {
          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        }
      }
      pixels.show();
      delay(1000);
    }
    
    pixels.clear();
    pixels.show();
    
    display.clearDisplay();
    display.setCursor(0,20);
    display.println(F("Demo Complete!"));
    display.display();
    delay(1500);
    return;
  }
  
  // Original code...
  Serial.println(F("\n[TX] ===== TEST PATTERN TX ====="));
  
  if(!cc1101APresent && !cc1101BPresent) {
    debugPrint("No CC1101 found!", true, true, 2000);
    return;
  }
  
  if(!txActive) {
    startTX();
  }
  
  int txPin = (activeModule == 0) ? CC1101_GDO0_A : CC1101_GDO0_B;
  
  // Common test patterns
  const char* patterns[] = {
    "10101010101010101010", // Alternating
    "11110000111100001111", // Square wave
    "10011001100110011001", // Pattern
    "11111111000000001111"  // Long pulse
  };
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("=[ TEST PATTERN ]="));
  display.println(F(""));
  
  // Make sure we're in TX mode
  strobeTX(0x35, activeModule); // STX
  
  for(int p = 0; p < 4; p++) {
    display.setCursor(0, 20);
    display.printf("Pattern %d/4\n", p + 1);
    display.println(patterns[p]);
    display.display();
    
    Serial.printf("[TX] Sending pattern %d: %s\n", p + 1, patterns[p]);
    
    // Send pattern 5 times
    for(int r = 0; r < 5; r++) {
      for(int i = 0; patterns[p][i] != '\0'; i++) {
        digitalWrite(txPin, patterns[p][i] == '1' ? HIGH : LOW);
        delayMicroseconds(1000);
      }
      digitalWrite(txPin, LOW);
      delay(100);
    }
    
    delay(1000);
  }
  
  Serial.println(F("[TX] Test patterns complete"));
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println(F("Patterns sent!"));
  display.display();
  delay(1500);
  
  stopTX();
}