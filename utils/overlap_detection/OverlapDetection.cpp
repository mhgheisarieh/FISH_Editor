#include "OverlapDetection.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <numeric>
#include <opencv2/highgui.hpp>
#include <math.h>
using namespace cv;

OverlapDetection::OverlapDetection()
{

}



void findNearest(const vector<Point> &contour, const Point &p, int &nearestIndex, double &nearestDist,
                const vector<bool> &mask = vector<bool>())
{
    nearestDist = 1e100;
    nearestIndex = -1;
    for (size_t i = 0; i < contour.size(); i++)
    {
        if (mask.empty() || mask[i]) {
            double dist = norm(contour[i] - p);
            if (dist < nearestDist) {
                nearestIndex = i;
                nearestDist = dist;
            }
        }
    }

}

bool isValidAngle(double angle, double minAngle, double maxAngle) {
    if (minAngle < maxAngle && angle >= minAngle && angle <= maxAngle)
        return true;

    if (minAngle > maxAngle) {
        if ((angle >= minAngle && angle <= M_PI) || (angle >= -M_PI && angle < maxAngle))
            return true;
    }

    return false;
}

void findNearest(const vector<Point> &contour, const Point &p, int &nearestIndex, double &nearestDist, double &a,
                 double minAngle, double maxAngle)
{
    nearestDist = 1e100;
    nearestIndex = -1;
    for (size_t i = 0; i < contour.size(); i++)
    {
        Point delta = contour[i] - p;
        double angle = atan2(delta.y, delta.x);
        if (isValidAngle(angle, minAngle, maxAngle)) {
            double dist = norm(delta);
            if (dist < nearestDist) {
                nearestIndex = i;
                nearestDist = dist;
                a = angle;
            }
        }
    }

}

void createNeighborhoodMask(const vector<Point> &contour, int idx, double neighborhoodSize, vector<bool> &mask) {

    if (idx < 0)
        return;

    int after;
    double dist = 0;
    int i = idx;
    mask[i] = false;
    while (dist < neighborhoodSize) {
        after = (i + 1) % contour.size();
        dist += norm(contour[i] - contour[after]);
        i = after;
        mask[i] = false;
    }

    int before;
    dist = 0;
    i = idx;
    while (dist < neighborhoodSize) {
        before = i - 1;
        if (before < 0) before += contour.size();
        dist += norm(contour[i] - contour[before]);
        i = before;
        mask[i] = false;
    }
}


void orderCornerPoints(vector<CornerPoint> &corners, Point &center) {
    center = Point(0, 0);
    for (auto &corner : corners)
        center += corner.pos;

    center /= (int)corners.size();

    vector<double> angles(corners.size());
    for (size_t i = 0; i < corners.size(); i++) {
        Point diff = corners[i].pos - center;
        angles[i] = atan2(diff.y, diff.x);
    }

    vector<int> order(corners.size());
    iota(order.begin(), order.end(), 0);
    stable_sort(order.begin(), order.end(),
           [&angles](int i1, int i2) {return angles[i1] > angles[i2];});


    vector<CornerPoint> tempCorners;
    for (size_t i = 0; i < corners.size(); i++) {
        tempCorners.push_back(corners[order[i]]);
    }
    corners = tempCorners;
}


vector<CornerPoint> get4NearestPoint(const vector<Point> &contour, const Point &p) {
    vector<CornerPoint> corners(4);

    vector<bool> mask(contour.size(), true);
    for (size_t i = 0; i < corners.size(); i++) {
        if (i > 0) {
            double neighborhoodSize = 3.0 * corners[0].distFromCenter;
            createNeighborhoodMask(contour, corners[i - 1].index, neighborhoodSize, mask);
        }
        findNearest(contour, p, corners[i].index, corners[i].distFromCenter, mask);
        corners[i].pos = contour[corners[i].index];
    }

    return corners;
}

bool isCross(const vector<CornerPoint> &corners, const vector<vector<Point>> &contours, const Point &center) {

    for (size_t i = 0; i < corners.size(); i++) {
        CornerPoint c1 = corners[i];
        CornerPoint c2 = corners[(i + 1) % 4];
        Point p = (c1.pos + c2.pos) / 2;
        for (size_t j = 0; j < contours.size(); j++) {
            int pointTest = pointPolygonTest(contours[j], p, false); // +1: inside, -1: outside, 0: on edge
            if ((j == 0 && pointTest <= 0) || (j > 0 && pointTest >= 0))
                return false;
        }
        if (c1.contourIndex == c2.contourIndex) {
            double distThresh = 0.65 * norm(p - center);
            RotatedRect ellipse;
            vector<Point> contour;
            if (c1.index < c2.index)
                contour.insert(contour.end(), contours[c1.contourIndex].begin() + c1.index,
                        contours[c1.contourIndex].begin() + c2.index + 1);
            else {
                contour.insert(contour.end(), contours[c1.contourIndex].begin() + c1.index,
                        contours[c1.contourIndex].end());
                contour.insert(contour.end(), contours[c1.contourIndex].begin(),
                        contours[c1.contourIndex].begin() + c2.index + 1);
            }
            double width = 0;
            if (contour.size() >= 5) {
                ellipse = fitEllipse(contour);
                width = min(ellipse.size.width, ellipse.size.height);
            }
            if (width < distThresh)
                return false;
        }
    }
    return true;
}

