#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#define HEIGHT_FRACTION 0.65
//#define DEBUG

using namespace std;

static char window_name[] = "Filter";

static void display_dst(cv::Mat dst, int delay)
{
#ifdef DEBUG
    cv::imshow(window_name, dst);
    cv::waitKey(delay);
#endif
}

/*
 * RETURN VALUES:
 *  0 : SUCCESS
 *  1 : BAD ARGUMENTS
 *  2 : INVALID IMAGE DATA
 *
 * */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " " << "<input.img>" << endl;
        return 1;
    }

    char *imageName = argv[1];

    cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);

    cv::Mat dst;
    cv::Mat src = cv::imread(imageName, 1);

    if (argc != 2 || !src.data) {
        cout << "Stage 1 - No image data." << endl;
        return 2;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Apply a series of filters to enhance the text.
    ///////////////////////////////////////////////////////////////////////////

    /// ORIGINAL
    dst = src.clone();
    display_dst(dst, 0);

    cv::Mat output;
    cv::medianBlur(dst, output, 3);

    /// GRAYSCALE
    cv::Mat img_gray;
    cv::cvtColor( src, img_gray, CV_BGR2GRAY );
    display_dst(img_gray, 0);

    /// HOMOGENUS BLUR
    blur(img_gray, img_gray, cv::Size(5, 5));
    display_dst(img_gray, 0);

    /// SOBEL
    cv::Mat img_sobel;
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    display_dst(img_sobel, 0);

    /// THRESHOLD
    cv::Mat img_threshold;
    cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU|CV_THRESH_BINARY);
    display_dst(img_threshold, 0);

    cv::Mat img_huehue = img_threshold.clone();

    /// MORPHOLOGY
    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
    display_dst(img_threshold, 0);


    ///////////////////////////////////////////////////////////////////////////
    // Find contours in order to extract the Region Of Interest (ROI).
    ///////////////////////////////////////////////////////////////////////////
    /// CONTOURS
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    findContours( img_threshold, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<cv::Point> > contours_poly( contours.size() );
    vector<cv::Rect> boundRect( contours.size() );
    vector<cv::Point2f>center( contours.size() );
    vector<float>radius( contours.size() );

    cv::RNG rng(12345);

    int largest_area = 0;
    int largest_contour_index = 0;
    for (size_t i = 0; i < contours.size(); ++i)
    {
        double a=contourArea( contours[i],false);  //  Find the area of contour
        if (a > largest_area) {
            largest_area=a;
            largest_contour_index=i;                //Store the index of largest contour
            approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( cv::Mat(contours_poly[i]) );
            minEnclosingCircle( (cv::Mat)contours_poly[i], center[i], radius[i] );
        }
    }


    cv::Mat drawing = cv::Mat::zeros( img_huehue.size(), CV_8UC3 );
    cv::Scalar color(255,255,255);
    cv::rectangle(drawing, boundRect[largest_contour_index].tl(), boundRect[largest_contour_index].br(), color, 2, 8, 0 );

    /// Crop the image
    // Get Region Of Interest ROI
    cv::Rect roi = boundRect[largest_contour_index];

    // Create a mask for each contour to mask out that region from image.
    cv::Mat mask = cv::Mat::zeros(img_huehue.size(), CV_8UC1);
    cv::drawContours(mask, contours, largest_contour_index, cv::Scalar(255), CV_FILLED);

    // Extract region using mask for region
    cv::Mat contourRegion;
    cv::Mat imageROI;
    img_huehue.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
    contourRegion = imageROI(roi);

#ifdef DEBUG
    display_dst(img_threshold, 0);
#endif


    ///////////////////////////////////////////////////////////////////////////
    // Write the final image.
    ///////////////////////////////////////////////////////////////////////////
    imwrite("output/cleanIm.jpg", contourRegion);


    return 0;
}

