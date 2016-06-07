/**
 * @file Morphology_1.cpp
 * @brief Erosion and Dilation sample code
 * @author OpenCV team
 */

#include <opencv2/opencv.hpp>
//#include "opencv2/imgcodecs.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables
Mat src, erosion_dst, dilation_dst;

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;

/** Function Headers */
//void Erosion( int, void* );
Mat  *Dilation( int, void* );


int main( int, char** argv )
{
  /// Load an image
  src = imread( argv[1] );

  if( src.empty() )
    return -1;

  

  //medianBlur(src, src, 5);

  //cvtColor( src, src, CV_BGR2GRAY );

  //adaptiveThreshold(src, src, 255, 1, 1, 11, 2);


  //for (int i = 1; i < 6; ++i)
  //{
    /* code */
    int dilation_size = 3;
    /// Default start
    Mat element = getStructuringElement( 0, Size( 2*dilation_size + 1, 2*dilation_size+1 ), Point( dilation_size, dilation_size ) );

    dilate(src, dilation_dst, element );
    // imshow( "Dilation Image", dilation_dst);
    // waitKey();
    
  //}

    int erosion_size=3;
    
    Mat element2 = getStructuringElement( 0, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );

    erode( dilation_dst, erosion_dst, element2 );
    // imshow("Erosion Image", erosion_dst);
    // waitKey();
    imwrite("output/pic1.jpg", erosion_dst);


  return 0;
}
