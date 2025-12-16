#include <vector>

#include "MangaData.hpp"

class Tools {
public:
  void imprimirTraduccion(Traducion &trad, int indice);
  void imprimirCapitulo(Capitulo &cap, int indice, int total);
  void imprimirTodosLosCapitulos(Manga manga);
};
