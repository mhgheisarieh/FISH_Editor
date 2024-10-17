//
// Created by mostafa on 5/19/2019.
//

#include "ImageUtils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc.hpp>
#include <opencv2/stitching.hpp>
#include <limits.h>

using namespace cv;
using namespace std;

ImageUtils::ImageUtils() {

}

std::vector<Segment *> ImageUtils::segmentation(const Mat &image, int thresh) {
    vector<Segment *> segments;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    contoursCalc(image, thresh, contours, hierarchy);
    segments = ImageUtils::getSegments(image, contours, hierarchy);
    return segments;
}

std::vector<Segment *> ImageUtils::getSegments(const Mat &image, const std::vector<std::vector<cv::Point>> &contours,
                                             const std::vector<cv::Vec4i> &hierarchy) {
    vector<Segment *> segments;
    for (size_t i = 0; i < contours.size(); i++) {
        int child = hierarchy[i][2];
        int parent = hierarchy[i][3];
        if (parent == -1 && cv::contourArea(contours[i], false) > MIN_CONTOUR_AREA) { // this is an external contour
            vector<vector<Point>> segContours;
            segContours.push_back(contours[i]);

            while (child != -1) {// this component has some holes
                if (cv::contourArea(contours[child], false) > MIN_CONTOUR_AREA)
                    segContours.push_back(contours[child]);
                child = hierarchy[child][0];
            }

            Segment *segment = new Segment(image, segContours);
            segments.push_back(segment);
        }
    }
    return segments;
}

// this function counts the external contours
int ImageUtils::getNumSegments(const std::vector<std::vector<Point> > &contours, const std::vector<Vec4i> &hierarchy)
{
    int count = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        int parent = hierarchy[i][3];
        if (parent == -1)
            count++;
    }
    return count;
}

Rect ImageUtils::getBoundingBox(const vector<Point> &points, int maxAllowedX, int maxAllowedY, int radius) {
    int minX = INT_MAX, maxX = 0;
    int minY = INT_MAX, maxY = 0;
    for (auto & point : points) {
        minX = min(point.x - radius, minX);
        minY = min(point.y - radius, minY);
        maxX = max(point.x + radius, maxX);
        maxY = max(point.y + radius, maxY);
    }

    minX = minX < 0 ? 0 : minX;
    minY = minY < 0 ? 0 : minY;
    maxX = maxX > maxAllowedX ? maxAllowedX : maxX;
    maxY = maxY > maxAllowedY ? maxAllowedY : maxY;

//    minX = floor(minX);
//    minY = floor(minY);
//    maxX = ceil(maxX);
//    maxY = ceil(maxY);

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

Rect ImageUtils::unionBoundingBox(Rect bb1, Rect bb2) {
    int minX = min(bb1.x, bb2.x);
    int minY = min(bb1.y, bb2.y);
    int maxX = max(bb1.x + bb1.width, bb2.x + bb2.width);
    int maxY = max(bb1.y + bb1.height, bb2.y + bb2.height);

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

double ImageUtils::contourArea(const vector<Point> contour)
{
    return cv::contourArea(contour, false);
}

Segment *ImageUtils::join(const vector<Segment *> &segments, const vector<Point> &points, int radius, bool truncate) {

    //    if (truncate)
    //        truncatePolyline(segments, points);

    Rect pointsBoundingBox = getBoundingBox(points, segments[0]->getImage().cols, segments[0]->getImage().rows, radius);
    Rect bb = overallBoundingBox(segments);
    bb = unionBoundingBox(bb, pointsBoundingBox);
    Point2f offset(bb.x, bb.y);
    Point2f negativeOffset(-bb.x, -bb.y);


    Mat mask = Mat::zeros(bb.size(), CV_8UC1);
    if (points.size() == 1) {
        Point centerPoint(points[0].x - offset.x, points[0].y - offset.y);
        circle(mask, centerPoint, radius, Scalar(255), -1, LINE_8, 0);
    }
    else {
        for (size_t i = 0; i < points.size() - 1; i++) {
            Point p1(points[i].x - offset.x, points[i].y - offset.y);
            Point p2(points[i + 1].x - offset.x, points[i + 1].y - offset.y);
            line(mask, p1, p2, Scalar(255), 2 * radius, LINE_8, 0);
        }
    }
    for (auto & segment : segments) {
        Rect relativeBoundingBox(segment->getBoundingBox().x - bb.x, segment->getBoundingBox().y - bb.y,
                segment->getBoundingBox().width, segment->getBoundingBox().height);

        segment->getCroppedMask().copyTo(mask(relativeBoundingBox), segment->getCroppedMask());
    }

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, offset);
    Segment *newSegment = getSegments(segments[0]->getImage(), contours, hierarchy)[0];
    mask.release();

    return newSegment;
}

vector<Point> ImageUtils::truncatePolyline(const vector<Segment *> &segments, const vector<Point> &points) {
    int head = 0;
    for (size_t i = 0; i < points.size(); i++) {
        bool outside = true;
        for (size_t j = 0; j < segments.size(); j++) {
            if (segments[j]->pointTest(points[i]) >= 0) {
                outside = false;
                break;
            }
        }
        if (outside)
            head++;
        else
            break;
    }

    int tail = points.size();
    for (int i = points.size() - 1; i >= 0; i--) {
        bool outside = true;
        for (int j = 0; j < segments.size(); j++) {
            if (segments[j]->pointTest(points[i]) >= 0) {
                outside = false;
                break;
            }
        }
        if (outside)
            tail--;
        else
            break;
    }

    vector<Point> newPoints(points.begin() + head, points.begin() + tail);
    return newPoints;
}

Rect ImageUtils::overallBoundingBox(const vector<Segment *> &segments) {
    int minX = INT_MAX, maxX = 0;
    int minY = INT_MAX, maxY = 0;
    for (const auto & segment : segments) {
        Rect rect = segment->getBoundingBox();
        minX = rect.x < minX ? rect.x : minX;
        minY = rect.y < minY ? rect.y : minY;
        maxX = rect.x + rect.width > maxX ? rect.x + rect.width : maxX;
        maxY = rect.y + rect.height > maxY ? rect.y + rect.height : maxY;
    }
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void ImageUtils::contoursCalc(const Mat &image, int thresh,
                              vector<vector<Point> > &contours, vector<Vec4i> &hierarchy)
{
    Mat binaryImage(image.size(), CV_8UC1);
    cv::threshold(image, binaryImage, thresh, 255, THRESH_BINARY_INV);
    Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3));
    morphologyEx(binaryImage, binaryImage, MORPH_CLOSE, kernel);
    findContours(binaryImage, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    binaryImage.release();
}

void ImageUtils::skeleton(const Mat &src, Mat &dst)
{
    using namespace ximgproc;
    thinning(src, dst);
}




void ImageUtils::gammaAdjust(const Mat &src, Mat &dst, double gamma, double lowThresh, double highThresh)
{
    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i) {
        double pixelValue = i / 255.0;
        pixelValue = max(lowThresh, min(highThresh, pixelValue));
        pixelValue = (pixelValue - lowThresh) / (highThresh - lowThresh);
        p[i] = saturate_cast<uchar>(pow(pixelValue, gamma) * 255.0);
    }

    LUT(src, lookUpTable, dst);
    lookUpTable.release();
}

