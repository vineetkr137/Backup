#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>
#include "/media/nvidia/b08e3d3c-4ab2-4614-b6c7-fb5b011786ac4/vineet/pyboostcvconverter/include/FlyCapture2.h"
using namespace std;
using namespace cv;
using namespace FlyCapture2;
namespace pbcvt 
{
using namespace boost::python;

Mat lmap_x;
Mat lmap_y;
Image rawImage;
Image rgbImage;
cv::Mat image;
Camera cam;
CameraInfo camInfo;
Property p;


float brightness;
int red ;
int blue;
float shutter; 
float gain ;
float frame_rate;
int frame_rate_range;

void Set_properties()
{       
	printf("[...]Setting up camera properties");
	//SET BRIGHTNESS
	p.type = FlyCapture2::BRIGHTNESS;
   	p.absControl = true;
   	p.absValue = 1.0*brightness;
   	p.autoManualMode = false;
   	p.onOff = true;

	//SET SHUTTER
	p.type = FlyCapture2::SHUTTER;
	p.autoManualMode = false;
	p.onOff = true;
	p.absControl = true;
	p.absValue = shutter;

	//SET GAIN 
	p.type = FlyCapture2::GAIN;
	p.absControl = true;
	p.autoManualMode = false;
	p.onePush = false;
	p.onOff = true;
	p.absValue = 1.0*gain;

	//SET FRAME RATE
	p.type = FlyCapture2::FRAME_RATE;
	p.absControl = true;
	p.autoManualMode = false;
	p.onOff = true;
	p.absValue = 1.0*frame_rate;

	//SET RED
	p.type = FlyCapture2::WHITE_BALANCE;
	p.absControl = false;
	p.onOff = true;
	p.valueA = (unsigned int)red;

	//SET BLUE
	p.type = FlyCapture2::WHITE_BALANCE;
	p.absControl = false;
	p.onOff = true;
	p.valueB = (unsigned int)blue;
	
	cam.SetProperty(&p);

	
}

void InitializeCameraProperties()
{
    

    printf("Camera Properties:\n");
    FILE *fp;
	fp = fopen("/media/nvidia/b08e3d3c-4ab2-4614-b6c7-fb5b011786ac4/vineet/pyboostcvconverter/src/CameraProperties.txt","r");
	string s="";
		

	size_t len=0;
	char *line=NULL;
	
	while(getline(&line, &len, fp) != -1)
	{
		 double val = 0.0;
	  s.clear();
	  printf("line %s\n", line);
	 fflush(stdout);
	  char *pt ;
	  pt = line;

	  while((*pt)!=' ')
	  {
		s.push_back(*pt);
	  	pt++;
	  }
	
	  while((*pt)==' ')
	  pt++;
	 while(((*pt)>='0') && ((*pt) <='9'))
	 {
		val = val*10.0+((*pt)-'0');
		
	        pt++;
	 }

	 double dec = 0.0;
         if(*pt=='.')
	 {
		pt++;
		
		int cnt_decimal = 1;
		while((*pt>='0') && (*pt <='9'))
		{
			dec = dec*10+(*pt-'0');
	        	pt++;
			cnt_decimal*=10;
		}
		dec/=cnt_decimal;
	 }

	     val = val+ dec;
         if(s=="BRIGHTNESS")
		 {
			 brightness = val;
				
		 }
		 else if(s=="RED")
		 red = (int)val;
		 else if(s=="BLUE")
		 blue = (int)val;
		 else if(s=="SHUTTER")
		 shutter = val;
		 else if(s=="GAIN")
		 gain = val;
		 else if(s=="FRAME_RATE")
		 frame_rate = val;
		 else if(s=="FRAME_RATE_RANGE")
		 frame_rate_range = (int)val;
	 }
	 
	Set_properties();

}

    
void Init_Flycapture()

{	
	
        cam.Connect( 0 );	
	// Get the camera info and print it out
	cam.GetCameraInfo( &camInfo );
        //std::cout << camInfo.vendorName <<" "<< camInfo.modelName <<" " ;
        // fflush(stdout);

	//InitializeCameraProperties();
	cam.StartCapture();
}

void Init_LoadMapping()
{
    	Mat map_x, map_y;
    	FileStorage lfs("/media/nvidia/b08e3d3c-4ab2-4614-b6c7-fb5b011786ac4/vineet/pyboostcvconverter/src/calibration.yml", FileStorage::READ);
    	lfs["rowMatrix"] >> map_y;
    	lfs["colMatrix"] >> map_x;
    	lfs.release();
    	map_y.convertTo(map_y, CV_32FC1);
    	map_x.convertTo(map_x, CV_32FC1);
    	convertMaps(map_x, map_y, lmap_x, lmap_y, CV_16SC2);   
    	/*
    	FileStorage rfs(RIGHT_MAP_NAME, FileStorage::READ);
    	rfs["rowMatrix"] >> map_y;
    	rfs["colMatrix"] >> map_x;
    	rfs.release();
    	map_y.convertTo(map_y, CV_32FC1);
    	map_x.convertTo(map_x, CV_32FC1);
    	convertMaps(map_x, map_y, rmap_x, rmap_y, CV_16SC2);
    	*/	
}

cv::Mat Remap_Conversion(cv::Mat Image_from_yolo)
{       
        Mat dst;
    	remap(Image_from_yolo, dst, lmap_x, lmap_y, INTER_LINEAR, BORDER_CONSTANT );
    	resize(dst, dst, Size(900,800), 0, 0, INTER_LINEAR);
    	return dst;
}

cv::Mat Stream_Flycapture()
{       
	cam.RetrieveBuffer( &rawImage );
  	rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );
	unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
        image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);
	/*IplImage* image2;
	image2 = cvCreateImage(cvSize(image.cols,image.rows),8,3);
	IplImage ipltemp=image;
	cvCopy(&ipltemp,image2);
	*/
	//resize(image, image, Size(1024, 720), 0, 0, INTER_CUBIC);
	//image = Remap_Conversion(image);
	//cv::imwrite("IMAGE_FLYCAPTURE_CPP.png", image);
	return image;	
}

#if (PY_VERSION_HEX >= 0x03000000)

    static void *init_ar() {
#else
        static void init_ar(){
#endif
        Py_Initialize();

        import_array();
        return NUMPY_IMPORT_ARRAY_RETVAL;
    }

    BOOST_PYTHON_MODULE (pbcvt) {
        //using namespace XM;
        init_ar();

        //initialize converters
        to_python_converter<cv::Mat,pbcvt::matToNDArrayBoostConverter>();
        matFromNDArrayBoostConverter();

        //expose module-level functions
         
	def("Init_Flycapture",Init_Flycapture);
	def("Stream_Flycapture",Stream_Flycapture);
	def("Init_LoadMapping",Init_LoadMapping);
    }

} //end namespace pbcvt
