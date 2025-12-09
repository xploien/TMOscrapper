#include "MangaData.hpp"
#include "Serializator.hpp"
#include "gui/Clay.hpp"
#include "include/clay/renderers/raylib/raylib.h"
#include <iostream>
#include <string>
#include <vector>
AppClay app;

int main(int argc, char *argv[]) {

  // std::vector<Manga> SavedMangas = LoadAllMangas();
  //
  //   std::cout << "Los mangas Guardados son: \n";
  //       int ix = 1;
  //       for (Manga man : SavedMangas) {
  //         std::cout << ix << ".- " << man.nombre << "\n";
  //         ix++;
  //       }

  AppClay app;
  app.Initialize(911, 640, "oWl",
                 FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI |
                     FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT,
                 "resources/ComicShannsMonoNerdFont-Regular.otf", 48);

  std::cout << "El motor Clay Iniciado\n";

  while (!WindowShouldClose()) {
    Clay_RenderCommandArray rendercommands = app.CreateLayout();

    BeginDrawing();
    // ClearBackground({239, 241, 245, 255});
    app.Render(rendercommands);

    EndDrawing();
  }
  return 0;
}
