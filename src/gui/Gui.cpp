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
#include <cstdio>
#include <cstdlib>
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

#include <iomanip>
#include <sstream>

std::string urlEncode(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (char c : value) {
    // Mantener alphanumericos y algunos caracteres seguros
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
    }
    // Convertir espacio a '+'
    else if (c == ' ') {
      escaped << '+';
    }
    // Codificar todo lo demás como %XX
    else {
      escaped << std::uppercase;
      escaped << '%' << std::setw(2) << int((unsigned char)c);
      escaped << std::nouppercase;
    }
  }

  return escaped.str();
}

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
      name[letterCount] = '\0';
    }
    key = GetCharPressed();
  }

  // Borrar caracter
  if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
    letterCount--;
    name[letterCount] = '\0';
  }

  if (IsKeyPressed(KEY_ENTER) && letterCount > 0) {
    SearchResults.clear();
    std::string searchquey(name);

    // URL encode el query antes de enviarlo
    std::string encodedQuery = urlEncode(searchquey);
    std::cout << "DEBUG: Original query: " << searchquey << "\n";
    std::cout << "DEBUG: Encoded query: " << encodedQuery << "\n";

    std::vector<SearchResult> resultsfromurl =
        logic.GetSearchResultsFromUrl(encodedQuery);
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
}

bool ShouldSelectTranslator = false;
int currentmanindex;
void MangaDownloaderIndexCatcher(std::string traductor) {
  std::vector<int> resultado;
  Manga inputmanga = SavedMangas[currentmanindex];
  std::string traductorselecionado = traductor;
  for (Capitulo cap : inputmanga.capitulos) {
    for (int i = 0; i < cap.NumTraduciones; i++) {
      if (cap.traducciones[i].NombreTraductor == traductorselecionado) {
        resultado.push_back(i);
        break;
      }
    }
  }
  dloader.setSpeed(300);
  dloader.setThreads(4);
  dloader.MangatoCBZ(inputmanga, resultado);
}

void closepopup() { ShouldSelectTranslator = !ShouldSelectTranslator; }
// bool tooglebool(bool elbool) { return !elbool; }

void MangaDownloaderCaller(int index) {
  // std::cout << "funciona!:" << index << "\n";
  if (!ShouldSelectTranslator) {
    Manga TestTemp = SavedMangas[index];

    int AnalisisResult = dloader.GuiAnalizeFilterIndex(TestTemp);
    std::cout << "uwu";
    std::vector<int> mangaindex;
    switch (AnalisisResult) {
    default: {
      std::cerr << "wtf how did you get this \n";
    }
    case 0: {
      break;
    }
    case 1: {
      std::vector<int> indice(TestTemp.numerocapitulos, 0);
      mangaindex = indice;
    }
    case 2: {
      std::cout << "we rocking \n";

      ShouldSelectTranslator = true;
    }
    }
  }

  // we implementing until uwu
  //  std::cout << "traductor ?:  \n";
  //  int iy = 1;
  //  for (std::string trad : fulltraductores) {
  //    std::cout << iy << " " << trad << "\n";
  //    iy++;
  //  }
  //
  //  int usertradselect;
  //  std::cin >> usertradselect;
  // uwu

  // std::string traductorselecionado = fulltraductores[usertradselect - 1];
  // // checar coincidencias al mas estilo fuerza bruta
  // for (Capitulo cap : inputmanga.capitulos) {
  //   for (int i = 0; i < cap.NumTraduciones; i++) {
  //     if (cap.traducciones[i].NombreTraductor == traductorselecionado) {
  //       resultado.push_back(i);
  //       break;
  //     }
  //   }
  // }
}

// std::vector<int> mangaindex = dloader.MangaFilterIndex(TestTemp);
// std::cout << "index's: ";
// for (int i : mangaindex) {
//   std::cout << i << "\n";
// }
//
// dloader.setSpeed(300);
// dloader.setThreads(4);
// dloader.MangatoCBZ(TestTemp, mangaindex);

void panelhandle(std::string mangabaseurl) {
  Manga Mimanga;

  Mimanga = logic.GetMangaFromUrl(mangabaseurl);
  Mimanga.baseurl = mangabaseurl;

  SavetoDB(Mimanga);
  SavedMangas.push_back(Mimanga);

  tooling.imprimirTodosLosCapitulos(Mimanga);

  SavedMangas = LoadAllMangas();
};

