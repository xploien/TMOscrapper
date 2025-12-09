#include "../include/clay/clay.h"
#include "../include/clay/renderers/raylib/raylib.h"

class AppClay {
private:
  Font fonts[1];
  Clay_Arena clayMemory;

  static void HandleClayErrors(Clay_ErrorData errorData);

  // custom text meausre method with utf8 suppport
  static inline Clay_Dimensions

  Raylib_MeasureText_Fixed(Clay_StringSlice text,
                           Clay_TextElementConfig *config, void *userData);

public:
  bool Initialize(int width, int height, const char *title, unsigned int flags,
                  const char *font_filename, int font_size);

  Clay_RenderCommandArray CreateLayout();

  void Render(Clay_RenderCommandArray renderCommands);
  void Cleanup();
};
