#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <stdio.h>

using namespace std;
using namespace cv;



using namespace cv;



int main( int argc, char** argv )
{
  
	Mat image;

	image = imread(argv[1]);

	if (!image.data) {
        cout << "Stage2 - No image data." << endl;
        return 2;
    }


    Mat image_gray;

	cv::Size s = image.size();
	int rows = s.height;
	int cols = s.width;

    Mat ColSum(Size(1,cols),CV_32SC1);//(1, cols, CV_64FC1);
    Mat revMat(Size(1,cols),CV_32SC1);
	//std::cout << typeid((ColSum.ptr<uchar>(4)).name() << '\n';

    cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);

    uchar black(0);

    for(int y = 0; y < image_gray.rows; y += image_gray.rows-1)
	{
	    uchar* row2 = image_gray.ptr<uchar>(y);
	    for(int x = 0; x < image_gray.cols; ++x)
	    {
	        if(row2[x] == black)
	        {   
	            cv::floodFill(image_gray, cv::Point(x,y), cv::Scalar(255), (cv::Rect*)0, cv::Scalar(), cv::Scalar(200));
	        }
	    }
	}

	// fix left and right sides
	for(int y = 0; y < image_gray.rows; ++y)
	{
	    uchar* row2 = image_gray.ptr<uchar>(y);
	    for(int x = 0; x < image_gray.cols; x += image_gray.cols - 1)
	    {
	        if(row2[x] == black)
	        {   
	            cv::floodFill(image_gray, cv::Point(x,y), cv::Scalar(255), (cv::Rect*)0, cv::Scalar(), cv::Scalar(200));
	        }
	    }
	 }

	// imwrite("output/pic1.jpg",image_gray);

	cv::reduce(image_gray, ColSum, 0, CV_REDUCE_SUM, CV_32SC1);

	//std::cout << ColSum << '\n';

	unsigned int max_out=0, max_invOut=0;
	MatIterator_<unsigned int> it = ColSum.begin<unsigned int>(), end = ColSum.end<unsigned int>(), revIt, revEnd;

	max_out = *std::max_element(it, end);

	//printf("max = %u\n\n", max_out);

	revMat = ColSum.clone();

	for(revIt = revMat.begin<unsigned int>(), revEnd = revMat.end<unsigned int>(); revIt!=revEnd; ++revIt)
	{
		*revIt = max_out - *revIt;
		//printf("%u, ", *revIt);
	}

	//std::cout << revMat << '\n';
	max_invOut = *std::max_element(revMat.begin<unsigned int>(), revMat.end<unsigned int>());
	//printf("max = %u\n\n", max_invOut);


	// Normalise the inverted histogram
	unsigned int mat[cols];
	int i=0;
	for(revIt = revMat.begin<unsigned int>(), revEnd = revMat.end<unsigned int>(); revIt!=revEnd; ++revIt){
		if(*revIt<max_out/10)
			*revIt=0;
		mat[i] = *revIt;
		i++;
	}

	//std::cout << revMat << '\n';


	int letter_start=0, letter_end=0;
	int imgID=1;


	for(i=0; i<cols-1; i++){
		if(mat[i]==0 && mat[i+1]>0){
			letter_start=i+1;
		}
		if (mat[i]>0 && mat[i+1]==0)
		{
			letter_end=i;
		}
		
		if(letter_start>0 && letter_end>0){
			printf("Pixel Start - Pixel End: %u - %u\n", letter_start, letter_end);

			
			
			cv::Rect roi(cv::Point2i(letter_start, rows), cv::Point2i(letter_end, 0));

			//Create a mask for each contour to mask out that region from image.
	        cv::Mat mask = cv::Mat::zeros(image_gray.size(), CV_8UC1);
	        cv::rectangle( mask, roi.tl(), roi.br(), cv::Scalar( cv::Scalar(255) ), CV_FILLED, 8, 0 );

	        // Extract region using mask for region
	        cv::Mat croppedImage;
	        cv::Mat imageROI;
	        image.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
	        croppedImage = imageROI(roi);

			imwrite("output/letter"+std::to_string(imgID)+".jpg", croppedImage);
			imgID++;
			letter_start=0;
			letter_end=0;
		}
	}

  	return 0;
}