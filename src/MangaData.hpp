#ifndef MANGADATA_H
#define MANGADATA_H

#include <string>
#include <vector>

class Traducion {
public:
  std::string Url;
  std::string NombreTraductor;
  std::vector<std::string> UrlImagenes;
  std::string UnfilterdUrl;
  Traducion() : Url(""), NombreTraductor("") {}

  Traducion(const std::string &url, const std::string &nombreTraductor,
            const std::vector<std::string> &urlImagenes,
            const std::string unfilterdUrl)
      : Url(url), NombreTraductor(nombreTraductor), UrlImagenes(urlImagenes),
        UnfilterdUrl(unfilterdUrl) {}
};

class Capitulo {
public:
  float NumCapitulo;
  std::string NameCapitulo;
  std::vector<Traducion> traducciones;
  int NumTraduciones;

  Capitulo() : NumCapitulo(0), NameCapitulo(""), NumTraduciones(0) {}

  Capitulo(int numcapitulo, const std::string &namecapitulo,
           const std::vector<Traducion> &traducciones)
      : NumCapitulo(numcapitulo), NameCapitulo(namecapitulo),
        traducciones(traducciones), NumTraduciones(traducciones.size()) {}
};

class Manga {
public:
  std::string nombre;
  std::vector<Capitulo> capitulos;
  int numerocapitulos = capitulos.size();
};

class MangaData {
public:
  std::string FindMangaURl(std::string titulo);
  Manga GetMangaFromUrl(std::string Url);
};

#endif // MANGADATA_H