void ImageUtils::equalizeHistogram(const Mat &src, Mat &dst, int thresh)
{
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange = { range };
    int histSize = 256;
    bool uniform = true, accumulate = false;
    Mat histogram;
    calcHist(&src, 1, nullptr, Mat(), histogram, 1, &histSize, &histRange, uniform, accumulate);

    for (int i = thresh + 1; i < histSize; i++) {
            histogram.at<float>(i) = 0;
    }

    normalize(histogram, histogram, 255, 0, NORM_L1);

    float sum = 0;
    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for (int i = 0; i < histSize; i++) {
        sum += histogram.at<float>(i);
        p[i] = saturate_cast<uchar>(sum);
    }

    LUT(src, lookUpTable, dst);
    lookUpTable.release();
    histogram.release();
}

void ImageUtils::sharpen(const Mat &src, Mat &dst, int radius, double amount)
{
    Mat bluredImage;
    GaussianBlur(src, bluredImage, Size(radius, radius), 15, 15);
    addWeighted(src, 1 + amount, bluredImage, -amount, 0, dst);
    bluredImage.release();
}
#include <QPainter>
#include <iostream>
using namespace std;
void ImageUtils::threshold(const Mat &src, Mat &dst, int highThresh, int lowThresh)
{
    Mat mask;
    // set pixels above highThresh to 255
    cv::threshold(src, mask, highThresh, 255, THRESH_BINARY);
    cv::bitwise_or(src, mask, dst);

    // set pixels below lowThresh to 255
    if (lowThresh > 0)
        cv::threshold(mask, mask, lowThresh, 255, THRESH_BINARY);
//    cv::bitwise_or(dst, mask, dst);
    mask.release();
}

std::vector<float> ImageUtils::histCalc(const Mat &image, int histSize)
{
    int minY = 0;
    int maxY = 1;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange = { range };
    bool uniform = true, accumulate = false;
    Mat histogram;
    calcHist(&image, 1, 0, Mat(), histogram, 1, &histSize, &histRange, uniform, accumulate);
    normalize(histogram, histogram, minY, maxY, NORM_MINMAX);

    vector<float> result(histSize);

    for (int i = 0; i < histSize; i++) {
        result[i] = histogram.at<float>(i);
    }

    histogram.release();
    return result;
}

