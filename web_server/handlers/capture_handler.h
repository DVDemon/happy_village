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
#include "Poco/NotificationQueue.h"

#include <iostream>
#include <vector>
#include <thread>

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

#include "../../capture/capture_service.h"
class CaptureHandler: public HTTPRequestHandler
{
public:
    CaptureHandler(const std::string& format,Poco::NotificationQueue &queue): _format(format),_queue(queue)
    {
    }

    void handleRequest([[maybe_unused]] HTTPServerRequest& request,
                       HTTPServerResponse& response)
    {
        response.setChunkedTransferEncoding(true);
        response.setContentType("image/jpeg");

        std::ostream& ostr = response.send();

        std::shared_ptr<CapturePayload> pl(new CapturePayload());

        _queue.enqueueNotification(new CapturePhotoNotification(pl));


        while(pl->state==0) std::this_thread::yield();
       
        if(pl->state==1)
            ostr.write((const char*)pl->buffer.data(),pl->buffer.size());
            else 
            {
                response.setContentType("text/html");
                ostr << "<html><body>Error capture photo</body><html" << std::endl;
            }
        
    }

private:
    std::string _format;
    Poco::NotificationQueue &_queue;
};
#endif // !CAPTUREHANDLER_H