cmake_minimum_required(VERSION 3.16)
project(EmbroideryThumbnailer)

# Find required Qt5 and KDE Frameworks
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 REQUIRED COMPONENTS Core Gui)
find_package(KF5 REQUIRED COMPONENTS KIO)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for libembroidery
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern/libembroidery/src)

# Build libembroidery as a static library
add_library(embroidery_static STATIC
    ../extern/libembroidery/src/pattern.c
    ../extern/libembroidery/src/formats.c
    ../extern/libembroidery/src/geometry.c
    ../extern/libembroidery/src/data.c
    ../extern/libembroidery/src/compress.c
    ../extern/libembroidery/src/script.c
)

# Set C standard for libembroidery
set_target_properties(embroidery_static PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Create the standalone thumbnailer executable
add_executable(embroidery-thumbnailer
    embroidery-thumbnailer-standalone.cpp
)

# Link libraries
target_link_libraries(embroidery-thumbnailer
    Qt5::Core
    Qt5::Gui
    embroidery_static
    m  # Math library for libembroidery
)

# Install the executable
install(TARGETS embroidery-thumbnailer
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

# Install the thumbnailer desktop file
install(FILES embroidery.thumbnailer
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/thumbnailers
)