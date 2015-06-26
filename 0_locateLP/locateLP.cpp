#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#define WINDOWNAME "Edge Map"
#define FILENAME "polyContour"

using namespace std;
using namespace cv;

/// Global variables
Mat src, src_gray;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 250;
int ratio = 3;
int kernel_size = 3;
RNG rng(12345);

// Helper function:
// Finds a cosine of angle between vectors
// From pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 ) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;

	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// Comparator for rectangle X coordinates (as Vectors)
static bool lessThanPair(vector<Point> init, vector<Point> other) {
	return boundingRect(init).tl().x < boundingRect(other).tl().x;
}

// Comparator for rectangle X coordinates (as Rect)
static bool lessThanRect(Rect l, Rect r) {
    if(l.tl().x == r.tl().x) return (l.tl().y < r.tl().y);
    return (l.tl().x < r.tl().x);
}

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*) {
	vector<vector<Point> > contours;
	vector<Point> polyContours;
	vector<Vec4i> hierarchy;

	Mat outputImage(src_gray.size().height, src_gray.size().width * 2, CV_8UC3, cv::Scalar(0,0,0));
	Mat outputCanny(src_gray.size().height, src_gray.size().width * 2, CV_BGR2GRAY, cv::Scalar(0,0,0));
	Mat detected_edges;

	blur(src_gray, detected_edges, Size(3,3));												/// Reduce noise with a kernel 3x3
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);	/// Canny detector - detected_edges is a mask filter!
	src.copyTo(outputCanny, detected_edges);												/// Using Canny's output as a mask, we display our result

	Mat targetROI = outputImage(Rect(0, 0, outputCanny.size().width, outputCanny.size().height));
	outputCanny.copyTo(targetROI);

	/// Finding Contours
	findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
// #define BLANKCANVAS
#ifdef BLANKCANVAS
	Mat drawing = Mat::zeros(detected_edges.size(), CV_8UC3);
#else
	Mat drawing;
	src.copyTo(drawing);
#endif

#ifdef DEBUG
	printf("Number of Contours: %d\n", contours.size());
#endif

#define ROTATEDRECT
#ifdef DRAWALLCONTOURS
	detected_edges.copyTo(drawing);
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
	drawContours(drawing, contours, -1, color, 2, 8, hierarchy, 0, Point());
#endif

#ifdef ROTATEDRECT
	vector<RotatedRect> minRect;
 	Point2f rect_points[4];

	for (int i = 0; i < contours.size(); i++)
		minRect.push_back(minAreaRect(Mat(contours[i])));

#ifdef DEBUG
	// Prints a rotatedRect vector
	for (int i = 0; i < minRect.size(); i++) {
		minRect[i].points(rect_points);

		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
		for (int j = 0; j < 4; j++)
			line(drawing, rect_points[j], rect_points[(j+1)%4], color, 3, 8);
	}
#endif

	/// Remove semi-rotated rectangles
#ifdef DEBUG
 	printf("Rectangles: %d", minRect.size());
#endif
	// Greece-1 is locked both with < and >
	// Plate01 locked with >, lost with <
	// Plate02 is okay with >, lost with <
	// Plate03 is somewhat located with >, SegDumps with <
	// Plate04 locked with <, lost with >
	// Plate05 lost here
	// Plate06 locked with <, lost with >
	// Plate07 locked with <, lost with >
	// Plate08 locked both with > and <
	// Plate09 locked with >, assertion failed with <
	
	for (int i = 0; i < minRect.size(); i++)
		if (minRect[i].angle > (-90 + 15)) {	// Angles are biased to [-90, 0)
			minRect.erase(minRect.begin() + i);
			i--;
		}
	// Prints a rotatedRect vector
	for (int i = 0; i < minRect.size(); i++) {
		minRect[i].points(rect_points);

		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
		for (int j = 0; j < 4; j++)
			line(drawing, rect_points[j], rect_points[(j+1)%4], color, 3, 8);
	}
#ifdef DEBUG
 	printf(" vs %d\n", minRect.size());
