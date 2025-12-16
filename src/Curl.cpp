#include "Curl.hpp"
#include <iostream>
#include <string>
#include <vector>
//
// size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
// {
//   size_t totalSize = size * nmemb;
//   std::vector<unsigned char> *buffer =
//       static_cast<std::vector<unsigned char> *>(userp);
//
//   if (!buffer || !contents) {
//     std::cerr << "WriteCallback: null pointer\n";
//     return 0;
//   }
//
//   try {
//     size_t oldSize = buffer->size();
//     buffer->resize(oldSize + totalSize);
//     std::memcpy(buffer->data() + oldSize, contents, totalSize);
//   } catch (const std::exception &e) {
//     std::cerr << "WriteCallback error: " << e.what() << "\n";
//     return 0;
//   }
//
//   return totalSize;
// }
// callback de curl
