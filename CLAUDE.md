# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A KDE/Dolphin thumbnailer for embroidery files (DST, JEF, PES formats) that generates preview thumbnails in the file manager. The project uses libembroidery for parsing embroidery files and Qt5/KDE Frameworks for rendering thumbnails.

## Build and Installation

**Quick build and install:**
```bash
cd thumbnailer-kde
./install.sh
```

The install script handles:
- CMake configuration and compilation
- Installation to `/usr/local/bin/`
- MIME type registration for DST, JEF and PES files
- KDE plugin cache update
- Thumbnail cache clearing

**Manual build:**
```bash
cd thumbnailer-kde
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

**Requirements:**
- Qt5 development libraries
- CMake
- KDE Frameworks 5 (KIO)
- C++ compiler with C++17 support

## Architecture

### Directory Structure

- `thumbnailer-kde/` - Main thumbnailer implementation
  - `embroidery-thumbnailer-standalone.cpp` - Core thumbnailer executable
  - `CMakeLists.txt` - Build configuration
  - `embroidery.thumbnailer` - Desktop service configuration
  - `embroidery-mimetypes.xml` - MIME type definitions
  - `install.sh` - Installation script

- `extern/libembroidery/` - Vendored embroidery parsing library
  - `src/embroidery.h` - Main library interface
  - `src/pattern.c` - Pattern data structures and manipulation
  - `src/formats.c` - File format readers/writers
  - `src/geometry.c` - Geometric operations

### Core Components

**Thumbnailer (embroidery-thumbnailer-standalone.cpp)**
- Standalone executable invoked by Dolphin via `embroidery.thumbnailer` service
- Command-line interface: `embroidery-thumbnailer [-s size] input_file output_file`
- Uses libembroidery's `emb_pattern_readAuto()` to parse embroidery files
- Renders stitches using QPainter with proper scaling and bounds calculation
- Handles stitch flags (NORMAL, JUMP, TRIM, STOP, END) to accurately render patterns
- Thread colors are retrieved from pattern thread_list

**libembroidery Integration**
- Compiled as static library (`embroidery_static`) with C99 standard
- Key data structures:
  - `EmbPattern`: Container for entire embroidery design
  - `EmbStitch`: Individual stitch with position (x, y), flags, and color index
  - `EmbThread`: Thread color information (RGB)
  - `EmbArray`: Generic array container for stitches and threads
- Y-axis is inverted during rendering (`-stitch.y`) due to coordinate system differences

### Rendering Logic

The thumbnailer implements KDE4-compatible rendering:
1. Calculates pattern bounds by skipping initial JUMP/TRIM stitches
2. Scales pattern to fit requested thumbnail size with padding
3. Renders stitches as connected paths using QPainterPath
4. Handles color changes at STOP flags, breaking paths and switching colors
5. Treats JUMP/TRIM as moveTo operations without stitching
6. Terminates rendering at END flag

## MIME Type Configuration

The thumbnailer is registered via `/usr/local/share/thumbnailers/embroidery.thumbnailer`:
```
TryExec=/usr/bin/embroidery-thumbnailer
Exec=/usr/bin/embroidery-thumbnailer -s %s %i %o
MimeType=application/x-dst;application/x-jef;application/x-pes;
```

Supported formats are defined in `embroidery-mimetypes.xml` and registered system-wide.

## Development Notes

- The project targets KDE Frameworks 5 (not KF6)
- libembroidery supports 60+ embroidery formats, but this thumbnailer currently only configures DST, JEF and PES
- To add more formats: update `embroidery-mimetypes.xml` and `embroidery.thumbnailer` MimeType field
- The thumbnailer expects files to be installed to system locations (`/usr/local/bin/`, `/usr/local/share/`)
- CMake install prefix can be customized via `CMAKE_INSTALL_PREFIX`

## Testing

After installation, test the thumbnailer:
1. Open Dolphin file manager
2. Navigate to folder with .dst, .jef or .pes files
3. Enable preview mode (F11)
4. Thumbnails should automatically generate

To test the standalone executable directly:
```bash
/usr/local/bin/embroidery-thumbnailer -s 256 input.dst output.png
```