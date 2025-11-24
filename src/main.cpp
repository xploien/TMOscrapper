#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Serializator.hpp"
#include "Tools.hpp"
#include <iostream>
#include <string>

std::string Tosearch;
int uioption;
bool shouldclose = false;

MangaData logic;
Tools tooling;
MyApi api;

int main(int argc, char *argv[]) {

  while (!shouldclose) {

    std::cout << "INGRESE UNA OPCION: \n \n";
    std::cout << "1.- Buscar y guardar manga \n";
    std::cout << "2.- Descargar Manga\n";
    std::cout << "0.- Salir \n";
    std::cin >> uioption;

    switch (uioption) {
    case 1: {
      std::cout << "ingresa el titulo a buscar: \n";
      std::cin >> Tosearch;

      std::string mangaurl = logic.FindMangaURl(Tosearch);

      Manga Mimanga = logic.GetMangaFromUrl(mangaurl);

      SavetoDB(Mimanga);

      tooling.imprimirTodosLosCapitulos(Mimanga);

      api.DownloadImage(Mimanga.capitulos[0].traducciones[0].UrlImagenes[1],
                        "prueba1");
    } break;
    case 2: {
      std::cout << "Uninmplemented \n";
      break;
    }
    case 0: {
      shouldclose = true;
      break;
    }
    default: {

      std::cout << " Selecione una opcion correcta \n";
      break;
    }
    }
  }

  std::cout << " Se llego al final del while Adios \n";
  return 0;
}
