#include "MyApi.hpp"
#include <curl/easy.h>
#include <string>
#include <vector>

std::string MyApi::extractSlug(const std::string &url) {
  // Encontrar la última posición de '/'
  size_t lastSlash = url.find_last_of('/');
  if (lastSlash != std::string::npos) {
    // Extraer desde después de la última '/'
    return url.substr(lastSlash + 1);
  }
  return "sorry no se pudo encontrar ";
}

std::string MyApi::CleanUrlString(const std::string &url) {
  const std::string &referer = "https://zonatmo.com/";
  CURL *curl = curl_easy_init();
  if (!curl)
    return "";

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
  curl_easy_setopt(curl, CURLOPT_REFERER, referer.c_str());
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
  curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Solo headers, no descargar body

  CURLcode res = curl_easy_perform(curl);

  std::string finalUrl;
  if (res == CURLE_OK) {
    char *finalUrlPtr = nullptr;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &finalUrlPtr);
    if (finalUrlPtr) {
      finalUrl = std::string(finalUrlPtr);
    }
  }

  curl_easy_cleanup(curl);
  return finalUrl;
}
//
// std::vector<std::string>
// MyApi::CleanUrl(const std::vector<std::string> urlvector) {
//   std::vector<std::string> Resultado;
//   const std::string &referer = "https://zonatmo.com/";
//   CURL *curl = curl_easy_init();
//
//   for (std::string i : urlvector) {
//     curl_easy_setopt(curl, CURLOPT_URL, i.c_str());
//     curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//     curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
//     curl_easy_setopt(curl, CURLOPT_REFERER, referer.c_str());
//     curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
//     curl_easy_setopt(curl, CURLOPT_NOBODY,
//                      1L); // Solo headers, no descargar body
//
//     CURLcode res = curl_easy_perform(curl);
//     std::string finalUrl;
//
//     if (res == CURLE_OK) {
//       char *finalUrlPtr = nullptr;
//       curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &finalUrlPtr);
//       if (finalUrlPtr) {
//         Resultado.push_back(std::string(finalUrlPtr));
//       }
//     }
//   }
//   curl_easy_cleanup(curl);
//   return Resultado;
// }

std::string MyApi::GetRawPage(std::string Titulo, std::string baseurl) {

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

std::string MyApi::FindNextString(std::stringstream &ss,
                                  std::streampos posicionActual,
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

        url.pop_back();
        // Eliminar los últimos 2 caracteres
        // if (url.length() >= 2) {
        //   url.pop_back();
        //   url.pop_back();
        //   // url.pop_back();
        //   // url.pop_back();
        //   // url.pop_back();
        // }
        Resultado = url;
      }
    }

    posicionActual = ss.tellg();
    contadorLineas++;
  }
  return Resultado;
}

std::vector<Traducion> MyApi::Findnext(std::stringstream &ss,
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
        if (url.length() >= 6) {
          url.pop_back();
          url.pop_back();
          url.pop_back();
          url.pop_back();
          url.pop_back();
          url.pop_back();
        }

        newtraducion.NombreTraductor = extractSlug(url);

        std::streampos posTemp = ss.tellg(); // Guardar posición
        std::string Uncleanurl =
            FindNextString(ss, posTemp, "zonatmo.com/view_uploads/",
                           NewTextoIdentificador, patron);
        newtraducion.Url = CleanUrlString(Uncleanurl);

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

std::vector<Capitulo> MyApi::GetChapters(const std::string rawPage,
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

std::vector<std::string>
MyApi::filterPage(const std::string rawPage,
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
