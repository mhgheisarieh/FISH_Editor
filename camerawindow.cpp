#include "camerawindow.h"
#include "ui_camerawindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>


// CameraWindow::CameraWindow(AppContext* context, QWidget *parent)
CameraWindow::CameraWindow( QWidget *parent)

    : QDialog(parent)
    , ui(new Ui::CameraWindow)
    , m_cam(nullptr)
    // , context(context) // Initialize context

{
    if (!initializeCamera()) {
        QMessageBox::critical(this, "Error", "Failed to initialize the camera.");
        setResult(QDialog::Rejected);
        QTimer::singleShot(0, this, SLOT(close())); // Schedule the dialog to close immediately
        return;
    }

    ui->setupUi(this);


    connect(&m_timer, &QTimer::timeout, this, &CameraWindow::updateFrame);
    m_timer.start(30); // Update every 30 ms (~33 FPS)

    connect(ui->captureButton, &QPushButton::clicked, this, &CameraWindow::captureImage);
}

CameraWindow::~CameraWindow()
{
    if (m_cam) {
        Toupcam_Close(m_cam);
    }
    delete ui;
}

bool CameraWindow::initializeCamera()
{
    ToupcamDeviceV2 arr[TOUPCAM_MAX];
    unsigned nCount = Toupcam_EnumV2(arr);

    if (nCount == 0) {
        qDebug() << "No camera found.";
        return false;
    }

    // Open the camera
    m_cam = Toupcam_Open(arr[0].id);
    if (!m_cam) {
        qDebug() << "Failed to open the camera.";
        return false;
    }

    // Start the camera in pull mode
    if (FAILED(Toupcam_StartPullModeWithCallback(m_cam, CameraCallback, this))) {
        qDebug() << "Failed to start camera in pull mode.";
        return false;
    }

    return true;
}

void __stdcall CameraWindow::CameraCallback(unsigned nEvent, void* pCallbackCtx)
{
    // Currently not used, but necessary for the SDK callback
}

void CameraWindow::updateFrame()
{
    if (!m_cam)
        return;

    int width = 0, height = 0;
    if (SUCCEEDED(Toupcam_get_Size(m_cam, &width, &height))) {
        std::vector<unsigned char> buf(width * height * 3);  // 3 channels for RGB
        unsigned int bits = 24;

        if (SUCCEEDED(Toupcam_PullImage(m_cam, buf.data(), bits, nullptr, nullptr))) {
            QImage img(buf.data(), width, height, QImage::Format_RGB888);
            m_image = img.rgbSwapped(); // Convert from BGR to RGB
            ui->videoLabel->setPixmap(QPixmap::fromImage(m_image).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
        }
    }
}

void CameraWindow::captureImage()
{
    if (m_image.isNull()) {
        QMessageBox::warning(this, "Warning", "No image to capture.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;JPEG Files (*.jpg)");
    if (!fileName.isEmpty()) {
        m_image.save(fileName);
    }

    // Close the dialog after capturing the image
    accept();  // This will return QDialog::Accepted to MainWindow
}

QImage CameraWindow::getCapturedImage() const
{
    return m_image;
}
