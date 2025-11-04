#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// obtiene la ultima frase de un url
std::string extractSlug(const std::string &url) {
  // Encontrar la última posición de '/'
  size_t lastSlash = url.find_last_of('/');
  if (lastSlash != std::string::npos) {
    // Extraer desde después de la última '/'
    return url.substr(lastSlash + 1);
  }
  return "sorry no se pudo encontrar ";
}

class Traducion {
public:
  std::string Url;
  std::string NombreTraductor;

  Traducion() : Url(""), NombreTraductor("") {}

  Traducion(const std::string &url, const std::string &nombreTraductor)
      : Url(url), NombreTraductor(nombreTraductor) {}
};

class Capitulo {
public:
  float NumCapitulo;
  std::string NameCapitulo;
  std::vector<Traducion> traducciones;
  int NumTraduciones = traducciones.size();
  Capitulo() : NumCapitulo(0), NameCapitulo("") {}

  Capitulo(int numcapitulo, const std::string &namecapitulo,
           const std::vector<Traducion> &traducciones)
      : NumCapitulo(numcapitulo), NameCapitulo(namecapitulo),
        traducciones(traducciones) {}
};

// callback de curl
size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *userp) {
  size_t totalSize = size * nmemb;
  userp->append((char *)contents, totalSize);
  return totalSize;
}

std::string GetRawPage(std::string Titulo, std::string baseurl) {

  std::string url;
  if (baseurl != "") {
    url = baseurl + Titulo + "&_pg=1";
    // std::cout << "llegamos aqui! \n";
  } else {

    url = baseurl;
  }

  std::string RawPage;
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if (curl) {
    // La URL que quieres
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Le dices a curl que use tu callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // Le pasas el string donde guardar
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &RawPage);

    // Ejecutar
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
    }
  }

  curl_easy_cleanup(curl);

  return RawPage;
}

//  std::regex patron(R"((https):\/\/[^\s\/$.?#].[^\s]*)");
// url
std::string FindNextString(std::stringstream &ss, std::streampos posicionActual,
                           const std::string NewTextoIdentificador,
                           const std::string TextoIdentificador,
                           std::regex patron) {

  std::string Resultado;
  std::smatch Coincidencia;

  std::string lineaExtra;
  int contadorLineas = 0;

  // Leer las próximas N líneas buscando traducciones
  while (std::getline(ss, lineaExtra) && contadorLineas < 50) {
    // Si encuentras otro capítulo, parar
    if (lineaExtra.find(TextoIdentificador) != std::string::npos) {
      // Retroceder para que el bucle principal lo capture
      ss.seekg(posicionActual);
      break;
    }

    if (lineaExtra.find(NewTextoIdentificador) != std::string::npos) {
      // Extraer URL del grupo
      if (std::regex_search(lineaExtra, Coincidencia, patron)) {

        std::string url = Coincidencia.str();

        // Eliminar los últimos 2 caracteres
        if (url.length() >= 2) {
          url.pop_back();
          url.pop_back();
          // url.pop_back();
          // url.pop_back();
          // url.pop_back();
        }
        Resultado = url;
      }
    }

    posicionActual = ss.tellg();
    contadorLineas++;
  }
  return Resultado;
}

std::vector<Traducion> Findnext(std::stringstream &ss,
                                std::streampos posicionActual,
                                const std::string NewTextoIdentificador,
                                const std::string TextoIdentificador,
                                std::regex patron) {

  std::vector<Traducion> Resultado;
  std::smatch Coincidencia;

  std::string lineaExtra;
  int contadorLineas = 0;
  std::streampos posicionInicial = ss.tellg();
  // Leer las próximas N líneas buscando traducciones
  while (std::getline(ss, lineaExtra) && contadorLineas < 50) {
    // Si encuentras otro capítulo, parar
    if (lineaExtra.find(TextoIdentificador) != std::string::npos) {
      // Retroceder para que el bucle principal lo capture
      ss.clear();
      ss.seekg(posicionActual);
      break;
    }

    if (lineaExtra.find(NewTextoIdentificador) != std::string::npos) {
      // Extraer URL del grupo

      Traducion newtraducion;
      if (std::regex_search(lineaExtra, Coincidencia, patron)) {

        std::string url = Coincidencia.str();

        // Eliminar los últimos 2 caracteres
        if (url.length() >= 5) {
          url.pop_back();
          url.pop_back();
          url.pop_back();
          url.pop_back();
          url.pop_back();
        }

        newtraducion.NombreTraductor = extractSlug(url);

        std::streampos posTemp = ss.tellg(); // Guardar posición
        newtraducion.Url =
            FindNextString(ss, posTemp, "zonatmo.com/view_uploads/",
                           NewTextoIdentificador, patron);
        Resultado.push_back(newtraducion);

        ss.clear();
        ss.seekg(posTemp);
      }
    }

    posicionActual = ss.tellg();
    contadorLineas++;
  }
  return Resultado;
}

