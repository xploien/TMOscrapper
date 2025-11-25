#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Serializator.hpp"
#include "Tools.hpp"
#include <iostream>
#include <string>
#include <vector>

std::string Tosearch;
int uioption;
bool shouldclose = false;
std::vector<Manga> SavedMangas = LoadAllMangas();

MangaData logic;
Tools tooling;
MyApi api;

int main(int argc, char *argv[]) {

  while (!shouldclose) {

    std::cout << "INGRESE UNA OPCION: \n \n";
    std::cout << "1.- Buscar y guardar manga \n";
    std::cout << "2.- Listar mangas guardados \n";
    std::cout << "0.- Salir \n";
    std::cin >> uioption;

    switch (uioption) {
    case 1: {
      std::cout << "ingresa el titulo a buscar: \n";
      std::cin >> Tosearch;

      std::string mangaurl = logic.FindMangaURl(Tosearch);

      Manga Mimanga;

      bool exists = false;

      for (const Manga &man : SavedMangas) {
        if (man.baseurl == mangaurl) {
          exists = true;
          Mimanga = man;
          break;
        }
      }

      if (!exists) {
        Mimanga = logic.GetMangaFromUrl(mangaurl);
        Mimanga.baseurl = mangaurl;

        SavetoDB(Mimanga);
        SavedMangas.push_back(Mimanga);

        tooling.imprimirTodosLosCapitulos(Mimanga);

        api.DownloadImage(Mimanga.capitulos[0].traducciones[0].UrlImagenes[1],
                          "prueba1");

        std::vector<Manga> SavedMangas = LoadAllMangas();
      }
      std::cerr << "ese manga ya esta en la base de datos \n";

    } break;
    case 2: {
      std::cout << "Los mangas Guardados son: \n";
      for (Manga man : SavedMangas) {
        std::cout << man.nombre << "\n";
      }
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

  return 0;
}
