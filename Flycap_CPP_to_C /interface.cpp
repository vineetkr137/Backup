#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../include/FlyCapture2.h"
using namespace std;
using namespace cv;
using namespace FlyCapture2;
Mat lmap_x;
Mat lmap_y;
Image rawImage;
Image rgbImage;
cv::Mat image;
Camera cam;
CameraInfo camInfo;


IplImage*  Remap_Conversion(IplImage* Image_from_yolo)
{       
        	//Converting 'IplImage' into 'Mat' format
		Mat src = cvarrToMat(Image_from_yolo);
		if(! src.data )                              // Check for invalid input
		    {	
        		cout <<  "\nCould not open or find the image\n" << std::endl ;
       		
    		    }
    		Mat dst;
    		remap(src, dst, lmap_x, lmap_y, INTER_LINEAR, BORDER_CONSTANT );
    		resize(dst, dst, Size(900,800), 0, 0, INTER_LINEAR);
    
	  	

		//Converting 'Mat' again into 'IplImage' format

		IplImage* image2;
		image2 = cvCreateImage(cvSize(dst.cols,dst.rows),8,3);
		IplImage ipltemp=dst;
		cvCopy(&ipltemp,image2);
		//cvSaveImage("IMAGE_REMAPED.png", image2);
		Mat vineet = cvarrToMat(image2);
		return image2;
}


IplImage* stream_flycapture()
{       
	cam.RetrieveBuffer( &rawImage );	             
  	rawImage.Convert( FlyCapture2::PIXEL_FORMAT_RGB, &rgbImage );
	unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
        image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);
	IplImage* image2;
	image2 = cvCreateImage(cvSize(image.cols,image.rows),8,3);
	IplImage ipltemp=image;
	cvCopy(&ipltemp,image2);
	//Using Remap function here
	image2 = Remap_Conversion(image2);
	//cvSaveImage("IMAGE_FLYCAPTURE_CPP.png", image2);
	return image2;
	
}

void initiation_flycapture()

{
        cam.Connect( 0 );	
	// Get the camera info and print it out
	cam.GetCameraInfo( &camInfo );
        //std::cout << camInfo.vendorName <<" "<< camInfo.modelName <<" " ;
        // fflush(stdout);
	cam.StartCapture();
}

void LoadMapping()
{
		Mat map_x, map_y;
		//printf("MATRIX");
    		FileStorage lfs("/media/nvidia/b08e3d3c-4ab2-4614-b6c7-fb5b011786ac4/vineet/Flycap_CPP_YOLO_with/calibration.yml", FileStorage::READ);
    		lfs["rowMatrix"] >> map_y;
    		lfs["colMatrix"] >> map_x;
    		lfs.release();	
    		map_y.convertTo(map_y, CV_32FC1);
    		map_x.convertTo(map_x, CV_32FC1);
    		convertMaps(map_x, map_y, lmap_x, lmap_y, CV_16SC2);   
 	       /* FileStorage rfs(RIGHT_MAP_NAME, FileStorage::READ);
    		rfs["rowMatrix"] >> map_y;
    		rfs["colMatrix"] >> map_x;
    		rfs.release();
    		map_y.convertTo(map_y, CV_32FC1);
    		map_x.convertTo(map_x, CV_32FC1);
    		convertMaps(map_x, map_y, rmap_x, rmap_y, CV_16SC2);
		*/
}



extern "C"
{
	void initiation_fly()
	{
		initiation_flycapture();
	}

	IplImage* fly_image()
	{
		return stream_flycapture();
	}
	
	void call_load_mapping()
	{
		LoadMapping();
	}
	
	IplImage* GetCppData(IplImage* Image_from_yolo)
	{      
		
	    return Remap_Conversion(Image_from_yolo);
		
	}

}
