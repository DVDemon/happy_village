#ifndef CAPTURESERVICEH
#define CAPTURESERVICEH

#include "Poco/Runnable.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Notification.h"
#include "Poco/AutoPtr.h"
#include "Poco/ThreadPool.h"
#include "Poco/AutoPtr.h"

#include <vector>
#include <atomic>
#include <memory>

struct CapturePayload{
    std::vector<unsigned char> buffer;
    std::atomic<int> state{0};
};

struct CapturePhotoNotification : public Poco::Notification{

    std::shared_ptr<CapturePayload> _pl;
    CapturePhotoNotification(std::shared_ptr<CapturePayload> &pl) : _pl(pl){};
};

class CaptureService : public Poco::Runnable{
    private:
        Poco::NotificationQueue& _queue;
    public:
        CaptureService(Poco::NotificationQueue& queue);
        void run() override;
};

#endif 