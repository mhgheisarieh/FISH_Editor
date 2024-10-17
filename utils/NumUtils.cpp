#include "NumUtils.h"
#include <cmath>
#include <numeric>

NumUtils::NumUtils()
{

}

template<typename T>
T NumUtils::stddev(const std::vector<T> &vec, T &mean)
{
    const size_t sz = vec.size();
        if (sz <= 1)
            return 0.0;

        // Calculate the mean
        mean = std::accumulate(vec.begin(), vec.end(), 0.0) / sz;

        // Now calculate the variance
        auto variance_func = [&mean, &sz](T accumulator, const T& val) {
            return accumulator + ((val - mean)*(val - mean) / (sz - 1));
        };

        return std::sqrt(std::accumulate(vec.begin(), vec.end(), 0.0, variance_func));
}

template<typename T>
bool NumUtils::isOutlier(const T &stddev, const T &mean, const T &value)
{
    return std::abs(value - mean) > 2.5 * stddev;
}

bool NumUtils::isSegmentOutlier(const Segment *segment, const QList<Segment *> segmentList)
{
    std::vector<double> areaVec;
    std::transform(segmentList.begin(), segmentList.end(), std::back_inserter(areaVec), std::mem_fun(&Segment::area));
    double mean;
    double stddev = NumUtils::stddev(areaVec, mean);
    return NumUtils::isOutlier(stddev, mean, segment->area());
}
