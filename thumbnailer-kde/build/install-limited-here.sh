#!/bin/bash

# Install limited embroidery thumbnailer (DST and JEF only)
# Run this from the build directory

echo "Installing Limited Embroidery Thumbnailer (DST and JEF only)..."

# Check if we're in the right directory
if [ ! -f "embroidery-thumbnailer" ]; then
    echo "Error: embroidery-thumbnailer not found. Run this from the build directory."
    exit 1
fi

# Install executable
sudo cp embroidery-thumbnailer /usr/local/bin/
sudo chmod +x /usr/local/bin/embroidery-thumbnailer

# Install limited thumbnailer config
sudo cp ../embroidery.thumbnailer /usr/share/thumbnailers/

# Update path in config
sudo sed -i 's|/usr/bin/embroidery-thumbnailer|/usr/local/bin/embroidery-thumbnailer|g' /usr/share/thumbnailers/embroidery.thumbnailer

# Install limited MIME types
sudo cp ../embroidery-mimetypes-limited.xml /usr/share/mime/packages/embroidery-limited.xml
sudo update-mime-database /usr/share/mime

# Clear thumbnail cache
rm -rf ~/.cache/thumbnails/*

echo "Limited thumbnailer installed!"
echo "Supports: DST and JEF files only"
echo "Test with DST files in Dolphin (F11 for preview mode)"