vector<CornerPoint> OverlapDetection::findCornerPoints(const vector<vector<Point>> &contours, const Point &p)
{
    vector<CornerPoint> corners(4);

    for (size_t i = 0; i < contours.size(); i++) {
        vector<CornerPoint> tempCorners = get4NearestPoint(contours[i], p);
        for (size_t j = 0; j < tempCorners.size(); j++) {
            tempCorners[j].contourIndex = i;
            for (size_t k = 0; k < corners.size(); k++) {
                if (tempCorners[j].distFromCenter < corners[k].distFromCenter) {
                    corners.insert(corners.begin() + k, tempCorners[j]);
                    corners.pop_back();
                    break;
                }
            }
        }
    }

    Point center;
    orderCornerPoints(corners, center);

    if (!isCross(corners, contours, center))   // Point p is not a valid cross point
        corners.clear();

    return corners;
}

double getContourAngle(const vector<Point> &contour, int index) {
    const double neighborhoodSize = 10;
    int after;
    double dist = 0;
    int i = index;
    while (dist < neighborhoodSize) {
        after = (i + 1) % contour.size();
        dist += norm(contour[i] - contour[after]);
        i = after;
    }
    after = i;

    int before = -1;
    dist = 0;
    i = index;
    while (dist < neighborhoodSize) {
        before = i - 1;
        if (before < 0) before += contour.size();
        dist += norm(contour[i] - contour[before]);
        i = before;
    }
    before = i;

    Point p, p1, p2;
    p1 = contour[before];
    p = contour[index];
    p2 = contour[after];

    double angle = abs(atan2((p2 - p).y, (p2 - p).x) - atan2((p1 - p).y, (p1 - p).x));
    if (angle > M_PI) angle -= 2 * M_PI;
    return angle;
}

vector<CornerPoint> OverlapDetection::findCornerPoints2(const vector<vector<Point> > &contours, const Point &p)
{
    vector<CornerPoint> corners(4);

    for (size_t i = 0; i < contours.size(); i++) {
        double dist;
        int index;
        findNearest(contours[i], p, index, dist);
        if (dist < corners[0].distFromCenter) {
            corners[0].distFromCenter = dist;
            corners[0].index = index;
            corners[0].contourIndex = i;
            corners[0].pos = contours[i][index];
        }
    }

    Point delta = corners[0].pos - p;
    double interval0 = getContourAngle(contours[corners[0].contourIndex], corners[0].index);
    double interval1 = M_PI - interval0;
    vector<double> angles(4);
    angles[0] = atan2(delta.y, delta.x) - interval0 / 2;
    if (angles[0] < -M_PI) angles[0] += 2 * M_PI;

    for (size_t i = 1; i < angles.size(); i++) {
        double interval = i % 2 == 0 ? interval1 : interval0;
        double a = angles[i - 1] + interval;
        if (a > M_PI) a -= 2 * M_PI;
        angles[i] = a;
    }


    for (size_t i = 1; i < 4; i++) {
        for (size_t j = 0; j < contours.size(); j++) {
            double dist;
            int index;
            double angle;
            findNearest(contours[j], p, index, dist, angle, angles[i], angles[(i + 1) % 4]);
            if (dist < corners[i].distFromCenter) {
                corners[i].index = index;
                corners[i].distFromCenter = dist;
                corners[i].contourIndex = j;
                corners[i].pos = contours[j][index];
            }
        }
    }

    Point center;
    orderCornerPoints(corners, center);

    return corners;
}



int getCurrCorner(const vector<CornerPoint> &corners, int pointIndex, int contourIndex) {
    for (size_t i = 0; i < corners.size(); i++)
        if (corners[i].index == pointIndex && corners[i].contourIndex == contourIndex)
            return i;

    return -1;
}

bool isAllVisitedTwice(const vector<CornerPoint> &corners) {
    for (size_t i = 0; i < corners.size(); i++)
        if (corners[i].numVisited != 2)
            return false;

    return true;
}

bool isAllVisitedSingle(const vector<CornerPoint> &corners) {

    for (size_t i = 0; i < corners.size(); i++)
        if (corners[i].numVisited != 1)
            return false;

    return true;
}

bool isSomeCornersVisitedTwice(const vector<CornerPoint> &corners) {
    for (size_t i = 0; i < corners.size(); i++)
        if (corners[i].numVisited == 2)
            return true;

    return false;
}


