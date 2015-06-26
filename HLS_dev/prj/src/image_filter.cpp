#include "filter.h"

void image_filter(AXI_STREAM& input, AXI_STREAM& output, int rows, int cols)
{
    //Create AXI streaming interfaces for the core
#pragma HLS RESOURCE variable=input core=AXIS metadata="-bus_bundle INPUT_STREAM"
#pragma HLS RESOURCE variable=output core=AXIS metadata="-bus_bundle OUTPUT_STREAM"
#pragma HLS RESOURCE core=AXI_SLAVE variable=rows metadata="-bus_bundle CONTROL_BUS"
#pragma HLS RESOURCE core=AXI_SLAVE variable=cols metadata="-bus_bundle CONTROL_BUS"
#pragma HLS RESOURCE core=AXI_SLAVE variable=return metadata="-bus_bundle CONTROL_BUS"

#pragma HLS INTERFACE ap_stable port=rows
#pragma HLS INTERFACE ap_stable port=cols

    RGB_IMAGE img(rows, cols);
    RGB_IMAGE img1(rows, cols);
    RGB_IMAGE img2(rows, cols);
    RGB_IMAGE gray(rows, cols);
    RGB_IMAGE blured(rows, cols);



#pragma HLS dataflow
    // AXI to RGB_IMAGE stream
    hls::AXIvideo2Mat(input, img);
    hls::Duplicate(img, img1, img2);
    // Grayscaling
    //hls::CvtColor<HLS_RGB2GRAY>(img1, gray);
	//hls::GaussianBlur<5,5>(gray, blured, 1.4, 1.4 );

    hls::Mat2AXIvideo(img1 /*blured*/, output);
}
