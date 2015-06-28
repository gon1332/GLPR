#include "hls_opencv.h"
#include "filter.h"

using namespace cv;
using namespace std;


void hls_filters(IplImage *_src, IplImage *_dst)
{
    AXI_STREAM src_axi, dst_axi;

    IplImage2AXIvideo(_src, src_axi);

    image_filter(src_axi, dst_axi, _src->height, _src->width);

    AXIvideo2IplImage(dst_axi,_dst);
}
