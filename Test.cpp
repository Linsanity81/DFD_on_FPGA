#include "DFD.h"
#include "hls_opencv.h"
#include "opencv2\\core\\core.hpp"

#include <iostream>
using namespace std;
using namespace cv;

//______________________________________________________________________________
// test sequence of grey scale image
#define INPUT_IMAGE          "new_sponge_n.bmp"
#define OUTPUT_IMAGE         "D:\\Research_Project\\Shape_Reconstruction_on_FPGA\\DFD\\DFD\\output.jpeg"

int main (int argc, char** argv) {

int nChannelsGray=1;
const int params[2]={CV_IMWRITE_JPEG_QUALITY,100};
IplImage* src = cvLoadImage(INPUT_IMAGE,CV_LOAD_IMAGE_ANYDEPTH);
IplImage *src32 = cvCreateImage(cvSize(MAX_WIDTH, MAX_HEIGHT), 32, 1);
cvConvertScale(src, src32);

// AXI stream version

AXI_STREAM src_axi, dst_axi;

//src = cvCreateImage(cvSize(MAX_WIDTH, MAX_HEIGHT), 8, nChannelsGray);
//IplImage* src32 = cvCreateImage(cvSize(MAX_WIDTH,MAX_HEIGHT),IPL_DEPTH_32F, nChannelsGray);
//IplImage* dst32 = cvCreateImage(cvSize(MAX_WIDTH, MAX_HEIGHT), IPL_DEPTH_32F, nChannelsGray);
IplImage* dst = cvCreateImage(cvSize(MAX_WIDTH, MAX_HEIGHT), 32, nChannelsGray);


for (int k=0;k<1;k++) {

    // Convert OpenCV format to AXI4 Stream format
    IplImage2AXIvideo(src32, src_axi);

    //______________________________________________________________________________
    // DUT:  the function to be synthesized
    image_filter(src_axi, dst_axi);
    //______________________________________________________________________________

    // Convert the AXI4 Stream data to OpenCV format
    AXIvideo2IplImage(dst_axi, dst);

}

// Standard OpenCV image functions
FILE *fp;
fp=fopen("D:\\Research_Project\\Shape_Reconstruction_on_FPGA\\DFD\\DFD\\result.txt","w");
CvScalar pixel;

for (int i = 0; i < dst->height; ++i)
    {
        for (int j = 0; j < dst->width; ++j)
        {

            pixel = cvGet2D(dst, i, j);

            fprintf(fp,"%f\n",pixel.val[0]);

        }
    }


cvSaveImage(OUTPUT_IMAGE, dst, params);

/*cvNamedWindow("Image:",1);
cvShowImage("Image:",dst);
cvWaitKey();
cvDestroyWindow("Image:");*/


cvReleaseImage(&src);
cvReleaseImage(&dst);
cvReleaseImage(&src32);
return 0;

}

