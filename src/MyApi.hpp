#include "MangaData.hpp"
#include <regex>
#include <string>
#include <vector>

#include "Curl.hpp"

class MyApi {
public:
  std::string extractSlug(const std::string &url);

  std::string GetRawPage(std::string Titulo, std::string baseurl);

  std::string FindNextString(std::stringstream &ss,
                             std::streampos posicionActual,
                             const std::string NewTextoIdentificador,
                             const std::string TextoIdentificador,
                             std::regex patron);

  std::vector<Traducion> Findnext(std::stringstream &ss,
                                  std::streampos posicionActual,
                                  const std::string NewTextoIdentificador,
                                  const std::string TextoIdentificador,
                                  std::regex patron);

  std::vector<Capitulo> GetChapters(const std::string rawPage,
                                    const std::string TextoIdentificador);

  std::vector<std::string> filterPage(const std::string rawPage,
                                      const std::string TextoIdentificador);

  std::vector<std::string> CleanUrl(const std::vector<std::string> urlvector);
  std::string CleanUrlString(const std::string &url);

  std::vector<std::string> GetImageUrls(std::string url);
};
// obtiene la ultima frase de un url

//  std::regex patron(R"((https):\/\/[^\s\/$.?#].[^\s]*)");
// url
