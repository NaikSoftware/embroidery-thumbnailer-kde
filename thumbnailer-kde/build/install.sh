#!/bin/bash

# Simple install script for embroidery thumbnailer
# Run from build directory

echo "Installing Embroidery Thumbnailer (DST and JEF only)..."

# Check if executable exists
if [ ! -f "embroidery-thumbnailer" ]; then
    echo "Error: embroidery-thumbnailer not found. Run 'make' first."
    exit 1
fi

# Install executable
echo "Installing executable..."
sudo cp embroidery-thumbnailer /usr/local/bin/
sudo chmod +x /usr/local/bin/embroidery-thumbnailer

# Install thumbnailer config
echo "Installing thumbnailer config..."
sudo cp ../embroidery.thumbnailer /usr/share/thumbnailers/

# Fix path in config
sudo sed -i 's|/usr/bin/embroidery-thumbnailer|/usr/local/bin/embroidery-thumbnailer|g' /usr/share/thumbnailers/embroidery.thumbnailer

# Install MIME types
echo "Installing MIME types..."
sudo cp ../embroidery-mimetypes.xml /usr/share/mime/packages/
sudo update-mime-database /usr/share/mime

# Clear thumbnail cache
echo "Clearing thumbnail cache..."
rm -rf ~/.cache/thumbnails/*

echo ""
echo "Installation complete!"
echo "Supports: DST and JEF files"
echo "Test in Dolphin with preview mode (F11)"