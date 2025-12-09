#include "Gui.hpp"
#include "../Downloader.hpp"
#include "../MangaData.hpp"
#include "../MyApi.hpp"
#include "../Serializator.hpp"
#include "../Tools.hpp"
#include "../include/clay/clay.h"
#include "ImageLoader.hpp"
#include "color/catppuccin_colors.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <raylib.h>
#include <string>
#include <vector>

MangaData logic;
Tools tooling;
MyApi api;
Downloader dloader;
#define MAX_INPUT_CHARS 32
char name[MAX_INPUT_CHARS + 1] = "\0";
int letterCount = 0;

bool shouldInputText = false;

std::vector<SearchResult> SearchResults;

void inputhand() {
  int key = GetCharPressed();

  while (key > 0 && shouldInputText) {
    if ((key >= 32) && (key <= 126) && (letterCount < MAX_INPUT_CHARS)) {
      name[letterCount] = (char)key;
      letterCount++;
    }
    key = GetCharPressed();
  }

  // Borrar caracter
  if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
    letterCount--;
  }

  name[letterCount] = '\0';

  if (IsKeyPressed(KEY_ENTER) && letterCount > 0) {
    SearchResults.clear();
    std::string searchquey(name);
    std::vector<SearchResult> resultsfromurl =
        logic.GetSearchResultsFromUrl(searchquey);
    SearchResults = resultsfromurl;
    // name[0] = '\0'; // no funciona
    // for (int i = 0; i > strlen(name); i++) {

    letterCount = 0;
    name[0] = '\0';
    // }
    // shouldInputText = true;
  }
}
std::vector<Manga> SavedMangas = LoadAllMangas();

static Clay_Color root = base_light;
static const int baseroundcorners = 6;
static const int basegap = 6;
Clay_Padding basepadding = {basegap, basegap, basegap, basegap};

Clay_String toClayString(std::string &str) {
  thread_local static char *temp = nullptr;
  thread_local static size_t tempSize = 0;
  size_t requiredSize = str.size() + 1;
  if (requiredSize > tempSize) {
    delete[] temp;
    temp = new char[requiredSize];
    tempSize = requiredSize;
  }
  std::strcpy(temp, str.c_str());
  Clay_String clayStr =
      Clay_String{.length = static_cast<int32_t>(str.size()), .chars = temp};
  return clayStr;
}

static void srealm() { std::cout << "xd" << std::flush; }

Clay_TextElementConfig DefaultText = {
    .textColor = {76, 79, 105, 255},
    .fontId = Gui::FONT_ID_BODY_16,
    .fontSize = 20,
    .textAlignment = CLAY_TEXT_ALIGN_RIGHT,

};
// implementacion del widget boton
using ButtonCallback = std::function<void()>;
static std::list<ButtonCallback> frameCallbacks;

static void buttonCallbackWrapper(Clay_ElementId id,
                                  Clay_PointerData pointerdata,
                                  void *userdata) {
  if (pointerdata.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    ButtonCallback *callback = reinterpret_cast<ButtonCallback *>(userdata);
    if (callback && *callback) {
      (*callback)();
    }
  }
}

