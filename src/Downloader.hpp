#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP
#include "MangaData.hpp"
#include "MyApi.hpp"
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <queue>
#include <string>

#include <vector>

namespace fs = std::filesystem;

extern std::vector<std::string> fulltraductores;

class Downloader {
private:
  MyApi api;
  int delayMilliseconds;
  int numThreads;
  std::string outputDirectory;

  std::mutex downloadMutex;
  std::mutex consoleMutex;

  // tarea de descarga
  struct DownloadTask {
    std::string url;
    std::string filename;
    int chapterIndex;
    int imageIndex;
  };

  // Cola safe?
  class DownloadQueue {
  private:
    std::queue<DownloadTask> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool finished = false;

  public:
    void push(const DownloadTask &task) {
      std::lock_guard<std::mutex> lock(mtx);
      queue.push(task);
      cv.notify_one();
    }

    bool pop(DownloadTask &task) {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [this] { return !queue.empty() || finished; });

      if (queue.empty()) {
        return false;
      }

      task = queue.front();
      queue.pop();
      return true;
    }

    void setFinished() {
      std::lock_guard<std::mutex> lock(mtx);
      finished = true;
      cv.notify_all();
    }

    size_t size() {
      std::lock_guard<std::mutex> lock(mtx);
      return queue.size();
    }
  };
  // encapsulacion go brrrrrrrrrrrr
  // Worker thread
  void downloadWorker(DownloadQueue &queue, std::atomic<int> &successCount,
                      std::atomic<int> &failCount,
                      std::atomic<int> &totalProcessed);

  // Helpers
  std::string createImageFilename(int index, const std::string &extension);
  std::string getExtensionFromUrl(const std::string &url);
  bool createCBZFromFolder(const fs::path &folder, const fs::path &outputFile);

public:
  // confifg defecto
  Downloader()
      : delayMilliseconds(5000), numThreads(2), outputDirectory("./output") {}

  // ia slop
  void setSpeed(int milliseconds) { delayMilliseconds = milliseconds; }

  void setThreads(int threads) { numThreads = threads; }

  void setOutputDir(const std::string &dir) { outputDirectory = dir; }

  // Getters
  int getSpeed() const { return delayMilliseconds; }
  int getThreads() const { return numThreads; }
  std::string getOutputDir() const { return outputDirectory; }

  // Métodos principales
  std::vector<int> MangaFilterIndex(Manga inputmanga);
  int GuiAnalizeFilterIndex(Manga inputmanga);
  bool MangatoCBZ(Manga &inputmanga, std::vector<int> index);
};

#endif // DOWNLOADER_HPP
