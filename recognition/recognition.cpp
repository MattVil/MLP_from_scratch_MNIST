#include "opencv/highgui.h"
#include "opencv/cv.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SIZE 200

void draw_reclangle(IplImage* img);
void extract_square(IplImage* img);
 
int main() {
  
        char key;
        IplImage *image;
        CvCapture *capture;
        capture = cvCaptureFromCAM(0);
  
        if (!capture) {
            printf("Ouverture du flux vidéo impossible !\n");
           return 1;
         }
         
        image = cvQueryFrame(capture);
          
        cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("Center", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("Center", 820, 380);
        cvMoveWindow("Camera", 100, 200);       
         
        while(key != 'q' && key != 'Q') {        
            
            image = cvQueryFrame(capture);
                  
            draw_reclangle(image);

            extract_square(image);

            cvShowImage( "Camera", image);

            key = cvWaitKey(10);
         }

        cvReleaseImage(&image);
  
        cvReleaseCapture(&capture);
        cvDestroyWindow("Camera");
        cvDestroyWindow("Center");
  
        return 0;
}

void extract_square(IplImage* img){
    IplImage* img_center = cvCloneImage(img);
    IplImage* center_gray;
    IplImage* resized = cvCreateImage(cvSize(28, 28), IPL_DEPTH_8U ,1);

    CvPoint topLeft = cvPoint((((img_center->width)/2)-(SIZE/2)), (((img_center->height)/2)-(SIZE/2)));

    cvSetImageROI(img_center, cvRect(topLeft.x, topLeft.y, SIZE, SIZE));

    center_gray = cvCreateImage(cvGetSize(img_center),IPL_DEPTH_8U,1);
    cvCvtColor(img_center, center_gray, CV_RGB2GRAY);
    //cvFlip(center_gray, NULL, 1);

    cvResize(center_gray, resized);

    cvShowImage("Center", resized);

    cvReleaseImage(&img_center);
    cvReleaseImage(&center_gray);
}

void draw_reclangle(IplImage* img){

    int i, j;

    CvPoint topLeft = cvPoint((((img->width)/2)-(SIZE/2)-2), (((img->height)/2)-(SIZE/2)-2));
    CvPoint buttomRight = cvPoint((((img->width)/2)+(SIZE/2)+2), (((img->height)/2)+(SIZE/2)+2));

    cvRectangle(img, topLeft, buttomRight, CV_RGB(255, 0, 0), 2);

    for(i=topLeft.x; i<buttomRight.x +1; i++){
        for(j=topLeft.y; j<buttomRight.y +1; j++){

        }
    }
}
