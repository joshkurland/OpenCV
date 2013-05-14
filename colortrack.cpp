#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




IplImage* imgPos    = NULL;
IplImage* imgRed    = NULL;
IplImage* imgcolor  = NULL;

CvScalar minC;
CvScalar maxC;

char key  = 'd';

// Detect a red ball
CvScalar hsv_min = cvScalar(1, 70, 200, 0); //3, 230, 100
CvScalar hsv_max = cvScalar(5, 240, 255, 0);  //358,256,255

IplImage* getThresholdImage(IplImage* img)
{
  IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);  	//Convert image to HSV
	
	IplImage* thresholded = cvCreateImage(cvGetSize(img), 8, 1);
	
  if(minC.val[0] == NULL && maxC.val[0] == NULL){cvInRangeS(imgHSV, hsv_min, hsv_max, thresholded);}  
	else{cvInRangeS(imgHSV, minC, maxC, thresholded);}

	cvReleaseImage(&imgHSV);
	return thresholded;
}

IplImage* getMoment(IplImage* img)
{
		//Calculate Position
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		cvMoments(imgRed, moments, 1);
		double moment10 = cvGetSpatialMoment(moments, 1, 0);
		double moment01 = cvGetSpatialMoment(moments, 0, 1);
		double area   = cvGetCentralMoment(moments,0,0);
		static int posX = 0;
		static int posY = 0;
		int lastX = posX;
		int lastY = posY;
		posX = moment10/area;
		posY = moment01/area;
		CvPoint* center = new CvPoint();
		center->x = lastX;
		center->y = lastY;
		printf("%d lastX", lastX);
		printf("%d lastY", lastY);
		if(lastX != posX && lastY != posY)
		{
		  cvDrawCircle(img, *center, 25, CV_RGB(10,10,255), -1);
		}
		
   return 0;
}

IplImage* getColor(IplImage* img)
{
	//printf("Place object in the middle of the frame");
  IplImage* img2 = cvCreateImage(cvGetSize(img),8,3);
  cvCvtColor(img,img2,CV_BGR2HSV);
	IplImage* roi = cvCreateImage(cvGetSize(img), 8, 3);
	roi = cvCloneImage(img2);
	cvSetImageROI(roi, cvRect(220,160,160,128)); // Center object in the middle of the frame
	
	// Find avg and std dev of Image
  CvScalar meanScalar;
  CvScalar stdDevScalar;
  cvAvgSdv(roi, &meanScalar, &stdDevScalar);
  
  double meanH;
  double meanS;
  double meanV;
  double stdH;
  double stdS;
  double stdV;
  
  meanH = meanScalar.val[0];
  meanS = meanScalar.val[1];
  meanV = meanScalar.val[2];
  stdH  = stdDevScalar.val[0];
  stdS  = stdDevScalar.val[1];
  stdV  = stdDevScalar.val[2];
  
  // Set ranges 
  double minH;
  double minS;
  double minV;
  double maxH;
  double maxS;
  double maxV;
  
  minH = meanH - stdH;
  minS = meanS - stdS;
  minV = meanV - stdV;
  maxH = meanH + stdH;
  maxS = meanS + stdS;
  maxV = meanV + stdV;
   
  minC.val[0] = minH;
  minC.val[1] = minS;
  minC.val[2] = minV;
  maxC.val[0] = maxH;
  maxC.val[1] = maxS;
  maxC.val[2] = maxV;

	cvReleaseImage(&img2);

  cvShowImage("ROI", roi);
  cvReleaseImage(&roi);  
}


int main()
{
	CvCapture* capture = 0;
	printf("Initializing Capture...\n");
//  capture = cvCreateFileCapture("http://192.168.1.100/videostream.cgi?user=admin&pwd=123456");
 capture = cvCaptureFromCAM( CV_CAP_ANY );
  printf("Camera found\n");
	if(!capture){printf("Could not initialize camera...\n");}
	printf("Capture Success!!!\n");
	cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
	cvResizeWindow("Video", 640,480);
	cvMoveWindow("Video", 0,100);
	cvNamedWindow("HSV",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("ROI");
	cvMoveWindow("ROI", 700,100);

  int state = 0;
  int track = 1;
  int change= 2;
  int reset = 3;
  int clear = 4;

	while(1)
	{
	  // Hold frame captured from camera
		IplImage* img = cvQueryFrame(capture);
		if(!img){printf("Image Query Failed");break;}
		
	  key = cvWaitKey(10);
	  if(key=='t'){state = track;}
	  if(key=='c'){state = change;}
	  if(key=='r'){state = reset;}
	  if(key=='q'){state = clear;}
	  key = cvWaitKey(10);
	
		
		
		if(state == track)
		{   
		  printf("Tracking\n");
      imgRed = getThresholdImage(img);
      imgPos = getMoment(img);
    }   
	  key = cvWaitKey(10); 
       	
		if(state == change)
		{
      cvRectangle(img,cvPoint(250,300),cvPoint(400,200),CV_RGB(255,0,0),5,8);
      cvWaitKey(30);
		  cvShowImage("Video",img);
			//printf("Getting Color\n");
			IplImage* imgColor = getColor(img);
  	  state = 0;
		}
		key = cvWaitKey(10);
		
		if(state == reset)
		{
			 printf("Resetting\n");
			 minC = hsv_min;
			 maxC = hsv_max;
			 state = 0;
		}
		
		if(state == clear)
		{
		  printf("Paused\n");
		  state = 0;
		  
		}
    else 
    {
      key = cvWaitKey(10);
    }

		cvShowImage("HSV", imgRed);
		cvShowImage("Video", img);
		cvReleaseImage(&imgPos);
		cvReleaseImage(&imgRed);

    if ( (cvWaitKey(10) & 255) == 27 ) break;
		//key = cvWaitKey(10);
	}
	cvDestroyWindow("Video");
	cvDestroyWindow("HSV");
	cvDestroyWindow("ROI");
	cvReleaseCapture(&capture);
}
