#ifndef NUMUTILS_H
#define NUMUTILS_H

#include "Segment.h"
#include <vector>


class NumUtils
{
    NumUtils();
public:
    template<typename T> static T stddev(const std::vector<T> &vec, T &mean);
    template<typename T> static bool isOutlier(const T &stddev, const T &mean, const T &value);
    static bool isSegmentOutlier(const Segment *segment, const QList<Segment *> segmentList);
};

#endif // NUMUTILS_H
