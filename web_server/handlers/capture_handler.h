#ifndef CAPTUREHANDLER_H
#define CAPTUREHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

class CaptureHandler: public HTTPRequestHandler
{
public:
    CaptureHandler(const std::string& format): _format(format)
    {
    }

    void handleRequest([[maybe_unused]] HTTPServerRequest& request,
                       HTTPServerResponse& response)
    {
        response.setChunkedTransferEncoding(true);
        response.setContentType("image/jpeg");

        std::ostream& ostr = response.send();


        cv::VideoCapture cap(0); // open the default camera
        if(!cap.isOpened())  // check if we succeeded
        {
            std::cout << "Could capture" << std::endl;
            response.setChunkedTransferEncoding(true);
            response.setContentType("text/html");
            ostr << "<html><body><h1>open camera error</h1></body></html" <<std::endl;
            return;
        }


        cv::Mat img;

        cap >> img;
    
        if(img.empty())
        {
            std::cout << "Could capture" << std::endl;
            response.setChunkedTransferEncoding(true);
            response.setContentType("text/html");
            ostr << "<html><body><h1>capture error</h1></body></html" <<std::endl;
            return ;
        }

        response.setChunkedTransferEncoding(true);
        response.setContentType("image/jpeg");
        cv::imwrite("buffer.jpeg", img);

        std::ifstream file( "buffer.jpeg", std::ios::binary );
        std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
        /*if (file.is_open()){
            while (file.good()){
                int sign = file.get();
                if(sign>0)
                ostr <<  (unsigned char)sign;
            } 
            file.close();*/
        std::cout << buffer.size() << std::endl;
        ostr.write(buffer.data(),buffer.size());
        //} else std::cerr << "cant open file" << std::endl;

        
    }

private:
    std::string _format;
};
#endif // !CAPTUREHANDLER_H