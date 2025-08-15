#!/bin/bash

# KDE Embroidery Thumbnailer Installation Script

set -e

echo "Installing KDE Embroidery Thumbnailer..."

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo "cmake is required but not installed. Aborting." >&2; exit 1; }
command -v make >/dev/null 2>&1 || { echo "make is required but not installed. Aborting." >&2; exit 1; }

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build
echo "Building thumbnailer..."
make -j$(nproc)

# Install (requires sudo)
echo "Installing thumbnailer (requires sudo)..."
sudo make install

# Install MIME types
echo "Installing MIME types..."
sudo xdg-mime install --mode system ../embroidery-mimetypes.xml
sudo update-mime-database /usr/share/mime

# Update KDE plugin cache
echo "Updating KDE plugin cache..."
kbuildsycoca5 --noincremental

# Clear thumbnail cache to force regeneration
echo "Clearing thumbnail cache..."
rm -rf ~/.cache/thumbnails/*

echo "Installation complete!"
echo ""
echo "To enable thumbnails in Dolphin:"
echo "1. Open Dolphin"
echo "2. Go to Settings → Configure Dolphin"
echo "3. Click on 'General' → 'Previews'"
echo "4. Enable 'Embroidery Files'"
echo ""
echo "Then navigate to a folder with embroidery files and enable preview mode (F11)."