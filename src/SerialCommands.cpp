// =============================================================================
//  SerialCommands.cpp
// =============================================================================
#include "SerialCommands.h"

// Read all available bytes; run dispatch() whenever a full line arrives.
void SerialCommands::poll() {
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n' || c == '\r') {
            if (_buf.length()) { _buf.trim(); dispatch(_buf); }
            _buf = "";
        } else if (_buf.length() < 64) {
            _buf += c;
        }
    }
}

void SerialCommands::dispatch(const String& raw) {
    // Split into an uppercase command and up to two numeric arguments.
    String line = raw; line.toUpperCase();
    int sp = line.indexOf(' ');
    String cmd = (sp < 0) ? line : line.substring(0, sp);
    String rest = (sp < 0) ? ""   : line.substring(sp + 1);

    // Any command other than SEARCH cancels the search sweep.
    if (cmd != "SEARCH") _am.setSearching(false);
    // Head tilt only persists for CURIOUS / CONFUSED.
    if (cmd != "CURIOUS" && cmd != "CONFUSED") _am.setHeadTilt(0);

    // ---- Emotions ----------------------------------------------------------
    if      (cmd == "NORMAL")    _em.setEmotion(EmotionManager::NORMAL);
    else if (cmd == "HAPPY")     _em.setEmotion(EmotionManager::HAPPY);
    else if (cmd == "EXCITED")   _em.setEmotion(EmotionManager::EXCITED);
    else if (cmd == "SAD")       _em.setEmotion(EmotionManager::SAD);
    else if (cmd == "ANGRY")     _em.setEmotion(EmotionManager::ANGRY);
    else if (cmd == "SURPRISED") _em.setEmotion(EmotionManager::SURPRISED);
    else if (cmd == "CONFUSED") { _em.setEmotion(EmotionManager::CONFUSED); _am.setHeadTilt(10); }
    else if (cmd == "WORRIED")  { _em.setEmotion(EmotionManager::WORRIED);  _am.lookAt(0, 0.5f); }
    else if (cmd == "CURIOUS")  { _em.setEmotion(EmotionManager::CURIOUS);  _am.setHeadTilt(12); }
    else if (cmd == "LOVE")      _em.setEmotion(EmotionManager::LOVE);
    else if (cmd == "THINK")    { _em.setEmotion(EmotionManager::THINKING); _am.lookAt(0.4f, -1); }
    else if (cmd == "LISTEN")    _em.setEmotion(EmotionManager::LISTENING);
    else if (cmd == "ERROR")     _em.setEmotion(EmotionManager::ERROR);

    // ---- Gaze --------------------------------------------------------------
    else if (cmd == "LOOK_LEFT")  _am.lookAt(-1,  0);
    else if (cmd == "LOOK_RIGHT") _am.lookAt( 1,  0);
    else if (cmd == "LOOK_UP")    _am.lookAt( 0, -1);
    else if (cmd == "LOOK_DOWN")  _am.lookAt( 0,  1);
    else if (cmd == "CENTER")     _am.lookAt( 0,  0);
    else if (cmd == "GAZE") {
        int s2 = rest.indexOf(' ');
        if (s2 > 0) _am.lookAt(rest.substring(0, s2).toFloat(),
                               rest.substring(s2 + 1).toFloat());
    }

    // ---- Sleep / wake ------------------------------------------------------
    else if (cmd == "SLEEP")  _em.setEmotion(EmotionManager::SLEEP, 600);
    else if (cmd == "WAKE")  { _em.setEmotion(EmotionManager::NORMAL, 500); _am.wake(); }

    // ---- One-shots ---------------------------------------------------------
    else if (cmd == "WINK" || cmd == "WINK_LEFT")  _am.wink(Side::LEFT);
    else if (cmd == "WINK_RIGHT")                  _am.wink(Side::RIGHT);
    else if (cmd == "BLINK")   _bs.trigger(BlinkSystem::FAST);
    else if (cmd == "SEARCH") { _em.setEmotion(EmotionManager::SEARCHING); _am.setSearching(true); }

    // ---- System states -----------------------------------------------------
    else if (cmd == "BATTERY_LOW") { _em.setEmotion(EmotionManager::SLEEPY); _dm.setBrightness(60); }
    else if (cmd == "CHARGING")    { _em.setEmotion(EmotionManager::HAPPY);  _dm.setBrightness(255); }
    else if (cmd == "BOOT")       { _dm.setBrightness(255); _em.setEmotion(EmotionManager::NORMAL, 200); _am.boot(); }
    else if (cmd == "SHUTDOWN")   { _em.setEmotion(EmotionManager::SLEEP, 500); _dm.setBrightness(20); }

    // Unknown command -> ignored (do nothing, no crash).
}
