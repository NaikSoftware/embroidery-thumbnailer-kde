# Embroidery Thumbnailer for KDE

A KDE/Dolphin thumbnailer for embroidery files that generates preview thumbnails in the file manager.

## Supported Formats

- DST (Tajima Embroidery Format)
- JEF (Janome Embroidery Format)

## Requirements

- Qt5 development libraries
- CMake
- KDE Frameworks 5
- C++ compiler

## Build and Install

```bash
cd thumbnailer-kde
./install.sh
```

This will:
- Install the thumbnailer executable to `/usr/local/bin/`
- Register MIME types for DST and JEF files
- Configure the thumbnailer service
- Clear thumbnail cache

## Usage

After installation, embroidery file thumbnails will appear automatically in Dolphin file manager when preview mode is enabled (F11).

## License

Based on libembroidery library for embroidery file parsing.