static void altbutton(Clay_String text, ButtonCallback callback) {
  // Obtener referencia a la lista de callbacks del frame actual

  frameCallbacks.push_back(callback);
  Clay_Color actualcolor = teal_dark;
  CLAY_AUTO_ID(.wrapped = {
                   .layout =
                       {
                           .sizing = {.width = CLAY_SIZING_PERCENT(0.9)},
                           .padding = {16, 16, 8, 8},
                           .childGap = basegap,
                           .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
                       },
                   .backgroundColor = actualcolor,
                   .cornerRadius = {baseroundcorners},
               }) {
    Clay_OnHover(buttonCallbackWrapper,
                 static_cast<void *>(&frameCallbacks.back()));
    CLAY_TEXT(text, CLAY_TEXT_CONFIG(.wrapped = {
                                         .textColor = text_light,
                                         .fontId = Gui::FONT_ID_BODY_16,
                                         .fontSize = 20,
                                     }));
  }
} // Gui
void Gui::mylayout() {

  frameCallbacks.clear();

  inputhand();
  bool showCursor = (fmod(GetTime() * 2, 2.0) < 1.0);
  // Crear el string con o sin cursor
  static char
      nameWithCursor[MAX_INPUT_CHARS + 2]; // +1 para cursor, +1 para null
  strcpy(nameWithCursor, name);
  if (showCursor) {
    strcat(nameWithCursor, "_");
  }

  // Actualizar el TextInput cada frame con el nuevo length
  Clay_String TextInput = {
      .isStaticallyAllocated = false,
      .length = showCursor ? letterCount + 1 : letterCount,
      .chars = nameWithCursor,
  };

  static std::vector<std::unique_ptr<char[]>> frameStringBuffers;
  frameStringBuffers.clear(); // Limpiar los buffers del frame anterior
  frameStringBuffers.reserve(SavedMangas.size());

  // std::cout << "Los mangas Guardados son: \n";
  // // int ix = 1;
  // // for (Manga man : SavedMangas) {
  // //   std::cout << ix << ".- " << man.nombre << "\n";
  // //   ix++;
  // }
  CLAY(CLAY_ID("root"), {
                            .layout =
                                {
                                    .sizing =
                                        {
                                            .width = CLAY_SIZING_GROW(),
                                            .height = CLAY_SIZING_GROW(),
                                        },
                                    .padding = basepadding,
                                    .childGap = basegap,
                                    .childAlignment =
                                        {
                                            // .x = CLAY_ALIGN_X_CENTER,
                                            // .y = CLAY_ALIGN_Y_CENTER
                                        },
                                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                },
                            .backgroundColor = root,
                        }) {
    CLAY_AUTO_ID(.wrapped = {
                     .layout = {.sizing =
                                    {
                                        .width = CLAY_SIZING_PERCENT(0.65),
                                        .height = CLAY_SIZING_PERCENT(1),
                                    },
                                // .padding = {12, 12, 12, 12},
                                .childGap = basegap,
                                .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                   .y = CLAY_ALIGN_Y_CENTER},
                                .layoutDirection = CLAY_TOP_TO_BOTTOM},
                     .backgroundColor = root,
                 }) {
      CLAY(CLAY_ID("SearchBar"),
           {
               .layout = {.sizing =
                              {
                                  .width = CLAY_SIZING_GROW(),
                                  .height = CLAY_SIZING_PERCENT(0.05),
                              },
                          .childAlignment = {.x = CLAY_ALIGN_X_LEFT,
                                             .y = CLAY_ALIGN_Y_CENTER},
                          .layoutDirection = CLAY_LEFT_TO_RIGHT},

               .backgroundColor = yellow_dark,
               .cornerRadius = {baseroundcorners},
           }) {

        CLAY_AUTO_ID(.wrapped = {.layout = {.padding = basepadding}}) {}
        if (Clay_Hovered()) {
          shouldInputText = true;
          CLAY_TEXT(TextInput, &DefaultText);
        } else {

          CLAY_TEXT(CLAY_STRING("Escribe el titulo del manga a buscar "),
                    &DefaultText);
          shouldInputText = false;
        }
      }

      CLAY_AUTO_ID(.wrapped = {
                       .layout = {.sizing =
                                      {
                                          .width = CLAY_SIZING_GROW(),
                                          .height = CLAY_SIZING_GROW(),
                                      },
                                  .padding = basepadding,
                                  .childGap = basegap,
                                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                     .y = CLAY_ALIGN_Y_TOP},
                                  .layoutDirection = CLAY_TOP_TO_BOTTOM},
                       .backgroundColor = yellow_dark,
                       .cornerRadius = {baseroundcorners},
                   }) {
        // CLAY_TEXT(TextInput, &DefaultText);
        if (!SearchResults.empty()) {
          for (SearchResult &result : SearchResults) {
            // Copiar el string a un buffer nuevo
            size_t len = result.MangaUrl.size();
            auto buffer = std::make_unique<char[]>(len + 1);
            std::strcpy(buffer.get(), result.MangaUrl.c_str());

            // Crear Clay_String apuntando al buffer
            Clay_String clayStr = {.isStaticallyAllocated = false,
                                   .length = static_cast<int32_t>(len),
                                   .chars = buffer.get()};

            // Guardar el buffer para que persista durante el frame
            frameStringBuffers.push_back(std::move(buffer));

            altbutton(clayStr, srealm);
          }
        }
      }
    }
    CLAY_AUTO_ID(.wrapped = {
                     .layout =
                         {
                             .sizing =
                                 {
                                     .width = CLAY_SIZING_GROW(),
                                     .height = CLAY_SIZING_GROW(),
                                 },
                             // .padding = {12, 12, 12, 12},
                             .childGap = basegap,
                             .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                .y = CLAY_ALIGN_Y_CENTER},
                             .layoutDirection = CLAY_TOP_TO_BOTTOM,
                         },
                     .backgroundColor = root,
                 }) {
      // CLAY_TEXT(CLAY_STRING("UwU"),
      //           CLAY_TEXT_CONFIG(.wrapped = {
      //                                .textColor = {76, 79, 105, 255},
      //                                .fontId = FONT_ID_BODY_16,
      //                                .fontSize = 24,
      //                            }));
      CLAY_AUTO_ID(.wrapped = {
                       .layout = {.sizing =
                                      {
                                          .width = CLAY_SIZING_GROW(),
                                          .height = CLAY_SIZING_PERCENT(0.05),
                                      },

                                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                     .y = CLAY_ALIGN_Y_CENTER}},
                       .backgroundColor = rosewater_dark,
                       .cornerRadius = {baseroundcorners},
                       .clip = {.vertical = true,
                                .childOffset = Clay_GetScrollOffset()},
                   }) {
        CLAY_TEXT(CLAY_STRING("Mangas Guardados: "), &DefaultText);
      }

      CLAY_AUTO_ID(.wrapped = {
                       .layout = {.sizing =
                                      {
                                          .width = CLAY_SIZING_PERCENT(1),
                                          .height = CLAY_SIZING_GROW(),
                                      },
                                  .padding = basepadding,

                                  .childGap = basegap,
                                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                     .y = CLAY_ALIGN_Y_TOP},
                                  .layoutDirection = CLAY_TOP_TO_BOTTOM},
                       .backgroundColor = rosewater_dark,
                       .cornerRadius = {baseroundcorners},
                   }) {

        // int ix = 1;
        for (const Manga &man : SavedMangas) {
          // Copiar el string a un buffer nuevo
          size_t len = man.nombre.size();
          auto buffer = std::make_unique<char[]>(len + 1);
          std::strcpy(buffer.get(), man.nombre.c_str());

          // Crear Clay_String apuntando al buffer
          Clay_String clayStr = {.isStaticallyAllocated = false,
                                 .length = static_cast<int32_t>(len),
                                 .chars = buffer.get()};

          // Guardar el buffer para que persista durante el frame
          frameStringBuffers.push_back(std::move(buffer));

          altbutton(clayStr, srealm);
        }
      }
      CLAY_AUTO_ID(.wrapped = {
                       .layout = {.sizing =
                                      {
                                          .width = CLAY_SIZING_PERCENT(1),
                                          .height = CLAY_SIZING_PERCENT(0.2),
                                      },

                                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                     .y = CLAY_ALIGN_Y_CENTER}},
                       .backgroundColor = rosewater_dark,
                       .cornerRadius = {baseroundcorners},
                   }) {}
    }
  }
}
