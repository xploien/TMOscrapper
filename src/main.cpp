#include <iostream>
#include <string>
#include <vector>

#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Tools.hpp"

int main(int argc, char *argv[]) {
  Tools tooling;
  MyApi Api;
  std::cout << "ingresa el titulo a buscar: \n";
  std::string Tosearch;
  std::cin >> Tosearch;

  std::string pagina =
      Api.GetRawPage(Tosearch, "https://zonatmo.com/library?title=");

  std::vector<std::string> Entradas =
      Api.filterPage(pagina, "zonatmo.com/library/");

  std::cout << "Ingresa el numero del resultado a usar: \n";
  for (size_t h = 0; h < Entradas.size(); ++h) {
    std::cout << "opcion " << h << ": " << Entradas[h] << "\n";
  }
  int selecionado;
  std::cin >> selecionado;

  std::string manga = Api.GetRawPage("", Entradas[selecionado]);
  std::cout << "la pagina a usar sera: " << Entradas[selecionado] << "\n";

  // std::vector<std::string> capitulos =
  //     filterPage(manga, "zonatmo.com/view_uploads/");
  std::vector<Capitulo> MangaCapitulos =
      Api.GetChapters(manga, "fa fa-chevron-down fa-fw");

  tooling.imprimirTodosLosCapitulos(MangaCapitulos);

  return 0;
}
