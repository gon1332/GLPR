#include "hls_opencv.h"
#include "filter.h"

using namespace cv;
using namespace std;

extern void hls_filters(IplImage *_src, IplImage *_dst);

int main (int argc, char** argv)
{
	IplImage* src_rgb = cvLoadImage(INPUT_IMAGE, 1);
	IplImage* dst_rgb = cvCreateImage(cvGetSize(src_rgb), src_rgb->depth, src_rgb->nChannels);

    hls_filters(src_rgb, dst_rgb);

    cvSaveImage(OUTPUT_IMAGE, dst_rgb);

    return 0;
}
