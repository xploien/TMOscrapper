#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <zip.h>

namespace fs = std::filesystem;

bool addFileToZip(zip_t *zip, const fs::path &filePath) {
  std::ifstream input(filePath, std::ios::binary);
  if (!input) {
    std::cerr << "❌ No se pudo abrir: " << filePath << std::endl;
    return false;
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(input)),
                           std::istreambuf_iterator<char>());

  if (buffer.empty()) {
    std::cerr << "⚠️ Archivo vacío: " << filePath << std::endl;
    return false;
  }

  zip_source_t *source =
      zip_source_buffer(zip, buffer.data(), buffer.size(), 0);
  if (source == nullptr) {
    std::cerr << "❌ zip_source_buffer falló: " << filePath << " -> "
              << zip_strerror(zip) << std::endl;
    return false;
  }

  if (zip_file_add(zip, filePath.filename().u8string().c_str(), source,
                   ZIP_FL_OVERWRITE) < 0) {
    std::cerr << "❌ No se pudo agregar " << filePath << " -> "
              << zip_strerror(zip) << std::endl;
    zip_source_free(source);
    return false;
  }

  return true;
}

bool createCbzForDirectory(const fs::path &chapterDir) {
  std::string cbzName = chapterDir.filename().u8string() + ".cbz";
  int err = 0;
  zip_t *zip = zip_open(cbzName.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);

  if (!zip) {
    std::cerr << "❌ No se pudo crear: " << cbzName << std::endl;
    return false;
  }

  size_t count = 0;
  for (auto &entry : fs::directory_iterator(chapterDir)) {
    if (!entry.is_regular_file())
      continue;

    std::string ext = entry.path().extension().u8string();
    for (auto &c : ext)
      c = std::tolower(c);

    if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
      if (addFileToZip(zip, entry.path()))
        count++;
    }
  }

  zip_close(zip);
  std::cout << "✅ Creado " << cbzName << " (" << count << " imágenes)\n";
  return true;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <directorio raíz>\n";
    return 1;
  }

  fs::path root = fs::u8path(argv[1]);
  if (!fs::exists(root)) {
    std::cerr << "❌ No existe: " << root << std::endl;
    return 1;
  }

  if (!fs::is_directory(root)) {
    std::cerr << "❌ No es un directorio: " << root << std::endl;
    return 1;
  }

  std::cout << "📁 Directorio raíz: " << root << std::endl;

  bool found = false;
  for (auto &entry : fs::directory_iterator(root)) {
    if (entry.is_directory()) {
      found = true;
      createCbzForDirectory(entry.path());
    }
  }

  if (!found) {
    std::cerr << "⚠️ No se encontraron subcarpetas en " << root << std::endl;
  }

  return 0;
}
