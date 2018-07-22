#include<iostream>
#include<math.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <wiringPi.h>
#include <sys/types.h>
#include <unistd.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define	mr1	0
#define	mr2	1
#define	ml1	2
#define	ml2	3


using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame);

String stop_cascade_name = "cascade.xml";
CascadeClassifier stop_cascade;
string window_name = "Capture - Stop detection";
int fps=60;



void stop()
{
digitalWrite (mr1, LOW) ;
digitalWrite (mr2, LOW) ;
digitalWrite (ml1, LOW) ;
digitalWrite (ml2, LOW) ;
}

void s (void)
{
  wiringPiSetup () ;
  pinMode (mr1, OUTPUT) ;
   pinMode (mr2, OUTPUT) ;
    pinMode (ml1, OUTPUT) ;
     pinMode (ml2, OUTPUT) ;
     }
void straight()
{
digitalWrite (mr1, HIGH) ;
digitalWrite (mr2, LOW) ;
digitalWrite (ml1, HIGH) ;
digitalWrite (ml2, LOW) ;


}
void right()
{
digitalWrite (mr1, LOW) ;
digitalWrite (mr2, LOW) ;
digitalWrite (ml1, HIGH) ;
digitalWrite (ml2, LOW) ;
delay(fps);
straight();


}
void left()
{
digitalWrite (mr1, HIGH) ;
digitalWrite (mr2, LOW) ;
digitalWrite (ml1, LOW) ;
digitalWrite (ml2, LOW) ;
delay(fps);
straight();

}

int low = 18, high = 135, thresholdd = 100,thresholdp=2, linelength = 50, maxlinegap = 200, bin_threshold = 80;


void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(10,10));
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 2, 8, 0);
		imwrite("stop1.jpg",frame);
		stop();
		delay(5000);
	}

}


int main()
{
    s();
	Mat img1, ROI, grey, binary, dil, erod, blur, cany;
	face_cascade.load(face_cascade_name);
	vector<Vec4i> lines;
	vector<Point> buf(3);

	VideoCapture cap(0);
	while (1){

		cap >> img1;

		int height = (img1.rows / 2), width = (img1.cols);
		Rect r(0, height, width, height - 1);
		ROI = img1(r);
		cvtColor(ROI, grey, CV_RGB2GRAY);
		threshold(grey, binary, bin_threshold, 255, THRESH_BINARY);
		dilate(binary, dil, Mat(), Point(-1, -1), 1);
		erode(dil, erod, Mat(), Point(-1, -1), 5);
		Canny(erod, cany, low, high);
		HoughLinesP(cany, lines, 1, 3.14 / 180, thresholdp, linelength, maxlinegap);
		if (lines.size())
		{
			for (size_t i = 0; i < lines.size(); i++)
			{
				Vec4i l = lines[i];
				line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 10);

			}
		}

		buf[0] = Point(10, ROI.rows-10);
		buf[1] = Point(630,ROI.rows-10);

		LineIterator it(ROI, Point(320, ROI.rows-10), Point(0, ROI.rows-10), 8);
		LineIterator it2(ROI, Point(320, ROI.rows-10), Point(640, ROI.rows-10), 8);

		for (int i = 0; i < it.count; i++, ++it)
		{
			Vec3b val1 = ROI.at<Vec3b>(it.pos());

			if ((int)val1(2) == 255)
			{
				buf[0] = it.pos();
				break;
			}
		}
		for (int i = 0; i < it2.count; i++, ++it2)
		{
			Vec3b val2 = ROI.at<Vec3b>(it2.pos());
			if ((int)val2(2) == 255)
			{
				buf[1] = it2.pos();
				break;
			}
		}
		buf[2].x = (buf[0].x + buf[1].x) / 2;
		buf[2].y = (buf[0].y + buf[1].y) / 2;
		line(ROI, Point(ROI.cols / 2, 0), Point(ROI.cols / 2, ROI.rows-10), Scalar(0, 255, 0), 1);
		line(ROI, Point(0, ROI.rows-10), Point(640, ROI.rows-10), Scalar(0, 255, 0), 1);
		line(ROI, buf[2], buf[2], Scalar(255, 0, 0), 10);
        pid_t pid=fork();
        if(pid==0){
        if (buf[2].x>320)
            {right();
            exit(0);
            }
       else if(buf[2].x<320)
         {
         left();
         exit(0);
         }


        else{
        straight();
        }
          exit(0);
          }

    waitKey(10);
	}

return 0;
}