void ImageUtils::cropByPolygon(const Mat &src, Mat &dst, const QPolygonF &polygon)
{
    vector<Point> poly(polygon.size());
    for (int i = 0; i < polygon.size(); i++) {
        QPoint point = polygon[i].toPoint();
        poly[i] = Point(point.x(), point.y());
    }
    vector<vector<Point>> polygons;
    polygons.push_back(poly);

    Mat mask(src.size(), CV_8U, Scalar(0));
    Mat maskNot(src.size(), CV_8U);

    cv::fillPoly(mask, polygons, Scalar(255));
    bitwise_not(mask, maskNot);

    QRect r = polygon.boundingRect().toRect();
    Rect rect(r.x(), r.y(), r.width(), r.height());
    rect.x = rect.x < 0 ? 0 : rect.x;
    rect.y = rect.y < 0 ? 0 : rect.y;
    rect.width = rect.x + rect.width > src.cols ? src.cols - rect.x : rect.width;
    rect.height = rect.y + rect.height > src.rows ? src.rows - rect.y : rect.height;

    dst.create(rect.size(), CV_8U);

    bitwise_and(src(rect), mask(rect), dst);
    bitwise_or(dst, maskNot(rect), dst); // fill outer region of the polygon with white color.

    mask.release();
    maskNot.release();

}

void ImageUtils::removeBackground(const Mat &src, Mat &dst)
{
    int histSize = 50;
    double binRange = 256. / histSize;
    vector<float> histogram = ImageUtils::histCalc(src, histSize);

    Point maxLocPoint;
    minMaxLoc(histogram, nullptr, nullptr, nullptr, &maxLocPoint);
    int maxLoc = qRound(maxLocPoint.x * binRange);
    const int PEAK_HIST_DISTANCE = 15;

    if (maxLoc < 250 && maxLoc - PEAK_HIST_DISTANCE > 0)
        threshold(src, dst, maxLoc - PEAK_HIST_DISTANCE);
    else
        src.copyTo(dst);

}

void ImageUtils::smooth(const Mat &src, Mat &dst, int kSize)
{
    medianBlur(src, dst, kSize);
}

void ImageUtils::contrastBrightnessAdjust(const Mat &src, Mat &dst, double contrast, double brightness)
{
    addWeighted(src, contrast, src, 0, brightness, dst);
}


Ptr<Stitcher> ImageUtils::stitcher = Stitcher::create(Stitcher::SCANS);

void ImageUtils::stitch(const vector<Mat> &images, Mat &stitchedImage)
{
    Stitcher::Status status = stitcher->stitch(images, stitchedImage);

    if (status != Stitcher::OK) {
        cout << "Can't stitch images, error code = " << int(status) << endl;
    }
}

QImage ImageUtils::stitch(const QVector<QImage> &images)
{
    vector<Mat> matImages;
    matImages.reserve(images.size());
    for(auto &image : images) {
        QImage clonedImage = image.convertToFormat(QImage::Format_RGB888);
        Mat mat = Mat(clonedImage.height(), clonedImage.width(), CV_8UC3,
                clonedImage.bits(), clonedImage.bytesPerLine()).clone();
        cvtColor(mat, mat, COLOR_RGB2BGR);
        matImages.push_back(mat);
    }

    Mat dst;
    stitch(matImages, dst);

    QImage result(dst.data, dst.cols, dst.rows, static_cast<int>(dst.step), QImage::Format_RGB888);

    return result.copy();  // copy is needed because after terminating the function, data pointer of dst image is no longer valid.

}

int ImageUtils::opencvType(QImage::Format format)
{
    switch (format) {
    case QImage::Format_Grayscale8:
        return CV_8UC1;
    case QImage::Format_RGB888:
        return CV_8UC3;
    case QImage::Format_RGBA8888:
    case QImage::Format_RGB32:
        return CV_8UC4;
    default:
        return CV_8UC1;
    }
}

void ImageUtils::imageNot(const Mat &src, const Mat &dst)
{
    bitwise_not(src, dst);
}

QPixmap ImageUtils::setOpacity(const QPixmap &pixmap, double opacity)
{
    QImage image(pixmap.size(), QImage::Format_ARGB32_Premultiplied); //Image with given size and format.
    image.fill(Qt::transparent); //fills with transparent
    QPainter p(&image);
    p.setOpacity(opacity); // set opacity from 0.0 to 1.0, where 0.0 is fully transparent and 1.0 is fully opaque.
    p.drawPixmap(0, 0, pixmap); // given pixmap into the paint device.
    p.end();
    return QPixmap::fromImage(image);
}


