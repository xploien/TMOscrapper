#include "Tools.hpp"
#include "MangaData.hpp"
#include <iomanip>
#include <iostream>

void Tools::imprimirTraduccion(Traducion &trad, int indice) {
  std::cout << "    Traducción " << indice + 1 << ":\n";
  std::cout << "      URL: " << trad.Url << "\n";
  std::cout << "      Traductor: " << trad.NombreTraductor << "\n";
  std::cout << "      URL Sin filtrar : " << trad.UnfilterdUrl << "\n";
  std::cout << "      urls de imagenes: " << "\n";
  for (const std::string &i : trad.UrlImagenes) {
    std::cout << i << "\n";
  }
  std::cout << "      --------------------\n";
}

void Tools::imprimirCapitulo(Capitulo &cap, int indice, int total) {
  std::cout << "═══════════════════════════════════════════════════\n";
  std::cout << "CAPÍTULO " << total - indice << ":\n";
  std::cout << "═══════════════════════════════════════════════════\n";
  std::cout << "Número: " << std::fixed << std::setprecision(2)
            << cap.NumCapitulo << "\n";
  std::cout << "Nombre: " << cap.NameCapitulo << "\n";
  std::cout << "Cantidad de traducciones: " << cap.traducciones.size() << "\n";

  if (!cap.traducciones.empty()) {
    std::cout << "\n  TRADUCCIONES:\n";
    std::cout << "  -------------------------------------------------\n";
    for (size_t i = 0; i < cap.traducciones.size(); ++i) {
      imprimirTraduccion(cap.traducciones[i], i);
    }
  } else {
    std::cout << "\n  No hay traducciones disponibles\n";
  }
  std::cout << "\n";
}

void Tools::imprimirTodosLosCapitulos(Manga manga) {
  if (manga.capitulos.empty()) {
    std::cout << "El vector de capítulos está vacío.\n";
    return;
  }

  std::cout << "\n";
  std::cout << "===================================================\n";
  std::cout << "          LISTA COMPLETA DE CAPÍTULOS\n";
  std::cout << "          Total: " << manga.capitulos.size() << " capítulos\n";
  std::cout << "===================================================\n\n";

  for (size_t i = 0; i < manga.capitulos.size(); ++i) {
    imprimirCapitulo(manga.capitulos[i], i, manga.capitulos.size());
  }

  std::cout << "===================================================\n";
  std::cout << "              FIN DE LA LISTA\n";
  std::cout << "===================================================\n";
};