// static void MangaPanel(Clay_String manname, Texture2D *image) {
//   // Obtener referencia a la lista de callbacks del frame actual
//
//   frameCallbacks.push_back(panelhandle);
//   Clay_Color actualcolor = teal_dark;
//   CLAY_AUTO_ID(.wrapped = {
//                    .layout =
//                        {
//                            .sizing =
//                                {
//
//                                    .width =
//                                        CLAY_SIZING_FIXED(220), // Ancho fijo
//                                    .height = CLAY_SIZING_FIXED(380) // Alto
//                                    fijo
//                                },
//                            .padding = {16, 16, 8, 8},
//                            .childGap = basegap,
//                            .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
//                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
//                        },
//                    .backgroundColor = yellow_dark,
//                    .cornerRadius = {baseroundcorners},
//                }) {
//     Clay_OnHover(buttonCallbackWrapper,
//                  static_cast<void *>(&frameCallbacks.back()));
//
//     CLAY_AUTO_ID(.wrapped = {
//                      .layout = {.sizing = {.width =
//                      CLAY_SIZING_PERCENT(0.95),
//                                            .height =
//                                                CLAY_SIZING_PERCENT(0.95)}},
//                      .image = {.imageData = image},
//                  }){};
//     CLAY_TEXT(manname, CLAY_TEXT_CONFIG(.wrapped = {
//                                             .textColor = text_light,
//                                             .fontId = Gui::FONT_ID_BODY_16,
//                                             .fontSize = 20,
//                                         }));
//   }
// }

static void panel(Clay_String manname, std::string imagelink,
                  std::string mangabaseurl) {

  frameCallbacks.push_back([mangabaseurl]() { panelhandle(mangabaseurl); });

  Texture2D *image = ImageLoader::GetImageFromUrl(imagelink);
  CLAY_AUTO_ID(.wrapped = {
                   .layout =
                       {
                           .sizing =
                               {

                                   .width = CLAY_SIZING_GROW(),
                                   .height = CLAY_SIZING_GROW()},
                           .padding = {16, 16, 8, 8},
                           .childGap = 12,
                           .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                              .y = CLAY_ALIGN_Y_CENTER},
                           .layoutDirection = CLAY_TOP_TO_BOTTOM,
                       },
                   .backgroundColor = yellow_dark,
                   .cornerRadius = {baseroundcorners},
               }) {
    Clay_OnHover(buttonCallbackWrapper,
                 static_cast<void *>(&frameCallbacks.back()));
    CLAY_AUTO_ID(.wrapped = {
                     .layout =
                         {
                             .sizing = {.width = CLAY_SIZING_FIXED(120),
                                        .height = CLAY_SIZING_FIXED(130)},
                             .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                .y = CLAY_ALIGN_Y_CENTER},
                         },
                     .backgroundColor = yellow_dark,

                     .image = {.imageData = image},
                 }){};

    CLAY_AUTO_ID(.wrapped = {
                     .layout =
                         {
                             .sizing = {.width = CLAY_SIZING_FIXED(150),
                                        .height = CLAY_SIZING_FIXED(30)},
                             .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                                .y = CLAY_ALIGN_Y_CENTER},
                         },

                     .backgroundColor = yellow_dark,
                 }) {
      CLAY_TEXT(manname, &DefaultText
                // CLAY_TEXT_CONFIG(.wrapped = {
                //                      .textColor = text_light,
                //                      .fontId = Gui::FONT_ID_BODY_16,
                //                      .fontSize = 20,
                //                  })
      );
    };
  }
}

