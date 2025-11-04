#ifndef MANGADATA_H
#define MANGADATA_H

#include <string>
#include <vector>

class MangaData {};

class Traducion : public MangaData { // También añadí 'public' aquí
public:
  std::string Url;
  std::string NombreTraductor;

  Traducion() : Url(""), NombreTraductor("") {}

  Traducion(const std::string &url, const std::string &nombreTraductor)
      : Url(url), NombreTraductor(nombreTraductor) {}
};

class Capitulo : public MangaData { // Y 'public' aquí también
public:
  float NumCapitulo;
  std::string NameCapitulo;
  std::vector<Traducion> traducciones;
  int NumTraduciones; // Esto no puede ser = traducciones.size() aquí

  Capitulo() : NumCapitulo(0), NameCapitulo(""), NumTraduciones(0) {}

  Capitulo(int numcapitulo, const std::string &namecapitulo,
           const std::vector<Traducion> &traducciones)
      : NumCapitulo(numcapitulo), NameCapitulo(namecapitulo),
        traducciones(traducciones), NumTraduciones(traducciones.size()) {}
};

#endif // MANGADATA_H
