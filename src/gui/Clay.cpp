#include <iostream>
#define CLAY_IMPLEMENTATION
#include "../include/clay/clay.h"
#include "../include/clay/renderers/raylib/clay_renderer_raylib.c"
// incluir despues de clay
//

#include "Clay.hpp"
#include "Gui.hpp"
#include "ImageLoader.hpp"
const Gui Gui_obj;

void AppClay::HandleClayErrors(Clay_ErrorData errorData) {
  std::cerr << errorData.errorText.chars;
}

inline Clay_Dimensions AppClay::Raylib_MeasureText_Fixed(
    Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
  Clay_Dimensions textSize = {0};

  float maxTextWidth = 0.0f;
  float lineTextWidth = 0;

  float textHeight = config->fontSize;
  Font *fonts = (Font *)userData;
  Font fontToUse = fonts[config->fontId];

  if (fontToUse.glyphs == nullptr) {
    fontToUse = GetFontDefault();
  }

  float scaleFactor = config->fontSize / (float)fontToUse.baseSize;

  int byte_index = 0;
  while (byte_index < text.length) {
    if (text.chars[byte_index] == '\n') {
      maxTextWidth = std::max(maxTextWidth, lineTextWidth);
      lineTextWidth = 0;
      byte_index++;
      continue;
    }

    int codepoint_bytes = 0;
    int codepoint = GetCodepoint(&text.chars[byte_index], &codepoint_bytes);
    int glyph_index = GetGlyphIndex(fontToUse, codepoint);
    byte_index += codepoint_bytes;

    if (fontToUse.glyphs[glyph_index].advanceX != 0) {
      lineTextWidth += fontToUse.glyphs[glyph_index].advanceX;
    } else {
      lineTextWidth += (fontToUse.recs[glyph_index].width +
                        fontToUse.glyphs[glyph_index].offsetX);
    }
  }

  maxTextWidth = std::max(maxTextWidth, lineTextWidth);

  textSize.width = maxTextWidth * scaleFactor;
  textSize.height = textHeight;

  return textSize;
}

bool AppClay::Initialize(int width, int height, const char *title,
                         unsigned int flags, const char *font_filename,
                         int font_size) {
  // Initialize raylib
  Clay_Raylib_Initialize(width, height, title, flags);

  // memory allocating
  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  clayMemory = Clay_CreateArenaWithCapacityAndMemory(
      clayRequiredMemory, malloc(clayRequiredMemory));
  Clay_Initialize(
      clayMemory,
      (Clay_Dimensions){.width = static_cast<float>(GetScreenWidth()),
                        .height = static_cast<float>(GetScreenHeight())},
      (Clay_ErrorHandler){HandleClayErrors});

  int *codepoints = (int *)malloc(2000 * sizeof(int));
  int count = 0;

  // Characters range, basic ascii:
  for (int i = 32; i <= 126; i++) {
    codepoints[count++] = i;
  }

  // extended ascii
  // for (int i = 160; i <= 255; i++) {
  //   codepoints[count++] = i;
  // }
  // ord y hex
  // especific non trivial icons
  int nerdIcons[] = {
      // Tu icono específico
      0xF0594, // 󰖔
      0xf04b,  // 
      0xf04ae, // 󰒮
      0xf04ad, // 󰒭
      0xf444,  // 
      0x3010,  //  【
      0x3011,  //  】
      0xe68f,  // 

      // Otros iconos comunes
      // 0xf013,
      // 0xf015,
      // 0xf00d,
      // 0xf067,
      // 0xf068,
      // 0xf07b,
      // 0xf07c,
      // 0xf15b,
      // 0xf15c,
      // 0xf060,
      // 0xf061,
      // 0xf062,
      // 0xf063,
      // 0xf04b,
      // 0xf04c,
      // 0xf04d,
      // 0xe702,
      // 0xe703,
      //
      // 0xF0590,
      // 0xF0591,
      // 0xF0592,
      // 0xF0593,
      // 0xF0595,
      // 0xF0596,
  };

  // Add icons to the array
  for (size_t i = 0;
       i < sizeof(nerdIcons) / sizeof(nerdIcons[0]) && count < 1900; i++) {
    codepoints[count++] = nerdIcons[i];
  }
  // debug might coment
  // printf("Loading %d characters including Unicode icons...\n", count);

  // Cargar fuente

  fonts[Gui::FONT_ID_BODY_16] =
      LoadFontEx(font_filename, font_size, codepoints, count);

  if (fonts[Gui::FONT_ID_BODY_16].texture.id == 0) {
    printf("LoadFontEx failed!\n");
    fonts[Gui::FONT_ID_BODY_16] = GetFontDefault();
  } else {
    printf("Font loaded: %dx%d texture with %d glyphs\n",
           fonts[Gui::FONT_ID_BODY_16].texture.width,
           fonts[Gui::FONT_ID_BODY_16].texture.height,
           fonts[Gui::FONT_ID_BODY_16].glyphCount);
  }

  SetTextureFilter(fonts[Gui::FONT_ID_BODY_16].texture,
                   TEXTURE_FILTER_BILINEAR);
  free(codepoints);

  // pass custom text measure function to clay
  Clay_SetMeasureTextFunction(Raylib_MeasureText_Fixed, fonts);

  // carga imagenes
  ImageLoader::Initialize();

  return true;
}

Clay_RenderCommandArray AppClay::CreateLayout() {
  Clay_SetLayoutDimensions(
      (Clay_Dimensions){.width = static_cast<float>(GetScreenWidth()),
                        .height = static_cast<float>(GetScreenHeight())});

  Vector2 mousePosition = GetMousePosition();
  Vector2 scrollDelta = GetMouseWheelMoveV();
  Clay_SetPointerState((Clay_Vector2){mousePosition.x, mousePosition.y},
                       IsMouseButtonDown(0));
  Clay_UpdateScrollContainers(
      true, (Clay_Vector2){scrollDelta.x, scrollDelta.y}, GetFrameTime());

  Clay_BeginLayout();
  Gui_obj.mylayout(); // <--- from external file
  return Clay_EndLayout();
}

void AppClay::Render(Clay_RenderCommandArray renderCommands) {
  Clay_Raylib_Render(renderCommands, fonts);
}

void AppClay::Cleanup() {
  ImageLoader::Cleanup();
  UnloadFont(fonts[Gui::FONT_ID_BODY_16]);
  Clay_Raylib_Close();
}