enum ContourRelation {
    CHILD,
    PARENT,
    SEPARATED
};

bool areSeparated(const vector<Point> &contour1, const vector<Point> &contour2) {

    Rect r1 = boundingRect(contour1);
    Rect r2 = boundingRect(contour2);
    Rect r = r1 | r2;

    Mat image1(r.size(), CV_8UC1, Scalar::all(0));
    Mat image2(r.size(), CV_8UC1, Scalar::all(0));
    vector<vector<Point>> contours;
    contours.push_back(contour1);
    contours.push_back(contour2);
    drawContours(image1, contours, 0, Scalar::all(255), -1, LINE_8, noArray(), INT_MAX, -r.tl());
    drawContours(image2, contours, 1, Scalar::all(255), -1, LINE_8, noArray(), INT_MAX, -r.tl());
    Mat andImage;
    bitwise_and(image1, image2, andImage);
    if (countNonZero(andImage) == 0) // two contours don't have any intersection
        return true;

    Mat labels;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(image1, image1, kernel);
    erode(image2, image2, kernel);
    int prevNumComp1 = connectedComponents(image1, labels, 4) - 1;
    int prevNumComp2 = connectedComponents(image2, labels, 4) - 1;

    int numComp1 = connectedComponents(image1 - image2, labels, 4) - 1;
    int numComp2 = connectedComponents(image2 - image1, labels, 4) - 1;


    return numComp1 > prevNumComp1 && numComp2 > prevNumComp2;
}


// this function returns contour1 is CHILD/PARENT/SEPARATE of contour2
ContourRelation getRelation(const vector<Point> &contour1, const vector<Point> &contour2) {

    if (areSeparated(contour1, contour2))
        return SEPARATED;

    for (auto &point : contour2)
        if (pointPolygonTest(contour1, point, false) > 0)
            return PARENT;

    return CHILD;
}

void correctNestingOrder(vector<vector<vector<Point>>> &result) {
    if (result.size() > 1) {
        for (size_t i = 0; i < result.size(); i++) {
            for (size_t j = i + 1; j < result.size(); j++) {
                ContourRelation rel = getRelation(result[i][0], result[j][0]);
                if (rel == PARENT) {
                    result[i].insert(result[i].end(), result[j].begin(), result[j].end());
                    result.erase(result.begin() + j);
                    j--;
                }
                else if (rel == CHILD) {
                    result[j].insert(result[j].end(), result[i].begin(), result[i].end());
                    result.erase(result.begin() + i);
                    i--;
                    break;
                }
            }
        }
    }
}

void OverlapDetection::resolveOverlap(const vector<vector<Point>> &contours,
                      vector<CornerPoint> &corners,
                      vector<vector<vector<Point>>> &result) {

    result.clear();
    int currentContour = corners[0].contourIndex;
    int currentIndex = (corners[0].index + 1) % contours[0].size();

    int startContour = currentContour;
    int startIndex = currentIndex;
    result.resize(1);
    result.back().resize(1);
    bool jump = true;
    vector<bool> usedContours(contours.size(), false);
    while(true) {
        result.back().front().push_back(contours[currentContour][currentIndex]);

        if (!usedContours[currentContour])
            usedContours[currentContour] = true;

        int currCorner = getCurrCorner(corners, currentIndex, currentContour);
        if (jump && currCorner >= 0)
        {
            corners[currCorner].isLastTimeJumpedOut = true;
            corners[currCorner].numVisited++;
            currentIndex = corners[(currCorner + 1) % 4].index;
            currentContour = corners[(currCorner + 1) % 4].contourIndex;
            jump = false;
        }
        else {
            if (currCorner >= 0) {
                corners[currCorner].numVisited++;
                corners[currCorner].isLastTimeJumpedOut = false;
            }
            jump = true;
            currentIndex = (currentIndex + 1) % contours[currentContour].size();
        }

        if (currentContour == startContour && currentIndex == startIndex) {
            if (isAllVisitedTwice(corners))
                break;
            else {
                result.push_back(vector<vector<Point>>());
                result.back().resize(1);

                for (size_t i = 0; i < corners.size(); i++) {
                    if (corners[i].numVisited < 2 && corners[i].isLastTimeJumpedOut) {
                        currentContour = corners[i].contourIndex;
                        currentIndex = (corners[i].index + 1) % contours[currentContour].size();
                        startIndex = currentIndex;
                        startContour = currentContour;
                        jump = false;
                        break;
                    }
                }
            }
        }
    }

    // add unused contours to the result
    for (size_t i = 0; i < contours.size(); i++) {
        if (!usedContours[i]) {
            result.push_back(vector<vector<Point>>());
            result.back().push_back(contours[i]);
        }
    }

    // correct nesting order of result contours
    correctNestingOrder(result);
}

