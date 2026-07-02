// =============================================================================
//  EyeRenderer.cpp
//  Draws ONE eye as a single glowing cyan shape (AURA style). The whole shape
//  slides for gaze, reshapes for emotion (slant / arcs / silhouette) and
//  squishes to a line when blinking. Anti-aliased via TFT_eSPI's smooth calls.
// =============================================================================
#include "EyeRenderer.h"
#include "Easing.h"

// Draw a filled heart from two circles and a triangle (LOVE eyes).
void EyeRenderer::drawHeart(TFT_eSprite& spr, int cx, int cy, int r, uint16_t col) {
    int lobe = r * 0.58f;
    spr.fillSmoothCircle(cx - lobe, cy - lobe / 2, lobe, col, Cfg::BG_COLOR);
    spr.fillSmoothCircle(cx + lobe, cy - lobe / 2, lobe, col, Cfg::BG_COLOR);
    spr.fillTriangle(cx - r, cy - lobe / 3,
                     cx + r, cy - lobe / 3,
                     cx,     cy + r, col);
}
void EyeRenderer::drawPupil(TFT_eSprite& spr,
                            int cx,
                            int cy,
                            int ew,
                            int eh,
                            float gazeX,
                            float gazeY)
{
    // Margin so pupil stays inside eye
    int margin = 12;

    int maxX = ew / 2 - margin;
    int maxY = eh / 2 - margin;

    int px = cx + constrain((int)gazeX, -maxX, maxX);
    int py = cy + constrain((int)gazeY, -maxY, maxY);

    int pupilRadius = min(ew, eh) / 4;

    // Black pupil
    spr.fillSmoothCircle(px, py, pupilRadius, TFT_BLACK, Cfg::BG_COLOR);

    // Large white highlight
    spr.fillCircle(
        px - pupilRadius / 3,
        py - pupilRadius / 3,
        pupilRadius / 4,
        TFT_WHITE
    );

    // Small highlight
    spr.fillCircle(
        px + pupilRadius / 4,
        py + pupilRadius / 4,
        pupilRadius / 8,
        TFT_WHITE
    );
}
void EyeRenderer::render(TFT_eSprite& spr, const EyeState& st) {
    const EyeParams& p = st.p;
    spr.fillSprite(Cfg::BG_COLOR);

    // Effective size (openness + blink squeeze the height).
    float openFactor = Ease::clampf(p.openness * st.blink, 0.0f, 1.0f);
    int ew = (int)p.width;
    int eh = (int)(p.height * openFactor);

    // The WHOLE eye moves with gaze (clamped to stay inside its sprite).
    int maxDX = Cfg::SPRITE_W / 2 - ew / 2 - 2;
    int maxDY = Cfg::SPRITE_H / 2 - max(eh, 6) / 2 - 2;
    int cx = Cfg::SPRITE_W / 2 + (int)Ease::clampf(st.gazeX, -maxDX, maxDX);
    int cy = Cfg::SPRITE_H / 2 + (int)st.bob
                              + (int)Ease::clampf(st.gazeY, -maxDY, maxDY);

    // Nearly shut -> draw a cyan lid line and stop.
    if (eh < 6) {
        spr.fillSmoothRoundRect(cx - ew / 2, cy - 2, ew, 4, 2, p.color, Cfg::BG_COLOR);
        return;
    }

    // Heart-eyes: the whole eye is a heart.
    if (p.shape == EyeShapeMode::HEART) {
        drawHeart(spr, cx, cy, min(ew, eh) / 2, p.color);
        return;
    }

    // Round eyes (surprised).
    if (p.shape == EyeShapeMode::CIRCLE) {
        spr.fillSmoothCircle(
    cx,
    cy,
    min(ew, eh) / 2,
    p.color,
    Cfg::BG_COLOR
);

drawPupil(
    spr,
    cx,
    cy,
    ew,
    eh,
    st.gazeX,
    st.gazeY
);
        return;
    }

    // Default: anti-aliased rounded rectangle.
    int r = (int)min(p.radius, min(ew, eh) * 0.5f);
    spr.fillSmoothRoundRect(cx - ew / 2, cy - eh / 2, ew, eh, r, p.color, Cfg::BG_COLOR);
    drawPupil(
    spr,
    cx,
    cy,
    ew,
    eh,
    st.gazeX,
    st.gazeY
);

    // Emotion slant: cut a background triangle off a top corner.
    // +slant = angry (inner-top down); -slant = sad (outer-top down).
    if (fabsf(p.slant) > 0.01f) {
        bool cutInner = p.slant > 0;
        bool innerIsLeft = (st.side == Side::RIGHT);
        bool cutLeftCorner = cutInner ? innerIsLeft : !innerIsLeft;
        float amt = fabsf(p.slant);
        int dropY = (int)(eh * 0.8f * amt);
        int top = cy - eh / 2 - 1;
        if (cutLeftCorner)
            spr.fillTriangle(cx - ew / 2 - 1, top, cx + ew / 2 + 1, top,
                             cx - ew / 2 - 1, top + dropY, Cfg::BG_COLOR);
        else
            spr.fillTriangle(cx - ew / 2 - 1, top, cx + ew / 2 + 1, top,
                             cx + ew / 2 + 1, top + dropY, Cfg::BG_COLOR);
    }

    // Happy: carve the bottom with a background circle -> upward smile arc.
    if (p.bottomArc > 0.01f) {
        int carveR = (int)(eh * (0.9f + p.bottomArc * 0.9f));
        int carveY = cy + eh / 2 + (int)(carveR * (1.0f - p.bottomArc)) - 2;
        spr.fillSmoothCircle(cx, carveY, carveR, Cfg::BG_COLOR, Cfg::BG_COLOR);
    }

    // Sad / worried: carve the top -> downward arc.
    if (p.topArc > 0.01f) {
        int carveR = (int)(eh * (0.9f + p.topArc * 0.9f));
        int carveY = cy - eh / 2 - (int)(carveR * (1.0f - p.topArc)) + 2;
        spr.fillSmoothCircle(cx, carveY, carveR, Cfg::BG_COLOR, Cfg::BG_COLOR);
    }
}
