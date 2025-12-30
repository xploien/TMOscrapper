# TuMangaOnline Downloader
Permite descargar mangas enteros de tmo;
## update:
code is finally functional but at an early stage
can be compiled with cmake as a normal cmake build 
### Dependencies: 
- libzip
- curl
- raylib

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libzip-dev \
    nlohmann-json3-dev \
    raylib

# Arch Linux
sudo pacman -S base-devel cmake curl libzip raylib 

# Fedora
sudo dnf install gcc-c++ cmake libcurl-devel libzip-devel raylib
```
## To build:

```bash
git clone https://github.com/xploien/TMOscrapper mangadownloader
cd mangadownloader
mkdir -p build && cd build && cmake .. && make -j$(nproc) && cd ..
```

### libraries that are not dependencies used:

- Cereal to serialize Manga data in a json format. 
- Simplewebp as a workarround to load webp image into raylib since raylib dosent support them out of the box.
- Clay with raylib as its renderer for gui.

## Roadmap 
- [x] Get Manga data from tmo 
- [x] Image Downloader and packager
- [x] Serialization and Database
- [ ] Gui or Tui
- [ ] first release
