// =============================================================================
//  Config.h
//  Single place for every tunable value in the face engine: board selection,
//  screen geometry, colours, eye shape defaults, and animation timings.
//  Change appearance/behaviour here without touching the engine code.
// =============================================================================
#pragma once
#include <Arduino.h>

// ---- 16-bit RGB565 colour helper -------------------------------------------
#define RGB565(r, g, b) ((uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)))

// =============================================================================
//  BOARD SELECTION
//  Exactly one of these must match your build environment in platformio.ini.
//  Default = the LCDWIKI E32R28T board from your datasheet.
// =============================================================================
#define BOARD_ILI9341_240x320       // <-- your current board
// #define BOARD_S3_ST7796           // <-- uncomment if you switch boards

namespace Cfg {

// ---- Screen geometry -------------------------------------------------------
#if defined(BOARD_S3_ST7796)
  constexpr int   SCREEN_W = 480;      // landscape width
  constexpr int   SCREEN_H = 320;      // landscape height
  constexpr uint8_t ROTATION = 1;
#else // BOARD_ILI9341_240x320
  constexpr int   SCREEN_W = 320;      // landscape width  (native 240x320 rotated)
  constexpr int   SCREEN_H = 240;      // landscape height
  constexpr uint8_t ROTATION = 1;      // try 3 if the image is upside-down
#endif

// Some ILI9341 panels show inverted colours. Flip this if red/blue look wrong.
constexpr bool INVERT_DISPLAY = false;

// ---- Backlight -------------------------------------------------------------
constexpr uint8_t BACKLIGHT_PIN = 21;  // matches TFT_BL; used for brightness

// ---- Frame rate ------------------------------------------------------------
constexpr int     TARGET_FPS   = 60;
constexpr uint32_t FRAME_US    = 1000000UL / TARGET_FPS;

// ---- Colours (AURA default = cyan glowing eyes on deep black) --------------
// For a warm look, try EYE_COLOR = RGB565(255,180,60). Everything else follows.
constexpr uint16_t BG_COLOR    = RGB565(2, 4, 8);       // deep black
constexpr uint16_t EYE_COLOR   = RGB565(40, 205, 255);  // AURA cyan glow
constexpr uint16_t PUPIL_COLOR = RGB565(10, 12, 18);    // (legacy, unused)
constexpr uint16_t LOVE_COLOR  = RGB565(255, 80, 120);  // heart-eyes pink
constexpr uint16_t ERROR_COLOR = RGB565(255, 70, 70);   // error state eyes
constexpr uint16_t LOGO_COLOR  = RGB565(40, 205, 255);  // boot "AURA" text

// ---- Eye geometry (base "NORMAL" look, in pixels) --------------------------
constexpr float EYE_W       = 86.0f;   // eye width
constexpr float EYE_H       = 90.0f;   // eye height
constexpr float EYE_RADIUS  = 30.0f;   // corner radius (rounded square)
constexpr float EYE_GAP     = 50.0f;   // gap between the two eyes
constexpr float PUPIL_SCALE = 0.42f;   // (legacy, unused in blob model)
constexpr float PUPIL_RANGE = 18.0f;   // max whole-eye travel from centre (px)

// Per-eye sprite box. Must be big enough to hold the largest eye + travel.
constexpr int SPRITE_W = 130;
constexpr int SPRITE_H = 150;

// ---- Blink timing (milliseconds) -------------------------------------------
constexpr uint32_t BLINK_MIN_GAP  = 2200;  // shortest idle wait between blinks
constexpr uint32_t BLINK_MAX_GAP  = 6000;  // longest idle wait between blinks
constexpr uint32_t BLINK_FAST_MS  = 130;   // duration of a quick blink
constexpr uint32_t BLINK_SLOW_MS  = 380;   // duration of a slow/sleepy blink

// ---- Idle gaze / micro-movement --------------------------------------------
constexpr uint32_t SACCADE_MIN_GAP = 1400;  // wait between look-around moves
constexpr uint32_t SACCADE_MAX_GAP = 4200;
constexpr uint32_t SACCADE_MS      = 220;   // time to move the eyes to a target
constexpr float    MICRO_JITTER    = 2.5f;  // tiny constant eye tremor (px)

// ---- Breathing (subtle idle "alive" motion) --------------------------------
constexpr uint32_t BREATH_PERIOD_MS = 3800; // one full breath cycle
constexpr float    BREATH_AMPLITUDE = 4.0f; // vertical bob (px)

} // namespace Cfg
