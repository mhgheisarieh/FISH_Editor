#ifndef OVERLAPDETECTION_H
#define OVERLAPDETECTION_H

#include <opencv2/core.hpp>
#include "CornerPoint.h"

using namespace std;

class OverlapDetection
{
    OverlapDetection();
public:

    static vector<CornerPoint> findCornerPoints(const vector<vector<cv::Point>> &contours,
                                                                  const cv::Point &p);


    static vector<CornerPoint> findCornerPoints2(const vector<vector<cv::Point>> &contours,
                                                                  const cv::Point &p);

    static void resolveOverlap(const vector<vector<cv::Point>> &contours,
                               vector<CornerPoint> &corners,
                               vector<vector<vector<cv::Point>>> &result);
};

#endif // OVERLAPDETECTION_H
