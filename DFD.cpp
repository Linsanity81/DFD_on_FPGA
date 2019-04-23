#include "DFD.h"
#include "math.h"

//______________________________________________________________________________

void image_filter(AXI_STREAM& INPUT_STREAM, AXI_STREAM& OUTPUT_STREAM) {

#pragma HLS INTERFACE axis  port=INPUT_STREAM bundle=VIDEO_IN
#pragma HLS INTERFACE axis  port=OUTPUT_STREAM bundle=VIDEO_OUT
//#pragma HLS INTERFACE s_axilite port=return   bundle=CONTROL_BUS

//#pragma HLS INTERFACE s_axilite port=rows     bundle=CONTROL_BUS //offset=0x14
//#pragma HLS INTERFACE s_axilite port=cols     bundle=CONTROL_BUS //offset=0x1C
//#pragma HLS INTERFACE ap_stable port=rows
//#pragma HLS INTERFACE ap_stable port=cols

#pragma HLS DATAFLOW

//assert(rows <= MAX_HEIGHT);
//assert(cols <= MAX_WIDTH);
const int rows = MAX_HEIGHT;
const int cols = MAX_WIDTH;

GRAY_IMAGE  img_0  (rows, cols);

GRAY_IMAGE  img_1  (rows, cols);
GRAY_IMAGE  img_1a (rows, cols);
GRAY_IMAGE  img_1b (rows, cols);
GRAY_IMAGE  img_1c (rows, cols);

GRAY_IMAGE  img_2a (rows, cols);
GRAY_IMAGE  img_2b (rows, cols);
GRAY_IMAGE  img_2a1 (rows, cols);
GRAY_IMAGE  img_2b1 (rows, cols);
GRAY_IMAGE  img_2aa (rows, cols);
GRAY_IMAGE  img_2ab (rows, cols);
GRAY_IMAGE  img_2ba (rows, cols);
GRAY_IMAGE  img_2bb (rows, cols);

GRAY_IMAGE  img_3a  (rows, cols);
GRAY_IMAGE  img_3b  (rows, cols);
GRAY_IMAGE  img_3c  (rows, cols);

GRAY_IMAGE  img_4a  (rows, cols);
GRAY_IMAGE  img_4b1  (rows, cols);
GRAY_IMAGE  img_4c1  (rows, cols);
GRAY_IMAGE  img_4b11  (rows, cols);
GRAY_IMAGE  img_4c11  (rows, cols);
GRAY_IMAGE  img_4b  (rows, cols);
GRAY_IMAGE  img_4c  (rows, cols);
GRAY_IMAGE  img_5  (rows, cols);
// Convert AXI4 Stream data to hls::mat format
hls::AXIvideo2Mat(INPUT_STREAM, img_0);

hls::Duplicate( img_0, img_1, img_1a );


hls::Duplicate( img_1a, img_1b, img_1c );

Gassian_5<GRAY_IMAGE>(img_1b, img_2a, rows, cols);
Gassian_10<GRAY_IMAGE>(img_1c, img_2b, rows, cols);

hls::Duplicate( img_2a, img_2a1, img_2ab );
hls::Duplicate( img_2b, img_2b1, img_2bb );

hls::Duplicate( img_2a1, img_2aa, img_4b1 );
hls::Duplicate( img_2b1, img_2ba, img_4c1 );

float max[MAX_HEIGHT/8][MAX_WIDTH/8] = {0};

add<GRAY_IMAGE, GRAY_PIXEL>(img_2ab, img_2bb, img_2aa, img_2ba,img_3b,rows, cols, max);

max_filter<GRAY_IMAGE, GRAY_PIXEL>(img_3b, img_3c, rows, cols, max);

hls::Duplicate( img_4b1, img_4b, img_4b11 );
hls::Duplicate( img_4c1, img_4c, img_4c11 );

sigma<GRAY_IMAGE, GRAY_PIXEL>(img_4b, img_4c, img_3c, img_4b11, img_4c11, img_5, rows, cols);
//average<GRAY_IMAGE>(img_3b, img_4a, rows, cols);

// Convert the hls::mat format to AXI4 Stream format with SOF, EOL signals
hls::Mat2AXIvideo(img_5, OUTPUT_STREAM);

}

template<typename IMG_T, typename PIXEL_T>
void sigma(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_in2, IMG_T& img_in3, IMG_T& img_in4,IMG_T& img_out, int rows, int cols) {

PIXEL_T pin0, pin1, pin2,pin3,pin4;
PIXEL_T pout;

L_row: for(int row = 0; row < rows; row++) {
#pragma HLS LOOP_TRIPCOUNT min=720 max=1080

L_col: for(int col = 0; col < cols; col++) {
#pragma HLS LOOP_TRIPCOUNT min=1280 max=1920

           img_in0 >> pin0;
           img_in1 >> pin1;
           img_in2 >> pin2;
           img_in3 >> pin3;
           img_in4 >> pin4;

           pout = (pin0*pin1)/((pin4-pin3)*pin2 + pin1);

           img_out << pout;
        }
    }

}


template<typename IMG_T, typename PIXEL_T>
void max_filter(IMG_T& img_in0, IMG_T& img_out, int rows, int cols, float (&max)[MAX_HEIGHT/8][MAX_WIDTH/8]) {

PIXEL_T pin0;
PIXEL_T pout;
int i , j;
L_row: for(int row = 0; row < rows; row++) {
#pragma HLS LOOP_TRIPCOUNT min=720 max=1080

L_col: for(int col = 0; col < cols; col++) {
#pragma HLS LOOP_TRIPCOUNT min=1280 max=1920

           img_in0 >> pin0;
           pout = pin0;
           i = row / 8;
           j = col / 8;
           pout.val[0] = max[i][j];

           img_out << pout;
        }
    }

}


