#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H


#include <QDialog>
#include <QImage>
#include <QTimer>
#include <QWidget>
#include "toupcam.h"
// #include "base/app_context.h"


QT_BEGIN_NAMESPACE
namespace Ui { class CameraWindow; }
QT_END_NAMESPACE

class CameraWindow : public QDialog
{
    Q_OBJECT

public:
    // explicit CameraWindow(AppContext* context, QWidget *parent = nullptr);
    explicit CameraWindow(QWidget *parent = nullptr);

    ~CameraWindow();

    QImage getCapturedImage() const;
    QString getImagePath() const;


private slots:
    void updateFrame();
    void captureImage();
    // void browseImagePath();

private:
    Ui::CameraWindow *ui;
    HToupCam m_cam;
    QImage m_image;
    QTimer m_timer;

    // AppContext* context;
    QString m_imagePath;

    bool initializeCamera();
    static void __stdcall CameraCallback(unsigned nEvent, void* pCallbackCtx);
};

#endif // CAMERAWINDOW_H
