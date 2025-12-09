#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Tools.hpp"
#include <iostream>

std::string MangaData::FindMangaURl(std::string titulo) {
  MyApi Api;
  std::string pagina =
      Api.GetRawPage(titulo, "https://zonatmo.com/library?title=");

  std::vector<std::string> Entradas =
      Api.filterPage(pagina, "zonatmo.com/library/");

  std::cout << "Ingresa el numero del resultado a usar: \n";
  for (size_t h = 0; h < Entradas.size(); ++h) {
    std::cout << "opcion " << h << ": " << Entradas[h] << "\n";
  }
  int selecionado;
  std::cin >> selecionado;

  return Entradas[selecionado];
}
std::vector<SearchResult>
MangaData::GetSearchResultsFromUrl(std::string titulo) {
  MyApi Api;
  std::string pagina =
      Api.GetRawPage(titulo, "https://zonatmo.com/library?title=");

  std::vector<SearchResult> Entradas =
      Api.filterPageWithImage(pagina, "zonatmo.com/library/");

  return Entradas;
}

Manga MangaData::GetMangaFromUrl(std::string Url) {
  MyApi Api;
  Manga MangaObtenido;
  std::string manga = Api.GetRawPage("", Url);
  std::cout << "la pagina a usar sera: " << Url << "\n";
  std::cout << "Esto podria tomar desde segundos a minutos se paciente \n";
  MangaObtenido.nombre = Api.extractSlug(Url);
  // std::cout << "deadbuggin nombre:" << MangaObtenido.nombre << "\n";
  MangaObtenido.capitulos = Api.GetChapters(manga, "fa fa-chevron-down fa-fw");
  MangaObtenido.numerocapitulos = MangaObtenido.capitulos.size();
  return MangaObtenido;
}
