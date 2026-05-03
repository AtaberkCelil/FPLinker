#!/bin/bash

set -e

echo "======================================"
echo "  FPLinker Installer"
echo "======================================"
echo ""

INSTALL_DIR="$HOME/.local/bin"
APP_DIR="$HOME/.local/share/FPLinker"
DESKTOP_DIR="$HOME/.local/share/applications"
ICON_DIR="$HOME/.local/share/icons"

echo "Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is not installed"
    exit 1
fi

if ! command -v flatpak &> /dev/null; then
    echo "Error: flatpak is not installed"
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: g++ or clang++ is required"
    exit 1
fi

echo ""
echo "Checking for Qt6..."
if ! pkg-config --exists Qt6Widgets 2>/dev/null; then
    echo "Qt6 not found. Installing..."
    if command -v pacman &> /dev/null; then
        sudo pacman -S --noconfirm qt6-base
    elif command -v apt &> /dev/null; then
        sudo apt install -y qt6-base-dev
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y qt6-qtbase-devel
    elif command -v zypper &> /dev/null; then
        sudo zypper install -y libqt6-qtbase-devel
    else
        echo "Please install Qt6 manually for your distribution"
        exit 1
    fi
fi

echo ""
echo "Building FPLinker..."

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "Installing FPLinker..."

mkdir -p "$INSTALL_DIR"
mkdir -p "$DESKTOP_DIR"
mkdir -p "$ICON_DIR"

cp "$BUILD_DIR/FPLinker" "$INSTALL_DIR/fplinker"
chmod +x "$INSTALL_DIR/fplinker"

echo ""
echo "Creating desktop entry..."

cat > "$DESKTOP_DIR/fplinker.desktop" << DESKTOP
[Desktop Entry]
Type=Application
Name=FPLinker
Comment=Graphical Flatpak Application Launcher
Exec=$INSTALL_DIR/fplinker
Icon=fplinker
Terminal=false
Categories=Utility;System;
StartupWMClass=FPLinker
DESKTOP

chmod +x "$DESKTOP_DIR/fplinker.desktop"

if [ -f "$SCRIPT_DIR/assets/fplinker.png" ]; then
    cp "$SCRIPT_DIR/assets/fplinker.png" "$ICON_DIR/fplinker.png"
    echo "Icon installed to $ICON_DIR/fplinker.png"
fi

echo ""
echo "======================================"
echo "  Installation Complete!"
echo "======================================"
echo ""
echo "You can now launch FPLinker:"
echo "  - From your application menu (search 'FPLinker')"
echo "  - From terminal: fplinker"
echo ""

if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    echo "Note: ~/.local/bin is not in your PATH."
    echo "Add this to your ~/.bashrc or ~/.zshrc:"
    echo "  export PATH=\"\$HOME/.local/bin:\$PATH\""
    echo ""
fi
