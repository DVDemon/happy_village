#ifndef HTTPWEBSERVER_H
#define HTTPWEBSERVER_H

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

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "http_request_factory.h"
#include "../config/config.h"
#include "../database/person.h"
#include "../capture/capture_service.h"

class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
    HTTPWebServer() : _helpRequested(false){
    }

    ~HTTPWebServer(){
    }

protected:
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet &options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
            Option("host", "h", "set ip address for database")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleHost)));
        options.addOption(
            Option("port", "po", "set mysql port")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handlePort)));
        options.addOption(
            Option("login", "lg", "set mysql login")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleLogin)));
        options.addOption(
            Option("password", "pw", "set mysql password")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handlePassword)));
        options.addOption(
            Option("database", "db", "set mysql database")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleDatabase)));
        options.addOption(
            Option("init_db", "it", "create database tables")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleInitDB)));
        options.addOption(
            Option("queue", "q", "set queue host")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleQueueHost)));
        options.addOption(
            Option("topic", "t", "set queue topic")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleQueueTopic)));
         options.addOption(
            Option("cache_servers", "cs", "set ignite cache servers")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleCacheServers)));
    }

    void handleInitDB([[maybe_unused]] const std::string &name,
                      [[maybe_unused]] const std::string &value)
    {
        std::cout << "init db" << std::endl;
        database::Person::init();
    }
    void handleLogin([[maybe_unused]] const std::string &name,
                     [[maybe_unused]] const std::string &value)
    {
        std::cout << "login:" << value << std::endl;
        Config::get().login() = value;
    }

    void handleHost([[maybe_unused]] const std::string &name,
                     [[maybe_unused]] const std::string &value)
    {
        std::cout << "host:" << value << std::endl;
        Config::get().read_request_ip() = value;
        Config::get().write_request_ip() = value;
    }
    void handlePassword([[maybe_unused]] const std::string &name,
                        [[maybe_unused]] const std::string &value)
    {
        std::cout << "password:" << value << std::endl;
        Config::get().password() = value;
    }

     void handleDatabase([[maybe_unused]] const std::string &name,
                         [[maybe_unused]] const std::string &value)
    {
        std::cout << "database:" << value << std::endl;
        Config::get().database() = value;
    }   
    void handlePort([[maybe_unused]] const std::string &name,
                    [[maybe_unused]] const std::string &value)
    {
        std::cout << "port:" << value << std::endl;
        Config::get().port() = value;
    }
    void handleQueueHost([[maybe_unused]] const std::string &name,
                       [[maybe_unused]] const std::string &value)
    {
        std::cout << "queue host:" << value << std::endl;
        Config::get().queue_host() = value;
    }

    void handleQueueTopic([[maybe_unused]] const std::string &name,
                       [[maybe_unused]] const std::string &value)
    {
        std::cout << "queue topic:" << value << std::endl;
        Config::get().queue_topic() = value;
    }

    void handleCacheServers([[maybe_unused]] const std::string &name,
                       [[maybe_unused]] const std::string &value)
    {
        std::cout << "cache servers:" << value << std::endl;
        Config::get().cache_servers() = value;
    }



    int main([[maybe_unused]] const std::vector<std::string> &args)
    {
        if (!_helpRequested)
        {
            unsigned short port = (unsigned short)
                                      config()
                                          .getInt("HTTPWebServer.port", 80);
            std::string format(
                config().getString("HTTPWebServer.format",
                                   DateTimeFormat::SORTABLE_FORMAT));

            //database::Person::warm_up_cache();

            Poco::NotificationQueue queue;
            CaptureService service(queue);

            Poco::ThreadPool::defaultPool().start(service);
            
            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
            HTTPServer srv(new HTTPRequestFactory(format,queue),
                           svs, new HTTPServerParams);
            srv.start();
            std::cout << "[+] Http service started on port:" << port << std::endl;
            waitForTerminationRequest();
            Poco::ThreadPool::defaultPool().joinAll();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};
#endif // !HTTPWEBSERVER