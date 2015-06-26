#ifndef _TOP_H_
#define _TOP_H_

#include "hls_video.h"

// maximum image size
#define MAX_WIDTH  1024
#define MAX_HEIGHT 768

// I/O Image Settings
#define INPUT_IMAGE           "/home/inf2007/stpistop/Desktop/prj/src/images/input.jpg"
#define OUTPUT_IMAGE          "/home/inf2007/stpistop/Desktop/prj/src/images/output.jpg"
//#define OUTPUT_IMAGE_GOLDEN   "/home/inf2007/stpistop/Desktop/prj/src/images/output_golden.jpg"

// typedef video library core structures
typedef hls::stream<ap_axiu<34,1,1,1> >                 AXI_STREAM;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC3>     	RGB_IMAGE;


// top level function for HW synthesis
void image_filter(AXI_STREAM& input, AXI_STREAM& output, int rows, int cols);

#endif
