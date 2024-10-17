#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>


enum CameraStatus {
    NO_CAMERA,
    BASLER,
    TOUPCAM,
    NNCAM
};

class Settings
{
    QSettings configures;
    static Settings *m_instance;

    Settings();
public:
    static Settings *instance();
    const QString winWordPath() const;
    bool flipChromosomesDirections() const;
    double dragOpacity() const;
    double chromosomeMiddlePortion() const;

    QString chromosomeClassifierModelPath() const;
    QString orientationClassifierModelPath() const;


    CameraStatus cameraStatus() const;

};

#endif // SETTINGS_H
