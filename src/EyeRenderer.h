// =============================================================================
//  EyeRenderer.h
//  Pure rendering: given a fully-resolved EyeState, draw ONE eye into a sprite.
//  Holds no animation state of its own - it only translates numbers into
//  anti-aliased pixels. That separation keeps the visual logic testable and
//  makes future accessories (mouth, eyebrows, ears) drop-in additions.
// =============================================================================
#pragma once
#include <TFT_eSPI.h>
#include "EyeState.h"

class EyeRenderer {
public:
    // Draw `st` into `spr`. The sprite is assumed to be SPRITE_W x SPRITE_H and
    // is fully repainted (background first) so no ghosting remains.
    static void render(TFT_eSprite& spr, const EyeState& st);

private:
    // Draw a filled heart centred at (cx,cy) with the given half-width.
    static void drawHeart(TFT_eSprite& spr, int cx, int cy, int r, uint16_t col);
    static void drawPupil(TFT_eSprite& spr,
                          int cx,
                          int cy,
                          int ew,
                          int eh,
                          float gazeX,
                          float gazeY);
};
