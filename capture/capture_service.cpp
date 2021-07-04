#include "capture_service.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

CaptureService::CaptureService(Poco::NotificationQueue& queue) : _queue(queue){

}

void CaptureService::run(){
    std::cout << "[+] Webcam capture service started" << std::endl;
    Poco::AutoPtr<Poco::Notification>  ptr (_queue.waitDequeueNotification());
    cv::VideoCapture cap(0); // open the default camera

    while(ptr){
        CapturePhotoNotification *p_ntf = dynamic_cast<CapturePhotoNotification*> (ptr.get());
        if(p_ntf){
            if(!cap.isOpened())  // check if we succeeded
            {
                std::cout << "Could not open capture device" << std::endl;
                p_ntf->_pl->state = -1;
                return;
            }

            cv::Mat img;

            cap >> img;
        
            if(img.empty())
            {
                std::cout << "Could not capture" << std::endl;
                p_ntf->_pl->state = -1;
                return ;
            }

            std::vector<int> param(2);
            param[0] = cv::IMWRITE_JPEG_QUALITY;
            param[1] = 80;//default(95) 0-100
            cv::imencode(".jpg", img, p_ntf->_pl->buffer, param);
            p_ntf->_pl->state = 1;
        }
        ptr = _queue.waitDequeueNotification();
    }
}