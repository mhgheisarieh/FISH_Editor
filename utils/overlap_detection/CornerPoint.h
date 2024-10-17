#ifndef CORNERPOINT_H
#define CORNERPOINT_H

#include <opencv2/core.hpp>

enum ExitType{
    JUMP,
    NEXT
};

class CornerPoint
{
public:
    cv::Point pos;
    int index;
    int contourIndex;
    double distFromCenter;
    int numVisited = 0;
    bool isLastTimeJumpedOut = false;
public:
    CornerPoint(const cv::Point &pos, int index, int contourIndex, double distFromCenter);
    CornerPoint();
    cv::Point getPos() const;
    int getIndex() const;
    int getContourIndex() const;
    double getDistFromCenter() const;
    void setPos(const cv::Point &value);
    void setIndex(int value);
    void setContourIndex(int value);
    void setDistFromCenter(double value);
};

#endif // CORNERPOINT_H