template<typename IMG_T, typename PIXEL_T>
void add(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_in2, IMG_T& img_in3, IMG_T& img_out, int rows, int cols, float (&max)[MAX_HEIGHT/8][MAX_WIDTH/8]){//, float& max[rows/8][cols/8]) {

PIXEL_T pin0, pin1 ,pin2, pin3;
PIXEL_T pout1,pout2,pout;
int i , j ;


L_row: for(int row = 0; row < rows; row++) {
#pragma HLS LOOP_TRIPCOUNT min=720 max=1080

L_col: for(int col = 0; col < cols; col++) {
#pragma HLS LOOP_TRIPCOUNT min=1280 max=1920

           img_in0 >> pin0;
           img_in1 >> pin1;
           img_in2 >> pin2;
           img_in3 >> pin3;
		   pout1 = (pin0 - pin1)/(pin2 + pin3);// + ((pin0 - pin1)*256)/(pin2 + pin3);

		   i = row / 8;
		   j = col / 8;
		   if (pout1.val[0] > max[i][j])
			   max[i][j] = pout1.val[0];
           //pout = (pin0 + pin1) * (pin0+ pin1);
		   //pout = pout1 + pout2;
           img_out << pout1;
        }
    }

}


template<typename IMG_T>
void average(IMG_T& img_in, IMG_T& img_out, int rows, int cols) {

// 2D kernel for veritical gradient
const COEF_T coef_v[KS][KS]= {
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083},
  {0.083  ,  0.083  ,  0.083  ,  0.083  ,  0.083 ,   0.083 ,   0.083 ,   0.083  ,  0.083 ,   0.083 ,   0.083}
};

hls::Window<KS,KS,COEF_T> Sv;
for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sv.val[r][c] = coef_v[r][c];

// point
hls::Point_<INDEX_T> anchor;
anchor.x=-1;
anchor.y=-1;

hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sv, anchor);

}

template<typename IMG_T>
void Gassian_5(IMG_T& img_in, IMG_T& img_out, int rows, int cols) {

// 2D kernel for veritical gradient
const COEF_T coef_v[size][size]= {
		{0.0084,    0.0096,    0.0107,    0.0113,    0.0115,    0.0113,    0.0107,    0.0096,    0.0084},
	    {0.0096,    0.0111,    0.0123,    0.0130,    0.0133,    0.0130,    0.0123,    0.0111,    0.0096},
	    {0.0107,    0.0123,    0.0136,    0.0144,    0.0147,    0.0144,    0.0136,    0.0123,    0.0107},
	    {0.0113,    0.0130,    0.0144,    0.0153,    0.0156,    0.0153,    0.0144,    0.0130,    0.0113},
	    {0.0115,    0.0133,    0.0147,    0.0156,    0.0159,    0.0156,    0.0147,    0.0133,    0.0115},
	    {0.0113,    0.0130,    0.0144,    0.0153,    0.0156,    0.0153,    0.0144,    0.0130,    0.0113},
	    {0.0107,    0.0123,    0.0136,    0.0144,    0.0147,    0.0144,    0.0136,    0.0123,    0.0107},
	    {0.0096,    0.0111,    0.0123,    0.0130,    0.0133,    0.0130,    0.0123,    0.0111,    0.0096},
	    {0.0084,    0.0096,    0.0107,    0.0113,    0.0115,    0.0113,    0.0107,    0.0096,    0.0084}
};

hls::Window<size,size,COEF_T> Sv;
for (int r=0; r<size; r++) for (int c=0; c<size; c++) Sv.val[r][c] = coef_v[r][c];

// point
hls::Point_<INDEX_T> anchor;
anchor.x=-1;
anchor.y=-1;

hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sv, anchor);

}

template<typename IMG_T>
void Gassian_10(IMG_T& img_in, IMG_T& img_out, int rows, int cols) {

// 2D kernel for veritical gradient
const COEF_T coef_v[size][size]= {
	    {0.0112,    0.0116,    0.0119,    0.0121,    0.0122,    0.0121,    0.0119,    0.0116,    0.0112},
	    {0.0116,    0.0121,    0.0124,    0.0125,    0.0126,    0.0125,    0.0124,    0.0121,    0.0116},
	    {0.0119,    0.0124,    0.0127,    0.0129,    0.0129,    0.0129,    0.0127,    0.0124,    0.0119},
	    {0.0121,    0.0125,    0.0129,    0.0131,    0.0131,    0.0131,    0.0129,    0.0125,    0.0121},
	    {0.0122,    0.0126,    0.0129,    0.0131,    0.0132,    0.0131,    0.0129,    0.0126,    0.0122},
	    {0.0121,    0.0125,    0.0129,    0.0131,    0.0131,    0.0131,    0.0129,    0.0125,    0.0121},
	    {0.0119,    0.0124,    0.0127,    0.0129,    0.0129,    0.0129,    0.0127,    0.0124,    0.0119},
	    {0.0116,    0.0121,    0.0124,    0.0125,    0.0126,    0.0125,    0.0124,    0.0121,    0.0116},
	    {0.0112,    0.0116,    0.0119,    0.0121,    0.0122,    0.0121,    0.0119,    0.0116,    0.0112}
};

hls::Window<size,size,COEF_T> Sv;
for (int r=0; r<size; r++) for (int c=0; c<size; c++) Sv.val[r][c] = coef_v[r][c];

// point
hls::Point_<INDEX_T> anchor;
anchor.x=-1;
anchor.y=-1;

hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sv, anchor);

}