// Gui
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
        // popup code:
        if (ShouldSelectTranslator) {
          // super dense nest
          CLAY(CLAY_ID("FloatingContainer"),
               .wrapped = {
                   .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.5),
                                         .height = CLAY_SIZING_FIXED(300)},
                              .padding = {16, 16, 16, 16},
                              .childGap = basegap,
                              .layoutDirection = CLAY_TOP_TO_BOTTOM},
                   .backgroundColor = {140, 80, 200, 200},
                   .floating =
                       {
                           .offset = {0, 0},
                           .zIndex = 1,
                           .attachPoints = {CLAY_ATTACH_POINT_CENTER_TOP,
                                            CLAY_ATTACH_POINT_CENTER_TOP},
                           .attachTo = CLAY_ATTACH_TO_PARENT,
                       },
                   .border =
                       {
                           .color = {80, 80, 80, 255},
                           .width = CLAY_BORDER_OUTSIDE(2),
                       },
               }) {
            CLAY_TEXT(CLAY_STRING("Selecione Traductor: "),
                      CLAY_TEXT_CONFIG({
                          .textColor = {255, 255, 255, 255},
                          .fontSize = 24,
                      }));
            if (fulltraductores.empty()) {
              std::cout << "y la culpa no era mia sino de fulltraducterias "
                        << "\n";
            }

            // hay que pensar en como pasar los datos de fulltraducterias a aqui
            for (std::string trad : fulltraductores) {
              size_t len = trad.size();
              auto buffer = std::make_unique<char[]>(len + 1);
              std::strcpy(buffer.get(), trad.c_str());

              // Crear Clay_String apuntando al buffer
              Clay_String tradclayStr = {.isStaticallyAllocated = false,
                                         .length = static_cast<int32_t>(len),
                                         .chars = buffer.get()};
              // Guardar el buffer para que persista durante el frame
              frameStringBuffers.push_back(std::move(buffer));
              // pass id index?

              altbutton(tradclayStr,
                        [trad]() { MangaDownloaderIndexCatcher(trad); });
            }

            altbutton(Clay_String("cerrar"), closepopup);
          }
        }
        if (!SearchResults.empty()) {
          const int PANELS_PER_ROW = 2; // Paneles por fila

          // Calcular número de filas necesarias
          int numRows =
              (SearchResults.size() + PANELS_PER_ROW - 1) / PANELS_PER_ROW;

          // Iterar por cada fila
          for (int row = 0; row < numRows; row++) {

            // Contenedor de FILA (horizontal)
            CLAY_AUTO_ID(.wrapped = {
                             .layout =
                                 {
                                     .sizing =
                                         {
                                             .width = CLAY_SIZING_GROW(),
                                             .height = CLAY_SIZING_GROW(),
                                         },
                                     .padding = basepadding,
                                     .childGap = basegap * 8,
                                     .childAlignment = {.x =
                                                            CLAY_ALIGN_X_CENTER,
                                                        .y = CLAY_ALIGN_Y_TOP},
                                     .layoutDirection =
                                         CLAY_LEFT_TO_RIGHT // HORIZONTAL
                                 },
                             .backgroundColor = yellow_dark,
                             .cornerRadius = {baseroundcorners},
                         }) {

              // Calcular índice inicial de esta fila
              int startIdx = row * PANELS_PER_ROW;

              // Añadir hasta PANELS_PER_ROW paneles en esta fila
              for (int col = 0; col < PANELS_PER_ROW; col++) {
                int idx = startIdx + col;

                // VERIFICAR BOUNDS antes de acceder
                if (idx < SearchResults.size()) {
                  // Copiar el string a un buffer nuevo
                  size_t len = SearchResults[idx].nameManga.size();
                  auto buffer = std::make_unique<char[]>(len + 1);
                  std::strcpy(buffer.get(),
                              SearchResults[idx].nameManga.c_str());

                  // Crear Clay_String apuntando al buffer
                  Clay_String clayStr = {.isStaticallyAllocated = false,
                                         .length = static_cast<int32_t>(len),
                                         .chars = buffer.get()};

                  // Guardar el buffer para que persista durante el frame
                  frameStringBuffers.push_back(std::move(buffer));

                  panel(clayStr, SearchResults[idx].ImageUrl,
                        SearchResults[idx].MangaUrl);
                }
              }
            }
          }
        }
        // CLAY_TEXT(TextInput, &DefaultText);
        // if (!SearchResults.empty()) {
        //   for (SearchResult &result : SearchResults) {
        //     // Copiar el string a un buffer nuevo
        //     size_t len = result.nameManga.size();
        //     auto buffer = std::make_unique<char[]>(len + 1);
        //     std::strcpy(buffer.get(), result.nameManga.c_str());
        //
        //     // Crear Clay_String apuntando al buffer
        //     Clay_String clayStr = {.isStaticallyAllocated = false,
        //                            .length = static_cast<int32_t>(len),
        //                            .chars = buffer.get()};
        //
        //     // Guardar el buffer para que persista durante el frame
        //     frameStringBuffers.push_back(std::move(buffer));
        //     // Texture2D *mangacover =
        //     // ImageLoader::GetImageFromUrl(result.ImageUrl);
        //     // Texture2D *mangacover = ImageLoader::GetImage("mytest",
        //     // "/home/poe/dormir.png");
        //     Texture2D *mangacover =
        //         ImageLoader::GetImageFromUrl(result.ImageUrl);
        //
        //     // std::cout << "se llego a esta parte \n ";
        //
        //     MangaPanel(clayStr, mangacover, srealm);
        //   }
        // }
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
          int index = static_cast<int>(&man - &SavedMangas[0]);
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
          currentmanindex = index;
          altbutton(clayStr, [index]() { MangaDownloaderCaller(index); });
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
