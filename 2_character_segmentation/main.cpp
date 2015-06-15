#include <stdio.h>
#include <opencv2/opencv.hpp>

#define HEIGHT_FRACTION 0.70
#define WIDTH_FRACTION  0.50

using std::vector;

static bool comp_func(cv::Rect i, cv::Rect j)
{
    return i.tl().x < j.tl().x;
}

/*
 * RETURN VALUES:
 *  0 : SUCCESS
 *  1 : BAD ARGUMENTS
 *  2 : INVALID IMAGE DATA
 *
 * */
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.img>" << std::endl;
        return 1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], 1);

    if (!image.data)
    {
        printf("No image data \n");
        std::cerr << "No image data" << std::endl;
        return 2;
    }

    cv::Mat gray_image;
    cv::cvtColor( image, gray_image, CV_BGR2GRAY );

#ifdef DEBUG
    imshow("Gray", gray_image);
    cv::waitKey(0);
#endif

    ///////////////////////////////////////////////////////////////////////////
    // Apply a series of filters to enhance the text.
    ///////////////////////////////////////////////////////////////////////////

    cv::Mat blur_image;
    cv::medianBlur( gray_image, blur_image, 5 );

    cv::Mat thresh, color_image;
    cv::adaptiveThreshold(blur_image, thresh, 255, 1, 1, 11, 2);
    cv::cvtColor( thresh, color_image, CV_GRAY2BGR );

    dilate(thresh, thresh, 0);
    erode(thresh, thresh, 0);



    ///////////////////////////////////////////////////////////////////////////
    // Find contours in order to extract the Region Of Interest (ROI).
    ///////////////////////////////////////////////////////////////////////////
 
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(thresh, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<cv::Point> > contours_poly( contours.size() );
    vector<cv::Rect> boundRect( contours.size() );
    vector<cv::Point2f>center( contours.size() );
    vector<float>radius( contours.size() );

    cv::RNG rng(12345);
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
        cv::minEnclosingCircle( (cv::Mat)contours_poly[i], center[i], radius[i] );
    }

    // Isolate the contours that are most likely to refer to alphanumeric
    // characters.
    vector<vector<cv::Point> > final_contours;

#ifdef DEBUG
    std::cerr << "Num of Contours: " << contours.size() << std::endl;
#endif
    vector<vector<cv::Point> > final_contours_poly;
    vector<cv::Rect> final_boundRect;
    for (size_t i = 0; i < contours.size(); ++i) {
        if (boundRect[i].height > HEIGHT_FRACTION * color_image.size().height) {
            if (boundRect[i].width < color_image.size().width / 7) {
                final_contours.push_back(contours[i]);
                final_boundRect.push_back(boundRect[i]);
                final_contours_poly.push_back(contours_poly[i]);
            }
        }
    }
#ifdef DEBUG
    std::cerr << "Num of Contours: " << final_contours.size() << std::endl;
    std::cerr << "Num of bound: " << final_boundRect.size() << std::endl;
    std::cerr << "Num of poly: " << final_contours_poly.size() << std::endl;
#endif

    // Sort vector of Rectangles according to leftmost x-coordinate of the
    // contour.
    std::sort(final_boundRect.begin(), final_boundRect.end(), comp_func);
#ifdef DEBUG
    for (size_t i = 1; i < final_boundRect.size(); ++i)
    {
        std::cerr << " " << final_boundRect[i].tl().x;
    }
    std::cout << std::endl;
#endif


    /* TODO: gon1332 to 10 na ginei dynamika Δευ 15 Ιούν 2015 02:34:37 μμ EEST */
    // From the filtered contours, eliminate possible duplicate letters.
    for (size_t i = 1; i < final_boundRect.size(); ++i)
    {
        if (final_boundRect[i].tl().x - final_boundRect[i-1].tl().x < 10)
            final_boundRect.erase(final_boundRect.begin() + i);
    }
#ifdef DEBUG
    for (size_t i = 1; i < final_boundRect.size(); ++i) {
        std::cout << " " << final_boundRect[i].tl().x;
    }
    std::cout << std::endl;
#endif


    /// Draw polygonal contour + bonding rects + circles
    cv::Mat drawing = cv::Mat::zeros( color_image.size(), CV_8UC3 );
    for (size_t i = 0; i< final_contours.size(); ++i)
    {
        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        cv::drawContours( drawing, final_contours_poly, i, color, 1, 8, vector<cv::Vec4i>(), 0, cv::Point() );
        cv::rectangle( drawing, final_boundRect[i].tl(), final_boundRect[i].br(), color, 2, 8, 0 );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Character segmentation and write the output.
    ///////////////////////////////////////////////////////////////////////////
    char fname[13] = "letter00.png";
    for (size_t i = 0; i < final_boundRect.size(); ++i) {
        /// Crop the image
        // Get Region Of Interest ROI
        cv::Rect roi = final_boundRect[i];
        roi.height += 8;
        roi.x -= 4;
        roi.width += 8;
        roi.y -= 4;

        // Create a mask for each contour to mask out that region from image.
        cv::Mat mask = cv::Mat::zeros(gray_image.size(), CV_8UC1);
        cv::rectangle( mask, roi.tl(), roi.br(), cv::Scalar( cv::Scalar(255) ), CV_FILLED, 8, 0 );

        // Extract region using mask for region
        cv::Mat contourRegion;
        cv::Mat imageROI;
        image.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
        contourRegion = imageROI(roi);

        // Increase the number in the string.
        if (fname[7] == '9') { fname[6]++; fname[7] = '0'; }
        fname[7]++;

        cv::imwrite(fname, contourRegion);
    }

#ifdef DEBUG
    imwrite("final.png", drawing);
#endif

    return 0;
}
