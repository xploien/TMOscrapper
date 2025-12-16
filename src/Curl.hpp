#ifndef CURL_HPP
#define CURL_HPP

#include <cstddef>
#include <string>

// size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

inline size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *userp) {
  size_t totalSize = size * nmemb;
  userp->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}
#endif
