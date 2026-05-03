# FPLinker

**FPLinker** is a lightweight Linux desktop application that lets you browse and launch Flatpak applications with a clean, modern UI — no terminal required. Simply open FPLinker, search for your apps, and click Launch.

Inspired by [UELinker](https://github.com/AliGalipALATLI/UELinker).

![Platform](https://img.shields.io/badge/platform-Linux-lightgray.svg)
![C++](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Qt](https://img.shields.io/badge/framework-Qt6-green)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Features

- **Clean Dark UI** — Modern dark interface with tabbed layout
- **Installed Apps Tab** — Browse, search, launch, and remove installed Flatpak apps
- **Marketplace Tab** — Search Flathub and install apps directly from the UI
- **Instant App Launch** — One-click launch for any Flatpak app
- **Live Search** — Filter apps by name, ID, or description
- **App Icons** — Automatically loads Flatpak app icons
- **No Root Required** — Installs to user directories (`~/.local/`)
- **Wayland & X11 Support** — Works on both display servers

---

## Prerequisites

| Dependency | Version |
|------------|---------|
| CMake      | 3.20+   |
| Qt6 (Widgets) | 6.x  |
| GCC/Clang  | C++17 capable |
| Flatpak    | 1.0+    |

---

## Installation

### Quick Install

```bash
git clone https://github.com/YOUR_USERNAME/FPLinker.git
cd FPLinker
chmod +x install.sh
./install.sh
```

**What the script does:**

1. Checks for CMake, C++ compiler, and Flatpak
2. Installs Qt6 via system package manager if missing
3. Builds the project with CMake in Release mode
4. Installs binary to `~/.local/bin/fplinker`
5. Creates desktop entry in `~/.local/share/applications`

### Manual Build

```bash
# Install dependencies
# Arch: sudo pacman -S qt6-base cmake make gcc
# Debian/Ubuntu: sudo apt install qt6-base-dev cmake make g++
# Fedora: sudo dnf install qt6-qtbase-devel cmake make gcc-c++

git clone https://github.com/YOUR_USERNAME/FPLinker.git
cd FPLinker

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

mkdir -p ~/.local/bin
cp build/FPLinker ~/.local/bin/fplinker
chmod +x ~/.local/bin/fplinker

mkdir -p ~/.local/share/applications
cat > ~/.local/share/applications/fplinker.desktop << EOF
[Desktop Entry]
Type=Application
Name=FPLinker
Comment=Graphical Flatpak Application Launcher
Exec=$HOME/.local/bin/fplinker
Icon=fplinker
Terminal=false
Categories=Utility;System;
EOF
```

### Making `~/.local/bin` available in PATH

```bash
# For Bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc

# For Zsh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc
```

---

## Usage

1. Open your application menu and search for **FPLinker**, then launch it
2. Browse your installed Flatpak applications
3. Use the search box to filter apps
4. Click **Launch** to start any application

---

## Project Structure

```
FPLinker/
├── CMakeLists.txt
├── install.sh
├── README.md
├── assets/
│   └── fplinker.png
└── src/
    ├── main.cpp
    ├── MainWindow.h/.cpp
    ├── AppCard.h/.cpp
    ├── MarketCard.h/.cpp
    └── FlatpakManager.h/.cpp
```

---

## Tech Stack

| Layer        | Technology    |
|--------------|---------------|
| Language     | C++17         |
| GUI Framework| Qt6 (Widgets) |
| Build System | CMake 3.20+   |
| Target       | Linux x86_64  |

---

## License

[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](https://opensource.org/licenses/GPL-2.0)
