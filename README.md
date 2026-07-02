# Companion Robot Face — Modular Eye Engine (ESP32)

A non-blocking, OOP graphics engine that renders expressive animated eyes on a
TFT display. The ESP32 only draws the face; a laptop sends text commands over
Serial to drive emotions and gaze.

---

##  Read this first — hardware mismatch

Your written spec said **ESP32-S3 + ST7796 320×480**, but the datasheet you
uploaded is the **LCDWIKI E32R28T**, which is:

| | Your spec | |
|---|---|---|
| MCU |  **ESP32-WROOM-32E** |
| Driver |  **ILI9341** |
| Resolution | **240×320** (320×240 landscape) |

This project is built for **the board you actually have** (ILI9341 240×320).
If you want you switch to any board like S3/ST7796 board, see *Switching boards* below — it's a
4-line change because all hardware specifics live in `Config.h` / `platformio.ini`.

---

## Build & flash (PlatformIO)

```bash
# from the project root
pio run                    # compile
pio run --target upload    # flash (board on USB)
pio device monitor         # open serial monitor @115200
```

TFT_eSPI is configured entirely through `build_flags` in `platformio.ini`, so
you do **not** need to hand-edit `User_Setup.h` like the LCDWIKI PDF describes.
(If the panel shows wrong colours, set `INVERT_DISPLAY = true` in `Config.h`;
if it's upside-down, change `ROTATION` from 1 to 3.)

Pins come straight from your datasheet's pin table: CS=15, DC=2, SCK=14,
MOSI=13, MISO=12, RST=EN(-1), backlight=21.

---

## Architecture

| File | Responsibility |
|---|---|
| `Config.h` | Every tunable: geometry, colours, timings, board select |
| `Easing.h` | Easing curves, `lerp`, and the `Tween` class (nothing teleports) |
| `EyeState.h` | Shared structs: `EyeParams` (shape) and `EyeState` (resolved frame) |
| `DisplayManager` | TFT + two per-eye sprites; tear-free push |
| `EyeRenderer` | Draws one eye (anti-aliased) from an `EyeState` |
| `BlinkSystem` | Independent, interruptible blinking (fast/slow/double/sleep) |
| `EmotionManager` | Emotion state machine; cross-fades between shape presets |
| `AnimationManager` | Idle saccades, micro-tremor, breathing, winks, tilt, one-shots |
| `SerialCommands` | Parses laptop commands; ignores unknown safely |
| `main.cpp` | Fixed-timestep ~60 FPS loop that wires it all together |

**Why two per-eye sprites instead of one full-screen buffer?** A 320×240×16-bit
back buffer is ~150 KB — risky on a WROOM-32 with no PSRAM. Two eye-sized
sprites (~39 KB each) give the same flicker-free result, use far less RAM, and
only redraw the eye regions, which keeps the frame rate high.

---

## Serial command reference

Send newline-terminated ASCII. Unknown commands are ignored.

```
Emotions : NORMAL HAPPY EXCITED SAD ANGRY SURPRISED CONFUSED
           CURIOUS LOVE THINK LISTEN ERROR WORRIED
Gaze     : LOOK_LEFT LOOK_RIGHT LOOK_UP LOOK_DOWN CENTER
           GAZE <nx> <ny>      (nx,ny in -1..1, for face tracking)
Sleep    : SLEEP  WAKE
One-shots: WINK  WINK_LEFT  WINK_RIGHT  BLINK  SEARCH
System   : BATTERY_LOW  CHARGING  BOOT  SHUTDOWN
```

### Driving it from the laptop brain

The earlier Python brain sent JSON like `{"emotion":"happy"}`. This engine uses
plain tokens instead, which is simpler. To connect them, replace the `emotion()`
/ `gaze()` senders with:

```python
def emotion(self, name):  self._send(name.upper())            # e.g. "HAPPY\n"
def gaze(self, x, y):     self._send(f"GAZE {x:.3f} {y:.3f}")  # face tracking
def _send(self, s):       self.ser and self.ser.write((s + "\n").encode())
```

So MediaPipe face tracking becomes `face.gaze(nx, ny)` and the eyes follow.

---

## Switching to the the other boards like ESP32-S3 / ST7796 board

1. In `platformio.ini`: comment `[env:esp32_ili9341]`, uncomment
   `[env:s3_st7796]` (adjust its pins to your wiring).
2. In `Config.h`: comment `#define BOARD_ILI9341_240x320`, uncomment
   `#define BOARD_S3_ST7796`.
3. Build with `pio run -e s3_st7796`.

Everything else — every emotion, animation and behaviour — is resolution-
independent and needs no changes.

---

## Extending

* **New emotion:** add it to `EmotionManager::Emotion`, give it a preset in
  `preset()` and a behaviour hint in `behaviour()`, then map a command in
  `SerialCommands`. That's it — cross-fading is automatic.
* **Future mouth / eyebrows / ears:** add a sprite in `DisplayManager`, a
  renderer alongside `EyeRenderer`, and drive it from the same resolved state.
  The `EyeState`/`EyeParams` split is designed for exactly this.

---

## Honest status

The full architecture, easing/tween system, blink system, gaze/idle motion,
smooth emotion cross-fading, and the complete serial protocol are implemented
and wired. The core emotions (normal, happy, excited, sad, angry, surprised,
confused, sleepy, sleep, curious, love/heart-eyes, listening, thinking,
searching, error) render from parametric presets rather than 30 hand-drawn
sequences — this is deliberate: it's more maintainable and every extra state is
just another preset. 
