#include "layer.h"

#include "project.h"

Layer::Layer(QObject *parent, const QString &name) : QObject(parent)
{
    this->name = name;
    this->visibility = true;
    this->opacity = 1.0;
    this->antialiasing = true;
}

Layer::~Layer() {
}

void Layer::setVisible(bool visibility) {
    this->visibility = visibility;
}

bool Layer::isVisible() const {
    return this->visibility;
}

void Layer::setName(const QString &name) {
    this->name = name;
}

QString & Layer::getName() {
    return this->name;
}

float Layer::getOpacity() const
{
    return this->opacity;
}

void Layer::setOpacity(float newOpacity)
{
    this->opacity = newOpacity;
}

void Layer::requestRepaint()
{
    QObject *parent = this->parent();
    if(parent) {
        ((Project*)parent)->requestRepaint();
    }
}

QSize Layer::getSize() const
{
    QObject *parent = this->parent();
    if(parent) {
        return ((Project*)parent)->getSize();
    }
    return QSize(0, 0);
}

void Layer::enableAntialiasing(bool enabled)
{
    this->antialiasing = enabled;
}

bool Layer::isAntialiasingEnabled() const
{
    return this->antialiasing;
}

void Layer_paintBgGrid(QPainter &painter, const QSize &size, const size_t step)
{
    // vykresleni pozadi obrazku (sachovnice)
    painter.fillRect(
                0,
                0,
                size.width(),
                size.height(),
                QBrush(Qt::white, Qt::SolidPattern));

    QBrush brush(QColor(200, 200, 200), Qt::SolidPattern);
    int step2 = 2 * step;
    int y_end = size.height();
    int x_end = size.width();

    int x, x_offset, step_x, step_y;
    for(int y = 0, i = 0; y < y_end; y += step) {
        x_offset = i++ % 2 == 0 ? 0.0f : step;
        step_y = y + step < y_end ? step : (y_end - y);
        for(x = x_offset; x < x_end; x += step2) {
            step_x = x + step < x_end ? step : (x_end - x);
            painter.fillRect(x, y, step_x, step_y, brush);
        }
    }
}