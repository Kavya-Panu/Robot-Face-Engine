// =============================================================================
//  SerialCommands.h
//  Reads newline-terminated text commands from the laptop over Serial and
//  dispatches them to the managers. Non-blocking (drains the RX buffer each
//  call). Unknown commands are ignored safely. Also accepts:
//     GAZE <nx> <ny>   with nx,ny in [-1,1]  (for MediaPipe face tracking)
// =============================================================================
#pragma once
#include <Arduino.h>
#include "EmotionManager.h"
#include "AnimationManager.h"
#include "BlinkSystem.h"
#include "DisplayManager.h"

class SerialCommands {
public:
    SerialCommands(EmotionManager& em, AnimationManager& am,
                   BlinkSystem& bs, DisplayManager& dm)
        : _em(em), _am(am), _bs(bs), _dm(dm) {}

    // Call every loop. Reads any waiting bytes and executes complete lines.
    void poll();

private:
    // Execute one complete, trimmed command line.
    void dispatch(const String& line);

    EmotionManager&  _em;
    AnimationManager& _am;
    BlinkSystem&     _bs;
    DisplayManager&  _dm;
    String _buf;
};
