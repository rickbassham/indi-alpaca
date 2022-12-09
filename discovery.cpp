#include <deque>
#include <memory>
#include <string>

#if defined(_WIN32) || defined(__USE_W32_SOCKETS)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#ifdef _MSC_VER
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif
#else
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||                     \
        defined(__bsdi__) || defined(__DragonFly__)
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <arpa/inet.h>
#include <ifaddrs.h>
#endif

#include <libindi/indicom.h>
#include <libindi/indidevapi.h>
#include <libindi/json.h>

#include "discovery.h"

#define ALPACA_DISCOVERY_PORT 32227
#define RECEIVE_BUFFER_SIZE 64
#define ALPACA_DISCOVERY_TIMEOUT 1 // Seconds

using namespace INDI;

static class Loader
{
    std::deque<std::unique_ptr<AlpacaBase>> devices;
public:
    Loader()
    {
        IDLog("Loading Alpaca Devices driver\n");
        discover();
    }

    void discover()
    {
        IDLog("Sending discovery packet\n");

        int s;

        s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (s == -1)
        {
            IDLog("Error creating socket\n");
            return;
        }

        int broadcast=1;
        setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

        struct timeval tv;
        tv.tv_sec = ALPACA_DISCOVERY_TIMEOUT;
        tv.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));

        struct sockaddr_in Recv_addr;

        socklen_t len = sizeof(struct sockaddr_in);

        char recvbuff[RECEIVE_BUFFER_SIZE];

        Recv_addr.sin_family       = AF_INET;
        Recv_addr.sin_port         = htons(ALPACA_DISCOVERY_PORT);
        Recv_addr.sin_addr.s_addr  = INADDR_BROADCAST;

        char sendMSG[17] = "alpacadiscovery1";

        sendto(s, sendMSG, strlen(sendMSG), 0, (sockaddr *)&Recv_addr, sizeof(Recv_addr));

        while (true)
        {
            memset(recvbuff, 0, RECEIVE_BUFFER_SIZE);

            int n = recvfrom(s, recvbuff, RECEIVE_BUFFER_SIZE, 0, (sockaddr *)&Recv_addr, &len);
            if (n < 0)
                break;

            IDLog("Received: %s\n", recvbuff);

            nlohmann::json doc = nlohmann::json::parse(recvbuff);

            if (doc.contains("AlpacaPort"))
            {
                char deviceIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(Recv_addr.sin_addr), deviceIP, INET_ADDRSTRLEN);

                int port = doc["AlpacaPort"];

                IDLog("Found Alpaca Device Server at %s:%d\n", deviceIP, port);

                char url[256];
                memset(url, 0, 256);
                snprintf(url, 256, "http://%s:%d/management/v1/description", deviceIP, port);

                auto doc = get_json(url);
                if (doc == nullptr)
                {
                    return;
                }

                std::string serverName = doc["Value"]["ServerName"];
                std::string manufacturer = doc["Value"]["Manufacturer"];
                std::string manufacturerVersion = doc["Value"]["ManufacturerVersion"];
                std::string location = doc["Value"]["Location"];

                IDLog("ServerName: %s\n", serverName.c_str());
                IDLog("Manufacturer: %s\n", manufacturer.c_str());
                IDLog("ManufacturerVersion: %s\n", manufacturerVersion.c_str());
                IDLog("Location: %s\n", location.c_str());

                IDLog("\n\n");

                memset(url, 0, 256);
                snprintf(url, 256, "http://%s:%d/management/v1/configureddevices", deviceIP, port);

                doc = get_json(url);
                if (doc == nullptr)
                {
                    return;
                }

                for (auto &device : doc["Value"])
                {
                    std::string deviceName = device["DeviceName"];
                    std::string deviceType = device["DeviceType"];
                    uint32_t deviceNumber = device["DeviceNumber"];
                    std::string uniqueId = device["UniqueID"];

                    IDLog("Found Device: \n");
                    IDLog("DeviceName: %s\n", deviceName.c_str());
                    IDLog("DeviceType: %s\n", deviceType.c_str());
                    IDLog("DeviceNumber: %d\n", deviceNumber);
                    IDLog("UniqueID: %s\n\n", uniqueId.c_str());

                    std::transform(deviceType.begin(), deviceType.end(), deviceType.begin(), ::tolower);

                    if (deviceType == "covercalibrator")
                    {
                        devices.push_back(std::unique_ptr<AlpacaBase>(new CoverCalibrator(serverName, manufacturer, manufacturerVersion, location, deviceName, deviceType, deviceNumber, uniqueId, deviceIP, port)));
                    }
                }
                // lights.push_back(std::unique_ptr<DragonLight>(new DragonLight(std::string(str))));
            }
        }

        IDLog("discovery complete\n");
    }
} loader;
