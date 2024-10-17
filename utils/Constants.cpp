#include "Constants.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#ifdef MYLINUX
const QString Constants::DBROOT = "./db/";
#elif MYWIN32
const QString Constants::DBROOT = "./db/";
#endif

const int Constants::DONGLE_INTERVAL_MILLIS = 5 * 60 * 1000; // 5 minutes
const QString Constants::SETTINGS_FILE = "./config.ini";

const int Constants::CAMERA_CONTRAST_DEFAULT = 0;
const int Constants::CAMERA_BRIGHTNESS_DEFAULT = 0;
const int Constants::CAMERA_SATURATION_DEFAULT = 128;
const double Constants::CAMERA_GAMMA_DEFAULT = 1.0;
const bool Constants::CAMERA_GAIN_AUTO_DEFAULT = true;
const bool Constants::CAMERA_AUTO_EXPOSURE_DEFAULT = false;

const QString Constants::COMPANY = "Onyx";
const QString Constants::PRODUCT = "Karyotype";

const QString Constants::SEGMENT_MIME_TYPE = "segment_mime";

const QString Constants::CROP_COPY_MIME_TYPE = "crop_copy_mime";

const QRectF Constants::KARYOTYPE_SCENE_INITIAL_RECT(0, 0, 1500, 1000);

const double Constants::KARYOTYPE_CELL_INITIAL_WIDTH = Constants::KARYOTYPE_SCENE_INITIAL_RECT.width() / 7;

const int Constants::REPORT_WORD_MAX_IMAGES = 15;

void Constants::ShowMessageBox(const QString &text)
{
    QMessageBox qmsg;
    qmsg.setText(text);
    qmsg.setIcon(QMessageBox::Warning);
    qmsg.exec();
}

void Constants::WriteTextFile(const QString &text, const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        stream << text << Qt::endl;
    }
}
