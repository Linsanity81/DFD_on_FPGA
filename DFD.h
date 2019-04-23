#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#define MAX_WIDTH  640
#define MAX_HEIGHT 480

#include  "hls_video.h"
#include <ap_fixed.h>

typedef hls::stream<ap_axiu<32,1,1,1> >           AXI_STREAM;//

typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_32FC1> GRAY_IMAGE;
typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_32FC1> GRAY_IMAGE_32;
typedef hls::Scalar<1, float>  GRAY_PIXEL;
typedef hls::Scalar<1, float>  GRAY_PIXEL_32;

typedef ap_fixed<32,24,AP_RND> COEF_T;
typedef ap_int<16>           INDEX_T;

// kernel size
const int KS = 11;
const int size = 9;
//#define INPUT_IMAGE          "new_sponge_n.bmp"
//#define OUTPUT_IMAGE         "D:\\Research_Project\\Shape_Reconstruction_on_FPGA\\DFD\\DFD\\output.bmp"


void image_filter(
	AXI_STREAM& INPUT_STREAM,
	AXI_STREAM& OUTPUT_STREAM );


template<typename IMG_T, typename PIXEL_T>
void add(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_in2, IMG_T& img_in3, IMG_T& img_out, int rows, int cols, float (&max)[MAX_HEIGHT/8][MAX_WIDTH/8]);

template<typename IMG_T>
void average(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

template<typename IMG_T, typename PIXEL_T>
void sigma(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_in2, IMG_T& img_in3, IMG_T& img_in4,IMG_T& img_out, int rows, int cols);

template<typename IMG_T, typename PIXEL_T>
void max_filter(IMG_T& img_in0, IMG_T& img_out, int rows, int cols, float (&max)[MAX_HEIGHT/8][MAX_WIDTH/8]);

template<typename IMG_T>
void Gassian_5(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

template<typename IMG_T>
void Gassian_10(IMG_T& img_in, IMG_T& img_out, int rows, int cols);
#endif
