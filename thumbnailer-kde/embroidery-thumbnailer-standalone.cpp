/*
 * Standalone Embroidery Thumbnailer
 * Generates thumbnails for embroidery files
 * 
 * Usage: embroidery-thumbnailer [-s size] input_file output_file
 */

#include <QCoreApplication>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QString>
#include <QColor>
#include <QDebug>
#include <QCommandLineParser>

extern "C" {
#include "../extern/libembroidery/src/embroidery.h"
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("embroidery-thumbnailer");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Embroidery file thumbnailer");
    parser.addHelpOption();
    parser.addPositionalArgument("input", "Input embroidery file");
    parser.addPositionalArgument("output", "Output image file");
    
    QCommandLineOption sizeOption(QStringList() << "s" << "size",
                                  "Thumbnail size", "size", "256");
    parser.addOption(sizeOption);
    
    parser.process(app);
    
    const QStringList args = parser.positionalArguments();
    if (args.size() != 2) {
        qCritical() << "Error: Must specify input and output files";
        parser.showHelp(1);
    }
    
    const QString inputFile = args.at(0);
    const QString outputFile = args.at(1);
    const int size = parser.value(sizeOption).toInt();
    
    // Create pattern and read file
    EmbPattern *pattern = emb_pattern_create();
    if (!pattern) {
        qCritical() << "Failed to create pattern";
        return 1;
    }

    // Read the embroidery file
    if (!emb_pattern_readAuto(pattern, inputFile.toLocal8Bit().data())) {
        qCritical() << "Failed to read embroidery file:" << inputFile;
        emb_pattern_free(pattern);
        return 1;
    }

    // Calculate bounds from stitches - skip JUMP stitches for better bounds
    if (pattern->stitch_list->count == 0) {
        qCritical() << "No stitches in pattern";
        emb_pattern_free(pattern);
        return 1;
    }
    
    // Find first non-jump stitch for initial bounds
    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    bool boundsInitialized = false;
    
    for (int i = 0; i < pattern->stitch_list->count; i++) {
        EmbStitch stitch = pattern->stitch_list->stitch[i];
        
        // Skip initial jumps and focus on actual stitching coordinates
        if ((stitch.flags & (JUMP | TRIM)) && !boundsInitialized) {
            continue;
        }
        
        if (!boundsInitialized) {
            minX = maxX = stitch.x;
            minY = maxY = stitch.y;
            boundsInitialized = true;
        } else {
            if (stitch.x < minX) minX = stitch.x;
            if (stitch.x > maxX) maxX = stitch.x;
            if (stitch.y < minY) minY = stitch.y;
            if (stitch.y > maxY) maxY = stitch.y;
        }
    }
    
    double width = maxX - minX;
    double height = maxY - minY;
    
    if (width <= 0 || height <= 0) {
        qCritical() << "Invalid pattern dimensions";
        emb_pattern_free(pattern);
        return 1;
    }

    // Calculate scale to fit thumbnail size
    double scale = size / qMax(width, height);
    
    // Add some padding
    const int padding = 10;
    int imgWidth = static_cast<int>(width * scale) + 2 * padding;
    int imgHeight = static_cast<int>(height * scale) + 2 * padding;

    // Create image with white background
    QImage image(imgWidth, imgHeight, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // Translate to center the pattern
    painter.translate(padding - minX * scale, padding - minY * scale);
    painter.scale(scale, scale);

    // Draw stitches using KDE4 logic adapted to current API
    QPainterPath path;
    int previousColor = -1;
    
    // Initialize with first stitch color if available
    if (pattern->stitch_list->count > 0) {
        previousColor = pattern->stitch_list->stitch[0].color;
    }
    
    for (int i = 0; i < pattern->stitch_list->count; i++) {
        EmbStitch stitch = pattern->stitch_list->stitch[i];
        
        // Handle STOP and END exactly like KDE4 version
        if ((stitch.flags & STOP) || (stitch.flags & END)) {
            if (!path.isEmpty()) {
                // Get thread color (following KDE4 logic)
                QColor color(0, 0, 0); // Default black
                if (previousColor >= 0 && previousColor < pattern->thread_list->count) {
                    EmbThread thread = pattern->thread_list->thread[previousColor];
                    color = QColor(thread.color.r, thread.color.g, thread.color.b);
                }
                
                QPen pen(color);
                pen.setWidthF(0.35);
                pen.setCapStyle(Qt::RoundCap);
                pen.setJoinStyle(Qt::RoundJoin);
                
                painter.strokePath(path, pen);
                
                // Update color for next segment (exactly like KDE4)
                previousColor = stitch.color;
                path = QPainterPath();
            }
            
            // End of pattern - break like KDE4
            if (stitch.flags & END) {
                break;
            }
            
            // Start new path at this position (like KDE4)
            path.moveTo(stitch.x, -stitch.y);
        }
        else if ((stitch.flags & JUMP) || (stitch.flags & TRIM)) {
            // Move without stitching (exactly like KDE4)
            path.moveTo(stitch.x, -stitch.y);
        }
        else {
            // Normal stitch (exactly like KDE4)
            path.lineTo(stitch.x, -stitch.y);
        }
    }

    painter.end();
    
    // Save the image
    if (!image.save(outputFile)) {
        qCritical() << "Failed to save image to:" << outputFile;
        emb_pattern_free(pattern);
        return 1;
    }
    
    // Clean up
    emb_pattern_free(pattern);
    
    return 0;
}