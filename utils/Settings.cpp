#include "Constants.h"
#include "Settings.h"

#include <QMessageBox>

Settings *Settings::m_instance = nullptr;

Settings *Settings::instance()
{
    if (!m_instance)
        m_instance = new Settings();

    return m_instance;
}

const QString Settings::winWordPath() const
{
    QVariant value = configures.value("WINWORDPATH");

    if (value.isNull())
        QMessageBox::critical(nullptr, "WINWORDPATH config not found",
                              "Config file does not exist or WINWORDPATH is not set!");

    return value.toString();
}

bool Settings::flipChromosomesDirections() const
{
    return configures.value("flip_dir", QVariant(false)).toBool();
}

double Settings::dragOpacity() const
{
    return .6;
}

double Settings::chromosomeMiddlePortion() const
{
    return .6;
}

QString Settings::chromosomeClassifierModelPath() const
{
    return configures.value("chromosome_classifier", "tf_models/classifier_model").toString();
}

QString Settings::orientationClassifierModelPath() const
{
    return configures.value("orientation_classifier", "tf_models/orientation_model").toString();
}

CameraStatus Settings::cameraStatus() const
{

    QString camStatStr = configures.value("camera", "toupcam").toString();
    camStatStr = camStatStr.toLower();
    if (camStatStr == "basler")
        return BASLER;
    if (camStatStr == "toupcam")
        return TOUPCAM;
    if (camStatStr == "nncam")
        return NNCAM;

    return NO_CAMERA;
}

Settings::Settings() :
    configures(Constants::SETTINGS_FILE, QSettings::IniFormat)
{
}
