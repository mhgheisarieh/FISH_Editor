//
// Created by mostafa on 5/19/2019.
//

#ifndef KARYO_IMAGE_PROC_IMAGEUTILS_H
#define KARYO_IMAGE_PROC_IMAGEUTILS_H


#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/stitching.hpp>

#include "Segment.h"

#define MIN_CONTOUR_AREA 100


class ImageUtils {
    static cv::Ptr<cv::Stitcher> stitcher;

    ImageUtils();
public:
    //============ Segmentation functions =====================
    static std::vector<Segment *> segmentation(const cv::Mat &image, int thresh);
    static std::vector<Segment *> getSegments(const cv::Mat &image, const std::vector<std::vector<cv::Point>> &contours, const std::vector<cv::Vec4i> &hierarchy);
    static int getNumSegments(const std::vector<std::vector<cv::Point>> &contours, const std::vector<cv::Vec4i> &hierarchy);
    static cv::Rect getBoundingBox(const vector<cv::Point> &points, int maxAllowedX, int maxAllowedY, int radius);
    static cv::Rect unionBoundingBox(cv::Rect bb1, cv::Rect bb2);
    static double contourArea(const vector<cv::Point> contour);
    static Segment *join(const vector<Segment *> &segments, const vector<cv::Point> &points, int size, bool truncate = false);
    static vector<cv::Point> truncatePolyline(const vector<Segment *> &segments, const vector<cv::Point> &points);
    static cv::Rect overallBoundingBox(const vector<Segment *> &segments);
    static void contoursCalc(const cv::Mat &image, int thresh, vector<vector<cv::Point>> &contours, vector<cv::Vec4i> &hierarchy);
    static void skeleton(const cv::Mat &src, cv::Mat &dst);

    //============= Image Enhancement functions ===========================
    static void gammaAdjust(const cv::Mat &src, cv::Mat &dst, double gamma, double lowThresh, double highThresh);
    static void equalizeHistogram(const cv::Mat &src, cv::Mat &dst, int thresh = 255);
    static void sharpen(const cv::Mat &src, cv::Mat &dst, int radius, double amount);
    static void threshold(const cv::Mat &src, cv::Mat &dst, int highThresh, int lowThresh = 0);
    static std::vector<float> histCalc(const cv::Mat &image, int histSize);
    static void cropByPolygon(const cv::Mat &src, cv::Mat &dst, const QPolygonF &polygon);
    static void removeBackground(const cv::Mat &src, cv::Mat &dst);
    static void smooth(const cv::Mat &src, cv::Mat &dst, int kSize = 3);
    static void contrastBrightnessAdjust(const cv::Mat &src, cv::Mat &dst, double contrast, double brightness);

    //=============== Image Processing functions ===============
    static void stitch(const vector<cv::Mat> &images, cv::Mat &stitchedImage);
    static QImage stitch(const QVector<QImage> &images);
    static int opencvType(QImage::Format format);
    static void imageNot(const cv::Mat &src, const cv::Mat &dst);


    //================ Qt image utilities ====================
    static QPixmap setOpacity(const QPixmap &pixmap, double opacity = .5);

};


#endif //KARYO_IMAGE_PROC_IMAGEUTILS_H
