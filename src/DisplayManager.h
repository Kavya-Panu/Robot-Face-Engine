// =============================================================================
//  DisplayManager.h
//  Owns the TFT_eSPI driver and TWO off-screen sprites (one per eye). All eye
//  drawing happens into these sprites which are then pushed to the panel, so
//  the visible screen never shows a half-drawn frame (no flicker). Only the
//  eye regions are redrawn each frame, which keeps memory small and FPS high.
// =============================================================================
#pragma once
#include <TFT_eSPI.h>
#include "Config.h"

class DisplayManager {
public:
    DisplayManager();

    // Initialise panel, backlight and both eye sprites. Call once in setup().
    void begin();
    void showBoot();

    // Access to the two 16-bit sprite canvases the EyeRenderer draws into.
    TFT_eSprite& leftSprite()  { return _left; }
    TFT_eSprite& rightSprite() { return _right; }

    // Push both eye sprites to their screen positions in a single SPI burst.
    // cx/cy are the on-screen centres of each eye (px).
    void present(int leftCx, int leftCy, int rightCx, int rightCy);

    // Paint the whole background once (e.g. at boot or when clearing).
    void clearScreen();

    // Set backlight brightness 0..255 (used for low-battery dimming etc.).
    void setBrightness(uint8_t level);

    int width()  const { return Cfg::SCREEN_W; }
    int height() const { return Cfg::SCREEN_H; }

private:
    TFT_eSPI   _tft;
    TFT_eSprite _left;
    TFT_eSprite _right;
};
