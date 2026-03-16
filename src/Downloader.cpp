#include "Downloader.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_map> // index
#include <zip.h>

std::vector<std::string> fulltraductores;

std::vector<int> Downloader::MangaFilterIndex(Manga inputmanga) {
  std::vector<int> resultado;
  int numeroCapitulos = inputmanga.numerocapitulos;

  // integrity check
  for (Capitulo &capitulo : inputmanga.capitulos) {
    if (capitulo.NumTraduciones == 0) {
      std::cerr << "insufficient traductions" << std::endl;
    }
  }

  std::unordered_map<std::string, int> CapitulosTraductor;
  for (Capitulo &capitulo : inputmanga.capitulos) {
    for (Traducion &traducion : capitulo.traducciones) {
      CapitulosTraductor[traducion.NombreTraductor]++;
    }
  }

  for (auto &[traducion, numero] : CapitulosTraductor) {
    if (numero == numeroCapitulos) {
      fulltraductores.push_back(traducion);
    }
  }
  if (fulltraductores.empty()) {
    std::cout << "ninguno fue constante\n";
    std::vector<int> indice(inputmanga.numerocapitulos, 0);
    return indice;
  } else {
    std::cout << "Elije el traductor:\n";
    int iy = 1;
    for (std::string trad : fulltraductores) {
      std::cout << iy << " " << trad << "\n";
      iy++;
    }

    int usertradselect;
    std::cin >> usertradselect;
    std::string traductorselecionado = fulltraductores[usertradselect - 1];
    for (Capitulo cap : inputmanga.capitulos) {
      for (int i = 0; i < cap.NumTraduciones; i++) {
        if (cap.traducciones[i].NombreTraductor == traductorselecionado) {
          resultado.push_back(i);
          break;
        }
      }
    }
  }

  return resultado;
}

int Downloader::GuiAnalizeFilterIndex(Manga inputmanga) {

  int numeroCapitulos = inputmanga.numerocapitulos;

  for (Capitulo &capitulo : inputmanga.capitulos) {
    if (capitulo.NumTraduciones == 0) {
      std::cerr << "insufficient traductions" << std::endl;
      return 0;
    }
  }

  std::unordered_map<std::string, int> CapitulosTraductor;
  for (Capitulo &capitulo : inputmanga.capitulos) {
    for (Traducion &traducion : capitulo.traducciones) {
      CapitulosTraductor[traducion.NombreTraductor]++;
    }
  }

  fulltraductores.clear();
  for (auto &[traducion, numero] : CapitulosTraductor) {
    if (numero == numeroCapitulos) {
      fulltraductores.push_back(traducion);
    }
  }

  if (fulltraductores.empty()) {
    return 1;
  } else {
    return 2;
  }
  std::cerr << "Analizer failed\n";
  return 0;
}

// private functions
void Downloader::downloadWorker(DownloadQueue &queue,
                                std::atomic<int> &successCount,
                                std::atomic<int> &failCount,
                                std::atomic<int> &totalProcessed) {
  DownloadTask task;

  while (queue.pop(task)) {
    {
      std::lock_guard<std::mutex> lock(downloadMutex);
      std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }

    bool success = api.DownloadImage(task.url, task.filename);

    if (success) {
      successCount++;
      {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "[Cap " << task.chapterIndex + 1 << "]["
                  << std::setw(3) << std::setfill('0') << task.imageIndex + 1
                  << "] ok" << std::endl;
      }
    } else {
      failCount++;
      {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cerr << "[Cap " << task.chapterIndex + 1 << "]["
                  << std::setw(3) << std::setfill('0') << task.imageIndex + 1
                  << "] error" << std::endl;
      }
    }

    totalProcessed++;
  }
}

std::string Downloader::createImageFilename(int index,
                                            const std::string &extension) {
  std::ostringstream oss;
  oss << std::setw(3) << std::setfill('0') << index << extension;
  return oss.str();
}

std::string Downloader::getExtensionFromUrl(const std::string &url) {
  size_t dotPos = url.find_last_of('.');
  size_t queryPos = url.find('?');

  if (dotPos != std::string::npos) {
    std::string ext =
        url.substr(dotPos, queryPos != std::string::npos ? queryPos - dotPos
                                                         : std::string::npos);

    if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".gif" ||
        ext == ".webp") {
      return ext;
    }
  }

  return ".webm"; // fallback
}