std::vector<Capitulo> GetChapters(const std::string rawPage,
                                  const std::string TextoIdentificador) {
  std::string Linea;
  std::vector<Capitulo> Resultado;
  std::stringstream ss(rawPage);
  std::regex patron(R"(\b\d+\.\d+\b)"); // Números con punto decimal
  std::regex Newpatron(R"((https):\/\/[^\s\/$.?#].[^\s]*)");
  std::smatch Coincidencia;
  while (std::getline(ss, Linea)) {
    int i = 0;
    if (Linea.find(TextoIdentificador) != std::string::npos) {
      if (std::regex_search(Linea, Coincidencia, patron)) {

        std::string url = Coincidencia.str();

        float numcap = std::stof(url);

        Capitulo nuevoCapitulo;

        nuevoCapitulo.NumCapitulo = numcap;
        nuevoCapitulo.NameCapitulo = "sorry aun nose como obtener el nommbre";
        // nuevoCapitulo.NameCapitulo = "Capítulo " + std::to_string(numcap);

        std::streampos posicionActual = ss.tellg(); // Guardar posición
        nuevoCapitulo.traducciones =
            Findnext(ss, posicionActual, "zonatmo.com/groups/",
                     TextoIdentificador, Newpatron);

        Resultado.push_back(nuevoCapitulo);
      }
    }
  }
  return Resultado;
}

std::vector<std::string> filterPage(const std::string rawPage,
                                    const std::string TextoIdentificador) {
  std::string Linea;
  std::vector<std::string> Resultado;
  std::stringstream ss(rawPage);
  std::regex patron(R"((https):\/\/[^\s\/$.?#].[^\s]*)");
  std::smatch Coincidencia;
  while (std::getline(ss, Linea)) {
    int i = 0;
    if (Linea.find(TextoIdentificador) != std::string::npos) {
      if (std::regex_search(Linea, Coincidencia, patron)) {

        std::string url = Coincidencia.str();

        // Eliminar los últimos 2 caracteres
        if (url.length() >= 2) {
          url.pop_back();
          url.pop_back();
        }

        // Agregar al vector (¡push_back, no acceso por índice!)
        Resultado.push_back(url);
      }
    }
  }
  return Resultado;
}

// Función para imprimir una traducción individual
void imprimirTraduccion(const Traducion &trad, int indice) {
  std::cout << "    Traducción " << indice + 1 << ":\n";
  std::cout << "      URL: " << trad.Url << "\n";
  std::cout << "      Traductor: " << trad.NombreTraductor << "\n";
  std::cout << "      --------------------\n";
}

// Función para imprimir un capítulo individual
void imprimirCapitulo(const Capitulo &cap, int indice) {
  std::cout << "═══════════════════════════════════════════════════\n";
  std::cout << "CAPÍTULO " << indice + 1 << ":\n";
  std::cout << "═══════════════════════════════════════════════════\n";
  std::cout << "Número: " << std::fixed << std::setprecision(1)
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

// Función principal para imprimir todo el vector de capítulos
void imprimirTodosLosCapitulos(const std::vector<Capitulo> &capitulos) {
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
    imprimirCapitulo(capitulos[i], i);
  }

  std::cout << "===================================================\n";
  std::cout << "              FIN DE LA LISTA\n";
  std::cout << "===================================================\n";
}

int main(int argc, char *argv[]) {
  std::cout << "ingresa el titulo a buscar: \n";
  std::string Tosearch;
  std::cin >> Tosearch;

  std::string pagina =
      GetRawPage(Tosearch, "https://zonatmo.com/library?title=");

  std::vector<std::string> Entradas =
      filterPage(pagina, "zonatmo.com/library/");

  std::cout << "Ingresa el numero del resultado a usar: ";
  for (size_t h = 0; h < Entradas.size(); ++h) {
    // Usamos Entradas[h] para acceder al elemento en la posición 'h'
    std::cout << "opcion " << h << ": " << Entradas[h] << "\n";
  }
  int selecionado;
  std::cin >> selecionado;

  std::string manga = GetRawPage("", Entradas[selecionado]);
  std::cout << "la pagina a usar sera: " << Entradas[selecionado] << "\n";

  // std::vector<std::string> capitulos =
  //     filterPage(manga, "zonatmo.com/view_uploads/");
  std::vector<Capitulo> MangaCapitulos =
      GetChapters(manga, "fa fa-chevron-down fa-fw");

  imprimirTodosLosCapitulos(MangaCapitulos);

  return 0;
}
