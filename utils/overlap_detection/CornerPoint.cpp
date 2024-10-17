#include "CornerPoint.h"





CornerPoint::CornerPoint(const cv::Point &pos, int index, int contourIndex, double distFromCenter):
    pos(pos), index(index), contourIndex(contourIndex), distFromCenter(distFromCenter)
{
}

CornerPoint::CornerPoint():
    pos(-1, -1), index(-1), contourIndex(-1), distFromCenter(1e100)
{

}


cv::Point CornerPoint::getPos() const
{
    return pos;
}

int CornerPoint::getIndex() const
{
    return index;
}

int CornerPoint::getContourIndex() const
{
    return contourIndex;
}

double CornerPoint::getDistFromCenter() const
{
    return distFromCenter;
}

void CornerPoint::setPos(const cv::Point &value)
{
    pos = value;
}

void CornerPoint::setIndex(int value)
{
    index = value;
}

void CornerPoint::setContourIndex(int value)
{
    contourIndex = value;
}

void CornerPoint::setDistFromCenter(double value)
{
    distFromCenter = value;
}
