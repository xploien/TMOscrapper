#ifndef CATPPUCCIN_COLORS_H
#define CATPPUCCIN_COLORS_H

// ========================================
// Catppuccin Color Palette - Dual Theme
// Compatible con Raylib y Clay
// ========================================

// Tema oscuro (Mocha)
#define rosewater_dark {245, 224, 220, 255}
#define flamingo_dark {242, 205, 205, 255}
#define pink_dark {245, 194, 231, 255}
#define mauve_dark {203, 166, 247, 255}
#define red_dark {243, 139, 168, 255}
#define maroon_dark {235, 160, 172, 255}
#define peach_dark {250, 179, 135, 255}
#define yellow_dark {249, 226, 175, 255}
#define green_dark {166, 227, 161, 255}
#define teal_dark {148, 226, 213, 255}
#define sky_dark {137, 220, 235, 255}
#define sapphire_dark {116, 199, 236, 255}
#define blue_dark {137, 180, 250, 255}
#define lavender_dark {180, 190, 254, 255}
#define text_dark {205, 214, 244, 255}
#define subtext1_dark {186, 194, 222, 255}
#define subtext0_dark {166, 173, 200, 255}
#define overlay2_dark {147, 153, 178, 255}
#define overlay1_dark {127, 132, 156, 255}
#define overlay0_dark {108, 112, 134, 255}
#define surface2_dark {88, 91, 112, 255}
#define surface1_dark {69, 71, 90, 255}
#define surface0_dark {49, 50, 68, 255}
#define base_dark {30, 30, 46, 255}
#define mantle_dark {24, 24, 37, 255}
#define crust_dark {17, 17, 27, 255}

// Tema claro (Latte)
#define rosewater_light {220, 138, 120, 255}
#define flamingo_light {221, 120, 120, 255}
#define pink_light {234, 118, 203, 255}
#define mauve_light {136, 57, 239, 255}
#define red_light {210, 15, 57, 255}
#define maroon_light {230, 69, 83, 255}
#define peach_light {254, 100, 11, 255}
#define yellow_light {223, 142, 29, 255}
#define green_light {64, 160, 43, 255}
#define teal_light {23, 146, 153, 255}
#define sky_light {4, 165, 229, 255}
#define sapphire_light {32, 159, 181, 255}
#define blue_light {30, 102, 245, 255}
#define lavender_light {114, 135, 253, 255}
#define text_light {76, 79, 105, 255}
#define subtext1_light {92, 95, 119, 255}
#define subtext0_light {108, 111, 133, 255}
#define overlay2_light {124, 127, 147, 255}
#define overlay1_light {140, 143, 161, 255}
#define overlay0_light {156, 160, 176, 255}
#define surface2_light {172, 176, 190, 255}
#define surface1_light {188, 192, 204, 255}
#define surface0_light {204, 208, 218, 255}
#define base_light {239, 241, 245, 255}
#define mantle_light {230, 233, 239, 255}
#define crust_light {220, 224, 232, 255}

// ========================================
// Ejemplos de uso:
// ========================================
/*

// Para Raylib:
ClearBackground((Color)Base_Cat);
DrawRectangle(10, 10, 100, 50, (Color)Peach_Cat);
DrawText("Texto", 10, 10, 20, (Color)Text_Cat);

// Para Clay:
CLAY({
    .layout = {
        .sizing = {.width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW()}
    },
    .backgroundColor = Base_Cat,
}) {
    CLAY_TEXT(
        CLAY_STRING("Texto elegante"),
        CLAY_TEXT_CONFIG({
            .fontSize = 24,
            .textColor = Lavender_Cat
        })
    );
}

*/

#endif // CATPPUCCIN_MOCHA_COLORS_H
