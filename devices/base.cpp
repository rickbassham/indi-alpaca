#include "base.h"
#include "config.h"

#include <memory>
#include <string>
#include <stdexcept>

#include <libindi/defaultdevice.h>

using namespace INDI;

AlpacaBase::AlpacaBase(
    DefaultDevice *device,
    std::string serverName,
    std::string manufacturer,
    std::string manufacturerVersion,
    std::string location,
    std::string deviceName,
    std::string deviceType,
    uint32_t deviceNumber,
    std::string uniqueId,
    std::string ipAddress,
    uint16_t port
)
{
    _device = device;
    _serverName = serverName;
    _manufacturer = manufacturer;
    _manufacturerVersion = manufacturerVersion;
    _location = location;
    _deviceName = deviceName;
    _deviceType = deviceType;
    _deviceNumber = deviceNumber;
    _uniqueId = uniqueId;
    _ipAddress = ipAddress;
    _port = port;
}

bool AlpacaBase::initAlpacaBaseProperties()
{
    serverDescriptionTP[ServerDescription::SERVER_NAME].fill("SERVER_NAME", "Server Name", _serverName);
    serverDescriptionTP[ServerDescription::MANUFACTURER].fill("MANUFACTURER", "Manufacturer", _manufacturer);
    serverDescriptionTP[ServerDescription::MANUFACTURER_VERSION].fill("MANUFACTURER_VERSION", "Manufacturer Version", _manufacturerVersion);
    serverDescriptionTP[ServerDescription::LOCATION].fill("LOCATION", "Location", _location);
    serverDescriptionTP.fill(_device->getDeviceName(), "SERVER_DESCRIPTION", "Server Description", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(serverDescriptionTP);

    configuredDeviceTP[ConfiguredDevice::DEVICE_NAME].fill("DEVICE_NAME", "Device Name", _deviceName);
    configuredDeviceTP[ConfiguredDevice::DEVICE_TYPE].fill("DEVICE_TYPE", "Device Type", _deviceType);
    configuredDeviceTP[ConfiguredDevice::DEVICE_NUMBER].fill("DEVICE_NUMBER", "Device Number", std::to_string(_deviceNumber));
    configuredDeviceTP[ConfiguredDevice::UNIQUE_ID].fill("UNIQUE_ID", "Unique ID", _uniqueId);
    configuredDeviceTP.fill(_device->getDeviceName(), "CONFIGURED_DEVICE", "Configured Device", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(configuredDeviceTP);

    deviceTP[Device::DEVICE_DESCRIPTION].fill("DEVICE_DESCRIPTION", "Device Description", "");
    deviceTP.fill(_device->getDeviceName(), "DEVICE", "Device", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(deviceTP);

    driverInfoTP[DriverInfo::DRIVER_DESCRIPTION].fill("DRIVER_DESCRIPTION", "Driver Description", "");
    driverInfoTP.fill(_device->getDeviceName(), "DRIVER_INFO", "Driver Info", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(driverInfoTP);

    driverVersionTP[DriverVersion::DRIVER_VERSION].fill("DRIVER_VERSION", "Driver Version", "");
    driverVersionTP.fill(_device->getDeviceName(), "DRIVER_VERSION", "Driver Version", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(driverVersionTP);

    interfaceVersionNP[InterfaceVersion::INTERFACE_VERSION].fill("INTERFACE_VERSION", "Interface Version", "%d", 0, 0, 0, 0);
    interfaceVersionNP.fill(_device->getDeviceName(), "INTERFACE_VERSION", "Interface Version", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(interfaceVersionNP);

    nameTP[Name::NAME].fill("NAME", "Name", "");
    nameTP.fill(_device->getDeviceName(), "NAME", "Name", INFO_TAB, IP_RO, 60, IPS_IDLE);
    _device->registerProperty(nameTP);


    return true;
}

nlohmann::json AlpacaBase::doGetRequest(const std::string url)
{
    std::string fullUrl = "http://" + _ipAddress + ":" + std::to_string(_port) + url + "?ClientID=" + std::to_string(_clientId) + "&ClientTransactionID=" + std::to_string(_clientTransactionId);

    return get_json(fullUrl.c_str());
}

nlohmann::json AlpacaBase::doPutRequest(const std::string url, std::map<std::string, std::string> &body)
{
    std::string fullUrl = "http://" + _ipAddress + ":" + std::to_string(_port) +  url;

    body["ClientID"] = std::to_string(_clientId);
    body["ClientTransactionID"] = std::to_string(_clientTransactionId);

    return put_json(fullUrl.c_str(), body);
}

nlohmann::json AlpacaBase::doDeviceGetRequest(const std::string url)
{
    std::string deviceUrl = "/api/v1/" + _deviceType + "/" + std::to_string(_deviceNumber) + url;

    return doGetRequest(deviceUrl);
}

nlohmann::json AlpacaBase::doDevicePutRequest(const std::string url, std::map<std::string, std::string> &body)
{
    std::string deviceUrl = "/api/v1/" + _deviceType + "/" + std::to_string(_deviceNumber) + url;

    return doPutRequest(deviceUrl, body);
}

bool AlpacaBase::hasError(nlohmann::json &doc)
{
    if (doc == nullptr)
    {
        DEBUGDEVICE(_device->getDeviceName(), INDI::Logger::DBG_ERROR, "Non-200 response from Alpaca device.");
        return true;
    }

    if (doc.contains("ErrorNumber") && doc["ErrorNumber"] > 0)
    {
        DEBUGFDEVICE(_device->getDeviceName(), INDI::Logger::DBG_ERROR,"Error: %d %s", doc["ErrorNumber"].get<int>(), doc["ErrorMessage"].get<std::string>().c_str());
        return true;
    }

    return false;
}

bool AlpacaBase::putConnected(const bool connected)
{
    std::map<std::string, std::string> body;

    body["Connected"] = connected;

    auto response = doDevicePutRequest("/connected", body);

    if (hasError(response))
        return false;

    return true;
}

bool AlpacaBase::getConnected()
{
    nlohmann::json response = doDeviceGetRequest("/connected");

    if (hasError(response))
        return false;

    return true;
}
