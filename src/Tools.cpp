#include "Tools.hpp"
#include <iomanip>
#include <iostream>

void Tools::imprimirTraduccion(const Traducion &trad, int indice) {
  std::cout << "    Traducción " << indice + 1 << ":\n";
  std::cout << "      URL: " << trad.Url << "\n";
  std::cout << "      Traductor: " << trad.NombreTraductor << "\n";
  std::cout << "      --------------------\n";
}

void Tools::imprimirCapitulo(const Capitulo &cap, int indice, int total) {
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

void Tools::imprimirTodosLosCapitulos(const std::vector<Capitulo> &capitulos) {
  if (capitulos.empty()) {
    std::cout << "El vector de capítulos está vacío.\n";
    return;
  }

  std::cout << "\n";
  std::cout << "===================================================\n";
  std::cout << "          LISTA COMPLETA DE CAPÍTULOS\n";
  std::cout << "          Total: " << capitulos.size() << " capítulos\n";
  std::cout << "===================================================\n\n";

  for (size_t i = 0; i < capitulos.size(); ++i) {
    imprimirCapitulo(capitulos[i], i, capitulos.size());
  }

  std::cout << "===================================================\n";
  std::cout << "              FIN DE LA LISTA\n";
  std::cout << "===================================================\n";
};
