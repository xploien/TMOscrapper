#include "MangaData.hpp"
#include "Tools.hpp"
#include <iostream>
#include <string>

std::string Tosearch;
int main(int argc, char *argv[]) {
  MangaData logic;
  Tools tooling;

  std::cout << "ingresa el titulo a buscar: \n";
  std::cin >> Tosearch;

  std::string mangaurl = logic.FindMangaURl(Tosearch);

  Manga Mimanga = logic.GetMangaFromUrl(mangaurl);

  tooling.imprimirTodosLosCapitulos(Mimanga);

  return 0;
}