bool Downloader::createCBZFromFolder(const fs::path &folder,
                                     const fs::path &outputFile) {
  int errorp;
  zip_t *archive =
      zip_open(outputFile.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errorp);

  if (!archive) {
    zip_error_t error;
    zip_error_init_with_code(&error, errorp);
    std::cerr << "Error al crear CBZ: " << zip_error_strerror(&error) << std::endl;
    zip_error_fini(&error);
    return false;
  }

  std::vector<fs::path> images;
  for (const auto &entry : fs::directory_iterator(folder)) {
    if (entry.is_regular_file()) {
      images.push_back(entry.path());
    }
  }

  std::sort(images.begin(), images.end());

  for (const auto &imgPath : images) {
    zip_source_t *source = zip_source_file(archive, imgPath.c_str(), 0, 0);

    if (!source) {
      std::cerr << "Error al leer: " << imgPath << std::endl;
      zip_close(archive);
      return false;
    }

    std::string filename = imgPath.filename().string();
    zip_int64_t index =
        zip_file_add(archive, filename.c_str(), source, ZIP_FL_OVERWRITE);

    if (index < 0) {
      std::cerr << "Error al agregar: " << filename << std::endl;
      zip_source_free(source);
      zip_close(archive);
      return false;
    }
  }

  if (zip_close(archive) < 0) {
    std::cerr << "Error al cerrar archivo: " << zip_strerror(archive) << std::endl;
    return false;
  }

  return true;
}

// public main

bool Downloader::MangatoCBZ(Manga &inputmanga, std::vector<int> index) {
  std::cout << "Descargando: " << inputmanga.nombre << "\n";
  std::cout << "Capitulos: " << inputmanga.numerocapitulos
            << " | Delay: " << delayMilliseconds << "ms"
            << " | Threads: " << numThreads << "\n";

  std::vector<int> masterIndex = index;

  // Carpeta del manga: outputDirectory/nombre_manga/
  fs::path mangaDir = fs::path(outputDirectory) / inputmanga.nombre;
  fs::create_directories(mangaDir);

  std::atomic<int> successCount(0);
  std::atomic<int> failCount(0);
  std::atomic<int> totalProcessed(0);
  int totalImages = 0;

  for (size_t i = 0; i < inputmanga.capitulos.size(); i++) {
    int tradIndex = masterIndex[i];
    totalImages +=
        inputmanga.capitulos[i].traducciones[tradIndex].UrlImagenes.size();
  }

  std::cout << "Total imagenes: " << totalImages << "\n";

  for (size_t capIndex = 0; capIndex < inputmanga.capitulos.size(); capIndex++) {
    Capitulo &capitulo = inputmanga.capitulos[capIndex];
    int tradIndex = masterIndex[capIndex];
    Traducion &traduccion = capitulo.traducciones[tradIndex];

    std::cout << "Cap " << capitulo.NumCapitulo;
    if (!capitulo.NameCapitulo.empty()) {
      std::cout << ": " << capitulo.NameCapitulo;
    }
    std::cout << " (" << traduccion.UrlImagenes.size() << " imgs)" << std::endl;

    // Carpeta temporal dentro de la carpeta del manga
    std::ostringstream tempDirName;
    tempDirName << "temp_cap_" << std::setw(3) << std::setfill('0')
                << (int)capitulo.NumCapitulo;
    fs::path tempDir = mangaDir / tempDirName.str();
    fs::create_directories(tempDir);

    DownloadQueue downloadQueue;

    for (size_t imgIndex = 0; imgIndex < traduccion.UrlImagenes.size(); imgIndex++) {
      std::string url = traduccion.UrlImagenes[imgIndex];
      std::string ext = getExtensionFromUrl(url);
      std::string filename =
          (tempDir / createImageFilename(imgIndex, ext)).string();

      DownloadTask task;
      task.url = url;
      task.filename = filename;
      task.chapterIndex = capIndex;
      task.imageIndex = imgIndex;

      downloadQueue.push(task);
    }

    std::vector<std::thread> workers;
    for (int i = 0; i < numThreads; i++) {
      workers.emplace_back(&Downloader::downloadWorker, this,
                           std::ref(downloadQueue), std::ref(successCount),
                           std::ref(failCount), std::ref(totalProcessed));
    }

    downloadQueue.setFinished();

    for (auto &worker : workers) {
      if (worker.joinable()) {
        worker.join();
      }
    }

    // CBZ dentro de la carpeta del manga
    std::ostringstream cbzName;
    cbzName << std::setw(3) << std::setfill('0') << (int)capitulo.NumCapitulo
            << "_" << capitulo.NameCapitulo << ".cbz";

    fs::path cbzPath = mangaDir / cbzName.str();

    if (createCBZFromFolder(tempDir, cbzPath)) {
      auto fileSize = fs::file_size(cbzPath);
      std::cout << "CBZ: " << cbzPath.filename().string() << " ("
                << std::fixed << std::setprecision(2)
                << (fileSize / 1024.0 / 1024.0) << " MB)\n";
      fs::remove_all(tempDir);
    } else {
      std::cerr << "Error CBZ cap " << capitulo.NumCapitulo << "\n";
    }
  }

  std::cout << "Listo: " << successCount << "/" << totalImages << " exitosas";
  if (failCount > 0) {
    std::cout << ", " << failCount << " fallidas";
  }
  std::cout << "\n";

  return failCount == 0;
}
