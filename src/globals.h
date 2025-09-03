#ifndef GLOBALS_H
#define GLOBALS_H

#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include <WiFi.h>
#include "esp_task_wdt.h"
#include <EEPROM.h>
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>

// Pin Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define CC1101_MISO_A 14
#define CC1101_MOSI_A 13
#define CC1101_CLK_A 12
#define CC1101_CS_A 15
#define CC1101_GDO0_A 47

#define CC1101_MISO_B 18
#define CC1101_MOSI_B 17
#define CC1101_CLK_B 16
#define CC1101_CS_B 5
#define CC1101_GDO0_B 48

#define TX_PIN_A CC1101_GDO0_A
#define TX_PIN_B CC1101_GDO0_B

#define BTN_UP 4
#define BTN_DOWN 7
#define BTN_LEFT 9
#define BTN_RIGHT 10
#define BTN_SELECT 11

#define NEOPIXEL_PIN 21
#define NEOPIXEL_COUNT 32

#define SD_CMD 35
#define SD_CLK 36
#define SD_DAT0 37
#define SD_DAT1 38
#define SD_DAT2 39
#define SD_DAT3 40

#define EEPROM_SIZE 4096
#define SAMPLE_SIZE 2000

// Global objects
extern Adafruit_SH1106G display;
extern Adafruit_NeoPixel pixels;

// Common frequencies
extern const float commonFrequencies[];
extern const int numFrequencies;

// Menu structure
enum MenuState {
  MENU_MAIN,
  MENU_RX,
  MENU_TX,
  MENU_JAMMER,
  MENU_TESLA,
  MENU_SETTINGS,
  MENU_ABOUT,
  MENU_CREDITS,
  MENU_RX_CONFIG,
  MENU_TX_CONFIG,
  MENU_TX_BINARY,
  MENU_FILES,
  MENU_DISPLAY_SETTINGS,
  MENU_LED_PATTERN,
  MENU_LED_BRIGHTNESS,
  MENU_FREQUENCY_SELECT,
  MENU_TESLA_CONFIG
};

// Display Settings
enum IdlePattern {
  PATTERN_BREATHE,
  PATTERN_RAINBOW,
  PATTERN_SCANNER,
  PATTERN_SPARKLE,
  PATTERN_FIRE,
  PATTERN_OCEAN,
  PATTERN_MATRIX,
  PATTERN_SUPERGAY,
  PATTERN_SOLID_RED,
  PATTERN_SOLID_GREEN,
  PATTERN_SOLID_BLUE,
  PATTERN_SOLID_WHITE,
  PATTERN_SOLID_MAGENTA,
  PATTERN_SOLID_CYAN,
  PATTERN_SOLID_YELLOW,
  PATTERN_SOLID_ORANGE,
  PATTERN_NEON,
  PATTERN_PLASMA,
  PATTERN_METEOR,
  PATTERN_TWINKLE,
  PATTERN_VAPORWAVE,
  PATTERN_POLICE,
  PATTERN_PIRATE,
  PATTERN_THEATER,
  PATTERN_RUNNING,
  PATTERN_BOUNCING,
  PATTERN_HEARTBEAT,
  PATTERN_STROBE,
  PATTERN_CYLON,
  PATTERN_SNOW,
  PATTERN_PACIFICA,
  PATTERN_LAVA,
  PATTERN_LIGHTNING,
  PATTERN_CONFETTI,
  PATTERN_OFF
};

enum PixelMode {
  PIXEL_OFF,
  PIXEL_IDLE,
  PIXEL_RX,
  PIXEL_TX,
  PIXEL_JAMMER,
  PIXEL_TESLA,
  PIXEL_MENU
};

// Global variables
extern MenuState currentMenu;
extern MenuState previousMenu;
extern int menuSelection;
extern int menuOffset;
extern const int maxMenuItems;

extern IdlePattern idlePattern;
extern uint8_t ledBrightness;
extern PixelMode pixelMode;
extern unsigned long lastPixelUpdate;
extern uint8_t pixelHue;

extern String raw_rx;
extern String jammer_tx;
extern unsigned long sample[];
extern int samplecount;
extern float frequency;
extern float teslaFrequency;
extern int mod;
extern float deviation;
extern float setrxbw;
extern int datarate;
extern byte activeModule;
extern bool sdCardPresent;
extern bool cc1101APresent;
extern bool cc1101BPresent;

