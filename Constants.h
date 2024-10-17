#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QRectF>
#include <qstring.h>
#include <iostream>


class Constants
{
public:
    static const QString DBROOT;
    static const QString SETTINGS_FILE;
    static const int DONGLE_INTERVAL_MILLIS;
    static void ShowMessageBox(const QString &text);
    static void WriteTextFile(const QString &text, const QString &fileName);
    static const int CAMERA_BRIGHTNESS_DEFAULT;
    static const int CAMERA_CONTRAST_DEFAULT;
    static const int CAMERA_SATURATION_DEFAULT;
    static const bool CAMERA_GAIN_AUTO_DEFAULT;
    static const bool CAMERA_AUTO_EXPOSURE_DEFAULT;
    static const double CAMERA_GAMMA_DEFAULT;

    static const QString COMPANY;
    static const QString PRODUCT;

    static const QString SEGMENT_MIME_TYPE;

    static const QString CROP_COPY_MIME_TYPE;

    static const QRectF KARYOTYPE_SCENE_INITIAL_RECT;

    static const double KARYOTYPE_CELL_INITIAL_WIDTH;

    static const int REPORT_WORD_MAX_IMAGES;
};

#endif // CONSTANTS_H
