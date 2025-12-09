#include "Downloader.hpp"
#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Serializator.hpp"
#include "Tools.hpp"

#include <iostream>
#include <string>
#include <vector>

inline void InitCli() {
  std::string Tosearch;
  int uioption;
  bool shouldclose = false;

  std::vector<Manga> SavedMangas = LoadAllMangas();
  MangaData logic;
  Tools tooling;
  MyApi api;
  Downloader dloader;

  while (!shouldclose) {

    std::cout << "INGRESE UNA OPCION: \n \n";
    std::cout << "1.- Buscar y guardar manga \n";
    std::cout << "2.- Descargar Manga \n";
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

        SavedMangas = LoadAllMangas();
      } else {
        std::cerr << "ese manga ya esta en la base de datos Desearia re "
                     "escanearlo ?\n";
        std::cout << "1.= si \n 0. = no";
        int userselec;
        std::cin >> userselec;
        if (userselec) {
          Mimanga = logic.GetMangaFromUrl(mangaurl);
          Mimanga.baseurl = mangaurl;

          SavetoDB(Mimanga);
          SavedMangas.push_back(Mimanga);

          tooling.imprimirTodosLosCapitulos(Mimanga);
          //
          // api.DownloadImage(Mimanga.capitulos[0].traducciones[0].UrlImagenes[1],
          //                   "prueba1.webm");
          //
          SavedMangas = LoadAllMangas();
        }
      }

    } break;

    case 2: {
      std::cout << "Los mangas Guardados son: \n";
      int ix = 1;
      for (Manga man : SavedMangas) {
        std::cout << ix << ".- " << man.nombre << "\n";
        ix++;
      }
      std::cout << "Elije el numero a Descargar \n";

      int usrseleccase2;
      std::cin >> usrseleccase2;
      usrseleccase2 = usrseleccase2 - 1;
      Manga TestTemp = SavedMangas[usrseleccase2];

      std::vector<int> mangaindex = dloader.MangaFilterIndex(TestTemp);
      std::cout << "debug de indexes: ";
      for (int i : mangaindex) {
        std::cout << i << "\n";
      }

      dloader.setSpeed(300);
      dloader.setThreads(4);
      dloader.MangatoCBZ(TestTemp, mangaindex);
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
}
