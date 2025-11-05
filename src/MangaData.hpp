#ifndef MANGADATA_H
#define MANGADATA_H

#include <string>
#include <vector>

class MangaData {};

class Traducion : public MangaData {
public:
  std::string Url;
  std::string NombreTraductor;
  std::vector<std::string> UrlImagenes;
  Traducion() : Url(""), NombreTraductor("") {}

  Traducion(const std::string &url, const std::string &nombreTraductor,
            const std::vector<std::string> &urlImagenes)
      : Url(url), NombreTraductor(nombreTraductor), UrlImagenes(urlImagenes) {}
};

class Capitulo : public MangaData {
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

#endif // MANGADATA_H