extern unsigned long lastButtonPress;
extern const unsigned long buttonDebounce;

// Menu navigation
extern int frequencyIndex;

// Tesla configuration
extern int teslaTransmissions;
extern int teslaDelay;

// Jammer configuration
extern int jammerPower;
extern bool jammerSweep;

// LED Pattern preview state
extern IdlePattern previewPattern;
extern IdlePattern savedPattern;
extern bool inPatternPreview;

// Credits
extern unsigned long lastCreditScroll;
extern int creditScrollPos;
extern const char* creditLines[];
extern const int numCreditLines;

// Function declarations

// Core functions
void debugPrint(String message, bool serial = true, bool screen = true, int displayTime = 1500);
void updateDisplay();
void handleButtons();
void showStartupAnimation();
const char* getModulationName(int modulation);

// Hardware functions
void initializeHardware();
void initializeButtons();
void initializeSDCard(bool displayOK);
void initializeCC1101(bool displayOK);
void showHardwareSummary(bool displayOK);
void testCC1101SPI();

// Menu functions
void handleMenuUp();
void handleMenuDown();
void handleMenuBack();
void handleMenuSelect();
int getMaxMenuItems();

// Display functions
void drawMainMenu();
void drawRXMenu();
void drawTXMenu();
void drawJammerMenu();
void drawTeslaMenu();
void drawSettingsMenu();
void drawAbout();
void drawCredits();
void drawRXConfig();
void drawTXConfig();
void drawDisplaySettings();
void drawLEDPatternMenu();
void drawBrightnessMenu();
void drawFrequencySelect();
void drawTeslaConfig();
void drawMenuItems(const char* items[], int itemCount);

// RX functions
void startRX();
void stopRX();
void handleRXMode();
void cycleRXVisualization();
void enableReceive();
bool checkReceived(void);
void receiver();
void printReceived();
void signalanalyse();
void updateWaterfall();
void drawWaterfall();
void drawRFMatrix();
void drawRFScope();
void drawSignalMeter();
void updateRXPixels();
void rxScannerEffect(int signalBrightness);

// TX functions
void startTX();
void stopTX();
void sendBinaryData();
void transmitBinaryString(String data);
void sendLastRX();
void sendRawData(long *data, int count, int transmissions);
void loadTXFromFile();
void drawTXBinaryMenu();
void sendTestPattern();

// Jammer functions
void startJammer();
void stopJammer();
void runJammer();
void updateJammerDisplay();
void configureJammer();
void adjustJammerFrequency();
void adjustJammerPower();
void setupJammerSweep();

// Tesla functions
void sendTeslaSignal();
void sendByte(uint8_t dataByte, int txPin);
void animateTeslaDelay(int delayTime);
void teslaSuccessAnimation();
void configureTesla();
void sendTeslaRepeat();
void analyzeTeslaSignal();

// Pixel functions
void updatePixels();
void breatheAnimation(uint8_t r, uint8_t g, uint8_t b);
void scannerAnimation(uint8_t r, uint8_t g, uint8_t b);
void pulseAnimation(uint8_t r, uint8_t g, uint8_t b);
void chaosAnimation();
void lightningAnimation();
void sparkleAnimation();
void fireEffect();
void oceanWave();
void matrixRain();
void rainbowCycle();
void successAnimation();
void errorAnimation();
void bootAnimation();
void signalStrengthLEDs(int rssi);

// Settings functions
void saveSettings();
void loadSettings();
void setDefaultSettings();
void validateSettings();
uint16_t calculateChecksum();
void resetToFactory();
void exportSettings();

// File operations
void initializeFileSystem();
void createDirectory(const char* path);
void startNewLogFile();
void logEvent(const char* event, const char* details = "");
void saveRXData(const char* filename);
void listDirectory(const char* dirname, uint8_t levels);
void displayFileMenu();
void viewFiles(const char* path);
void fileOptions(const char* filename);
void viewFileContent(const char* filename);
void deleteFileConfirm(const char* filename);
void deleteFile(fs::FS &fs, const char* path);
void showCardInfo();
void appendFile(fs::FS &fs, const char* path, const char* message, String messagestring = "");
void appendFileLong(fs::FS &fs, const char* path, unsigned long messagechar);

#endif