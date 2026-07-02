// =============================================================================
//  DisplayManager.cpp
// =============================================================================
#include "DisplayManager.h"

// Construct the sprites bound to our TFT instance.
DisplayManager::DisplayManager()
    : _tft(), _left(&_tft), _right(&_tft) {}

// Bring up the panel, backlight (via LEDC PWM) and allocate the eye sprites.
void DisplayManager::begin() {
    _tft.init();
    _tft.setRotation(Cfg::ROTATION);
    _tft.invertDisplay(Cfg::INVERT_DISPLAY);
    _tft.fillScreen(Cfg::BG_COLOR);

    // Backlight via analogWrite so brightness works across ESP32 core versions.
    pinMode(Cfg::BACKLIGHT_PIN, OUTPUT);
    analogWrite(Cfg::BACKLIGHT_PIN, 255);  // full brightness

    // 16-bit colour sprites, one per eye. ~130*150*2 = ~39 KB each.
    _left.setColorDepth(16);
    _right.setColorDepth(16);
    _left.createSprite(Cfg::SPRITE_W, Cfg::SPRITE_H);
    _right.createSprite(Cfg::SPRITE_W, Cfg::SPRITE_H);
}

void DisplayManager::clearScreen() {
    _tft.fillScreen(Cfg::BG_COLOR);
}

// Small non-delay() wait that keeps the watchdog happy.
static void hold(uint32_t ms) {
    uint32_t t0 = millis();
    while (millis() - t0 < ms) { yield(); }
}

// Boot splash: power-on -> "AURA" logo -> animated loading dots -> clear.
// The eye engine then takes over and does the eyes-open / blink / idle part.
void DisplayManager::showBoot() {
    const int cx = Cfg::SCREEN_W / 2;
    const int cy = Cfg::SCREEN_H / 2;

    _tft.fillScreen(Cfg::BG_COLOR);
    hold(300);                                   // power-on beat

    _tft.setTextDatum(MC_DATUM);
    _tft.setTextColor(Cfg::LOGO_COLOR, Cfg::BG_COLOR);
    _tft.setTextSize(5);
    _tft.drawString("AURA", cx, cy - 12);        // logo
    hold(900);

    _tft.setTextSize(3);                          // loading dots
    for (int i = 1; i <= 3; i++) {
        String dots;
        for (int j = 0; j < i; j++) dots += ".";
        _tft.fillRect(0, cy + 30, Cfg::SCREEN_W, 40, Cfg::BG_COLOR);
        _tft.drawString(dots, cx, cy + 40);
        hold(280);
    }
    hold(200);
    _tft.fillScreen(Cfg::BG_COLOR);
}

void DisplayManager::setBrightness(uint8_t level) {
    analogWrite(Cfg::BACKLIGHT_PIN, level);
}

// Push both sprites inside one write transaction for speed and tear-free output.
// Sprites are positioned so that (leftCx,leftCy) is the CENTRE of the left eye.
void DisplayManager::present(int leftCx, int leftCy, int rightCx, int rightCy) {
    const int halfW = Cfg::SPRITE_W / 2;
    const int halfH = Cfg::SPRITE_H / 2;
    _tft.startWrite();
    _left.pushSprite(leftCx  - halfW, leftCy  - halfH);
    _right.pushSprite(rightCx - halfW, rightCy - halfH);
    _tft.endWrite();
}
