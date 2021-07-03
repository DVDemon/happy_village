#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
int main()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat img;

    cap >> img;
    
    if(img.empty())
    {
        std::cout << "Could capture" << std::endl;
        return 1;
    }
    
    imwrite("starry_night.png", img);
    
    return 0;
}