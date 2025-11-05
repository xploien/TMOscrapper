#include <vector>

#include "MangaData.hpp"

class Tools {
public:
  // Función para imprimir una traducción individual
  void imprimirTraduccion(Traducion &trad, int indice);
  // Función para imprimir un capítulo individual
  void imprimirCapitulo(Capitulo &cap, int indice, int total);
  // Función principal para imprimir todo el vector de capítulos
  void imprimirTodosLosCapitulos(std::vector<Capitulo> &capitulos);
};
