#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Serializator.hpp"
#include "Tools.hpp"
#include <iostream>
#include <string>

std::string Tosearch;

int main(int argc, char *argv[]) {
  MangaData logic;
  Tools tooling;
  MyApi api;

  std::cout << "ingresa el titulo a buscar: \n";
  std::cin >> Tosearch;

  std::string mangaurl = logic.FindMangaURl(Tosearch);

  Manga Mimanga = logic.GetMangaFromUrl(mangaurl);

  SavetoDB(Mimanga, "MangaElegido.json");

  tooling.imprimirTodosLosCapitulos(Mimanga);

  api.DownloadImage(Mimanga.capitulos[0].traducciones[0].UrlImagenes[1],
                    "prueba1");
  return 0;
}
