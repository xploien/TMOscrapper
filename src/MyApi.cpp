#include "MyApi.hpp"
#include <curl/easy.h>
#include <iostream>
#include <string>
#include <vector>

std::string MyApi::extractSlug(const std::string &url) {
  // Encontrar la última posición de '/'
  size_t lastSlash = url.find_last_of('/');
  if (lastSlash != std::string::npos) {
    // Extraer desde después de la última /
    return url.substr(lastSlash + 1);
  }
  return "sorry no se pudo encontrar ";
}

std::string MyApi::GetRawPage(std::string Titulo, std::string baseurl) {
  std::string url;
  if (Titulo != "" && baseurl != "") {
    url = baseurl + Titulo + "&_pg=1";
  } else if (baseurl != "") {
    url = baseurl; // Usar baseurl sin modificar
  }

  std::string RawPage;
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    // URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers,
                                "User-Agent: Mozilla/5.0 (Windows NT 10.0; "
                                "Win64; x64) AppleWebKit/537.36 (KHTML, like "
                                "Gecko) Chrome/120.0.0.0 Safari/537.36");
    headers = curl_slist_append(
        headers,
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    headers = curl_slist_append(headers, "Accept-Language: es-ES,es;q=0.9");
    headers = curl_slist_append(headers, "Referer: https://zonatmo.com/");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Seguir redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);

    // Soporte para cookies (en memoria)
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

    // SSL (si hay problemas de certificados)
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Callbacks
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &RawPage);

    // Timeout razonable
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Ejecutar
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
    }

    // Limpiar
    curl_slist_free_all(headers);
  }

  curl_easy_cleanup(curl);
  return RawPage;
}

std::vector<std::string> MyApi::GetImageUrls(const std::string url) {
  std::string Linea;
  std::string TextoIdentificador = "img1tmo.com/uploads/";
  std::string rawPage = GetRawPage("", url);
  std::vector<std::string> Resultado;
  std::stringstream ss(rawPage);
  std::regex patron(R"((https):\/\/[^\s\/$.?#].[^\s]*)");
  std::regex patronJSON(R"(\(['"]\[([^\]]+)\]['"]\))");
  std::smatch Coincidencia;
  std::smatch Coincidencia2;
  std::string baseimageurl;
  std::string imgs;
  while (std::getline(ss, Linea)) {
    if (Linea.find(TextoIdentificador) != std::string::npos &&
        Linea.find("var") != std::string::npos) {
      if (std::regex_search(Linea, Coincidencia, patron)) {

        std::string tempurl = Coincidencia.str();

        // Eliminar los últimos 2 caracteres
        if (tempurl.length() >= 2) {
          tempurl.pop_back();
          tempurl.pop_back();
        }
        baseimageurl = tempurl;
      }
    }
    if (Linea.find("JSON.parse") != std::string::npos &&
        Linea.find(".webp") != std::string::npos) {

      if (std::regex_search(Linea, Coincidencia2, patronJSON)) {

        imgs = Coincidencia2[1].str();
        break;
        // std::cout << "Debugregex!: " << imgs << "\n";
      }
    }
  }
  // // separar en un vector:
  std::regex re("\"([^\"]+)\""); // todo esté entre comillas

  for (std::sregex_iterator i =
           std::sregex_iterator(imgs.begin(), imgs.end(), re);
       i != std::sregex_iterator(); ++i) {
    std::string tempmath = (*i)[1];
    Resultado.push_back(baseimageurl + tempmath);
  }
  return Resultado;
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
        newtraducion.UnfilterdUrl = Uncleanurl;
        newtraducion.Url = CleanUrlString(Uncleanurl);
        newtraducion.UrlImagenes = GetImageUrls(newtraducion.Url);

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

        Resultado.push_back(url);
      }
    }
  }
  return Resultado;
}