#endif

	double sizeOne, sizeTwo, rectRatio;
	double minRectArea = (src_gray.size().height * src_gray.size().width) / 100;

	for (int i = 0; i < minRect.size(); i++) {
		minRect[i].points(rect_points);
		sizeOne = norm(rect_points[0] - rect_points[1]);
		sizeTwo = norm(rect_points[1] - rect_points[2]);
		ratio = max(sizeOne,sizeTwo) / min(sizeOne, sizeTwo);
// 		printf("%lf, ", max(sizeOne,sizeTwo) / min(sizeOne, sizeTwo));
		if (ratio < 2.5 || ratio > 7) {	// We expect a licence plate to have speciefic dimensions
			minRect.erase(minRect.begin() + i);
			i--;
			continue;
		}
		if ((sizeOne * sizeTwo) < minRectArea) { // If it is too small, then don't consider it
			minRect.erase(minRect.begin() + i);
			i--;
		}
		// Could use vertical and horizontal histograms AFTER rotating
		// the rotatedRect to a normal rectangle, but then again ...
		// http://docs.opencv.org/modules/imgproc/doc/histograms.html?highlight=hist#cv.CalcHist
	}
// 	printf("\n");

	vector<Rect> finalRectangles;
	for (int i = 0; i < minRect.size(); i++) {
		finalRectangles.push_back(minRect[i].boundingRect());
	}

	// Duplicate Removal
#ifdef DEBUG
	for (int i = 0; i < finalRectangles.size(); i++) {
		printf("%d: %d.%d\n", i, finalRectangles[i].tl().x, finalRectangles[i].tl().y);
	}
#endif

	sort(finalRectangles.begin(), finalRectangles.end(), lessThanRect);

	// Clear Duplicates on X-axis
	for (int i = 1; i < finalRectangles.size(); i++) {
#ifdef DEBUG
		printf("%d: %d.%d vs ", i, finalRectangles[i].tl().x, finalRectangles[i].tl().y);
		printf("%d: %d.%d: ", i-1, finalRectangles[i-1].tl().x, finalRectangles[i-1].tl().y);
		printf("%lf ", finalRectangles[i].tl().x / finalRectangles[i-1].tl().x);
#endif
		if ((finalRectangles[i].tl().x / finalRectangles[i-1].tl().x) > 0.9) {
			finalRectangles.erase(finalRectangles.begin() + i-1);
#ifdef DEBUG
			printf("Success!");
#endif
		}
#ifdef DEBUG
		printf("\n");
#endif
	}

	// Clear Duplicates on Y-axis
	for (int i = 1; i < finalRectangles.size(); i++) {
#ifdef DEBUG
		printf("%d: %d.%d vs ", i, finalRectangles[i].tl().x, finalRectangles[i].tl().y);
		printf("%d: %d.%d: ", i-1, finalRectangles[i-1].tl().x, finalRectangles[i-1].tl().y);
		printf("%lf ", finalRectangles[i].tl().x / finalRectangles[i-1].tl().x);
#endif
		if ((finalRectangles[i].tl().y / finalRectangles[i-1].tl().y) > 0.9) {
			finalRectangles.erase(finalRectangles.begin() + i-1);
#ifdef DEBUG
			printf("Success!");
#endif
		}
#ifdef DEBUG
		printf("\n");
#endif
	}

#ifdef DEBUG
	for (int i = 0; i < finalRectangles.size(); i++) {
		printf("%d: %d.%d\n", i, finalRectangles[i].tl().x, finalRectangles[i].tl().y);
	}
#endif

	system("rm -f "FILENAME"*.png");																					/// Clear Directory

	//Commence Print
	for (int i = 0; i < finalRectangles.size(); i++) {
		// Extract region using mask for region
		Mat contourRegion;
		src_gray(finalRectangles[i]).copyTo(contourRegion);

		string name = FILENAME;
		stringstream filenameStream;
		filenameStream << name << (i) << ".png";
		string filename = filenameStream.str();
		filenameStream.str("");

		imwrite(filename, contourRegion);

		// Added Bonus: Print the rect
		rectangle(drawing, finalRectangles[i].tl(), finalRectangles[i].br(), Scalar(60,60,255), 2, 8, 0);
	}
#endif
#ifdef CORRECTCODE
	/// Find Quadrilaterals
	vector<vector<Point> > finalVector;
	string name = FILENAME;
	int contourCount = 0;

	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(contours[i], polyContours, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		// Skip small or non-convex objects 
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(Mat(polyContours)))
			continue;

		if (polyContours.size() == 4 && fabs(contourArea(Mat(polyContours))) > 1000 && isContourConvex(Mat(polyContours))) {
			double maxCosine = 0;

#ifdef DEBUG
			printf("Contour #%d: ", contourCount++);
#endif
			for(int j = 2; j < (4+1); j++) {
				double cosine = fabs(angle(polyContours[j%4], polyContours[j-2], polyContours[j-1]));
#ifdef DEBUG
				printf("%lf, ", cosine);
#endif
				maxCosine = MAX(maxCosine, cosine);
			}
#ifdef DEBUG
			printf("\n");
#endif

			if(maxCosine > 0.3)
				continue;

// 			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
// 			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());

			finalVector.push_back(contours[i]);
		}
	}

	// Duplicate Removal
