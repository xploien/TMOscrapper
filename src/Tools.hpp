#include <vector>

#include "MangaData.hpp"

class Tools {
public:
  // Función para imprimir una traducción individual
  static void imprimirTraduccion(const Traducion &trad, int indice);
  // Función para imprimir un capítulo individual
  static void imprimirCapitulo(const Capitulo &cap, int indice, int total);
  // Función principal para imprimir todo el vector de capítulos
  static void imprimirTodosLosCapitulos(const std::vector<Capitulo> &capitulos);
};
