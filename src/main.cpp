// =============================================================================
//  main.cpp
//  Wires the subsystems together and runs a fixed-timestep (~60 FPS) loop.
//  Nothing here blocks: every subsystem advances by the frame's dt and the two
//  eye sprites are pushed once per frame.
//
//  Data flow each frame:
//     Serial commands -> EmotionManager (shape) + AnimationManager (motion)
//     BlinkSystem (openness) + AnimationManager (gaze/bob/wink/tilt/intro)
//                         |
//                         v
//     resolve per-eye EyeState -> EyeRenderer -> sprite -> DisplayManager
// =============================================================================
#include <Arduino.h>
#include "Config.h"
#include "DisplayManager.h"
#include "EyeRenderer.h"
#include "EmotionManager.h"
#include "AnimationManager.h"
#include "BlinkSystem.h"
#include "SerialCommands.h"

DisplayManager   display;
EmotionManager   emotion;
AnimationManager anim;
BlinkSystem      blink;
SerialCommands   serialCmd(emotion, anim, blink, display);

// Precomputed on-screen eye centres (landscape).
static int gLeftCx, gRightCx, gEyeCy;

// Timing for the fixed-step loop.
static uint32_t gLastMicros = 0;

// Build a fully-resolved EyeState for one eye from the current subsystem state.
static EyeState resolveEye(Side side) {
    EyeState st;
    st.p    = emotion.params();
    st.side = side;
    st.gazeX = anim.gazeX();
    st.gazeY = anim.gazeY();

    float wink  = (side == Side::LEFT) ? anim.winkL() : anim.winkR();
    float tilt  = (side == Side::LEFT) ? anim.tiltL() : anim.tiltR();
    st.blink = blink.openness() * wink * anim.introOpen();
    st.bob   = anim.bob() + tilt;
    return st;
}

void setup() {
    Serial.begin(115200);
    randomSeed(micros());

    display.begin();
    display.showBoot();          // AURA logo + loading dots splash
    display.clearScreen();

    // Eye positions: symmetric about screen centre.
    int cx = Cfg::SCREEN_W / 2;
    int half = (int)(Cfg::EYE_GAP / 2 + Cfg::EYE_W / 2);
    gLeftCx  = cx - half;
    gRightCx = cx + half;
    gEyeCy   = Cfg::SCREEN_H / 2;

    emotion.setEmotion(EmotionManager::NORMAL, 1);
    anim.boot();                 // grow the eyes in on power-up
    gLastMicros = micros();
}

void loop() {
    // ---- Fixed timestep gate ----
    uint32_t now = micros();
    uint32_t dtUs = now - gLastMicros;
    if (dtUs < Cfg::FRAME_US) return;   // not time for a new frame yet
    gLastMicros = now;
    float dtMs = dtUs / 1000.0f;

    // ---- Update subsystems ----
    serialCmd.poll();
    emotion.update(dtMs);
    blink.setRateScale(emotion.blinkRateScale());
    blink.update(dtMs);
    anim.update(dtMs, emotion.gazeEnergy(), blink);

    // ---- Resolve, render, present ----
    EyeState left  = resolveEye(Side::LEFT);
    EyeState right = resolveEye(Side::RIGHT);
    EyeRenderer::render(display.leftSprite(),  left);
    EyeRenderer::render(display.rightSprite(), right);
    display.present(gLeftCx, gEyeCy, gRightCx, gEyeCy);
}