#ifdef DEBUG
	for (int i = 0; i < finalVector.size(); i++) {
		printf("%d: %d.%d\n", i, boundingRect(finalVector[i]).tl().x, boundingRect(finalVector[i]).tl().y);
	}
#endif

	sort(finalVector.begin(), finalVector.end(), lessThanPair);

	// Clear Duplicates on X-axis
	for (int i = 1; i < finalVector.size(); i++) {
#ifdef DEBUG
		printf("%d: %d.%d vs ", i, boundingRect(finalVector[i]).tl().x, boundingRect(finalVector[i]).tl().y);
		printf("%d: %d.%d: ", i-1, boundingRect(finalVector[i-1]).tl().x, boundingRect(finalVector[i-1]).tl().y);
		printf("%lf ", boundingRect(finalVector[i]).tl().x / boundingRect(finalVector[i-1]).tl().x);
#endif
		if ((boundingRect(finalVector[i]).tl().x / boundingRect(finalVector[i-1]).tl().x) > 0.9) {
			finalVector.erase(finalVector.begin() + i-1);
#ifdef DEBUG
			printf("Success!");
#endif
		}
#ifdef DEBUG
		printf("\n");
#endif
	}

	// Clear Duplicates on Y-axis
	for (int i = 1; i < finalVector.size(); i++) {
#ifdef DEBUG
		printf("%d: %d.%d vs ", i, boundingRect(finalVector[i]).tl().x, boundingRect(finalVector[i]).tl().y);
		printf("%d: %d.%d: ", i-1, boundingRect(finalVector[i-1]).tl().x, boundingRect(finalVector[i-1]).tl().y);
		printf("%lf ", boundingRect(finalVector[i]).tl().x / boundingRect(finalVector[i-1]).tl().x);
#endif
		if ((boundingRect(finalVector[i]).tl().y / boundingRect(finalVector[i-1]).tl().y) > 0.9) {
			finalVector.erase(finalVector.begin() + i-1);
#ifdef DEBUG
			printf("Success!");
#endif
		}
#ifdef DEBUG
		printf("\n");
#endif
	}

#ifdef DEBUG
	for (int i = 0; i < finalVector.size(); i++) {
		printf("%d: %d.%d\n", i, boundingRect(finalVector[i]).tl().x, boundingRect(finalVector[i]).tl().y);
	}
#endif

	system("rm -f "FILENAME"*.png");																					/// Clear Directory

	//Commence Print
	for (int i = 0; i < finalVector.size(); i++) {
		// Create a mask for each contour to mask out that region from image.
		Mat mask = Mat::zeros(src.size(), CV_8UC1);
		drawContours(mask, finalVector, i, Scalar(255), CV_FILLED);

		// Extract region using mask for region
		Mat contourRegion;
		Mat imageROI;
		src_gray.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
		contourRegion = imageROI(boundingRect(finalVector[i]));

		stringstream filenameStream;
		filenameStream << name << (i) << ".png";
		string filename = filenameStream.str();
		filenameStream.str("");

		imwrite(filename, contourRegion);

		// Added Bonus: Print the contour
		drawContours(drawing, finalVector, i, Scalar(60,60,255), 2, 8, hierarchy, 0, Point());
	}
#endif

#ifdef WINDOWDEBUG
	targetROI = outputImage(Rect(outputCanny.size().width, 0, outputCanny.size().width, outputCanny.size().height));	/// Change Reference to the right image
	drawing.copyTo(targetROI);

	imshow(WINDOWNAME, outputImage);
	moveWindow(WINDOWNAME, 0, 0);
#endif
}


/** @function main */
int main( int argc, char** argv ) {
	/// Load an image
	src = imread( argv[1] );

	if( !src.data ) {
		printf("Input file? No?\n");
		return -1;
	}

	/// Convert the image to grayscale
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3,3));

	namedWindow(WINDOWNAME, CV_WINDOW_AUTOSIZE);													/// Create a window
	createTrackbar("Min Threshold:", WINDOWNAME, &lowThreshold, max_lowThreshold, CannyThreshold);	/// Create a Trackbar for user to enter threshold
	CannyThreshold(0, 0);																			/// Show the image

	waitKey(0);																						/// Wait until user exit program by pressing a key

	return 0;
}
