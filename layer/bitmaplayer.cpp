#include "bitmaplayer.h"

BitmapLayer::BitmapLayer(QObject *parent, const QString &name, const QSize &size) : Layer(parent, name)
{
    this->size = size;
    if(size.width() >= 1 && size.height() >= 1) {
        this->pixmap = QPixmap(size);
    }
}

const QSize &BitmapLayer::getSize() const
{
    return this->size;
}

void BitmapLayer::setSize(const QSize &newSize)
{
    if(newSize.width() >= 1 && newSize.height() >= 1) {
        this->size = newSize;
        this->pixmap = this->pixmap.scaled(this->size.width(), this->size.height(), Qt::KeepAspectRatio);
    }
}

void BitmapLayer::paintEvent(QPainter &painter)
{
    painter.drawPixmap(
                0, 0,
                this->size.width(), this->size.height(),
                this->pixmap);
}

int BitmapLayer::getType()
{
    return BITMAP_LAYER_TYPE;
}