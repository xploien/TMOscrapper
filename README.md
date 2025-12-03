# TuMangaOnline Downloader
Permite descargar mangas enteros de tmo;
## update:
code is finally functional but at an early stage
can be compiled with cmake as a normal cmake build 
### Dependencies: 
- libzip
- curl

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libzip-dev \
    nlohmann-json3-dev

# Arch Linux
sudo pacman -S base-devel cmake curl libzip 

# Fedora
sudo dnf install gcc-c++ cmake libcurl-devel libzip-devel 
```
## To build:

```bash
git clone https://github.com/xploien/TMOscrapper mangadownloader
cd mangadownloader
mkdir -p build && cd build && cmake .. && make -j$(nproc) && cd ..
```

## Roadmap 
- [x] Get Manga data from tmo 
- [x] Image Downloader and packager
- [x] Serialization and Database
- [ ] Gui or Tui
- [ ] first release
