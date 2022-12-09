#include "base.h"
#include "config.h"

#include <memory>
#include <string>
#include <stdexcept>

using namespace INDI;

AlpacaBase::AlpacaBase(
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
    : DefaultDevice()
{
    setVersion(VERSION_MAJOR, VERSION_MINOR);

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

void AlpacaBase::ISGetProperties(const char *dev)
{
    DefaultDevice::ISGetProperties(dev);
}

bool AlpacaBase::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (DefaultDevice::ISNewNumber(dev, name, values, names, n))
        return true;

    return false;
}

bool AlpacaBase::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (DefaultDevice::ISNewSwitch(dev, name, states, names, n))
        return true;

    return false;
}

bool AlpacaBase::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (DefaultDevice::ISNewText(dev, name, texts, names, n))
        return true;

    return false;
}

bool AlpacaBase::ISSnoopDevice(XMLEle *root)
{
    return DefaultDevice::ISSnoopDevice(root);

    return false;
}

bool AlpacaBase::initProperties()
{
    INDI::DefaultDevice::initProperties();

    serverDescriptionTP[ServerDescription::SERVER_NAME].fill("SERVER_NAME", "Server Name", _serverName);
    serverDescriptionTP[ServerDescription::MANUFACTURER].fill("MANUFACTURER", "Manufacturer", _manufacturer);
    serverDescriptionTP[ServerDescription::MANUFACTURER_VERSION].fill("MANUFACTURER_VERSION", "Manufacturer Version", _manufacturerVersion);
    serverDescriptionTP[ServerDescription::LOCATION].fill("LOCATION", "Location", _location);
    serverDescriptionTP.fill(getDeviceName(), "SERVER_DESCRIPTION", "Server Description", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(serverDescriptionTP);

    configuredDeviceTP[ConfiguredDevice::DEVICE_NAME].fill("DEVICE_NAME", "Device Name", _deviceName);
    configuredDeviceTP[ConfiguredDevice::DEVICE_TYPE].fill("DEVICE_TYPE", "Device Type", _deviceType);
    configuredDeviceTP[ConfiguredDevice::DEVICE_NUMBER].fill("DEVICE_NUMBER", "Device Number", std::to_string(_deviceNumber));
    configuredDeviceTP[ConfiguredDevice::UNIQUE_ID].fill("UNIQUE_ID", "Unique ID", _uniqueId);
    configuredDeviceTP.fill(getDeviceName(), "CONFIGURED_DEVICE", "Configured Device", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(configuredDeviceTP);

    deviceTP[Device::DEVICE_DESCRIPTION].fill("DEVICE_DESCRIPTION", "Device Description", "");
    deviceTP.fill(getDeviceName(), "DEVICE", "Device", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(deviceTP);

    driverInfoTP[DriverInfo::DRIVER_DESCRIPTION].fill("DRIVER_DESCRIPTION", "Driver Description", "");
    driverInfoTP.fill(getDeviceName(), "DRIVER_INFO", "Driver Info", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(driverInfoTP);

    driverVersionTP[DriverVersion::DRIVER_VERSION].fill("DRIVER_VERSION", "Driver Version", "");
    driverVersionTP.fill(getDeviceName(), "DRIVER_VERSION", "Driver Version", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(driverVersionTP);

    interfaceVersionNP[InterfaceVersion::INTERFACE_VERSION].fill("INTERFACE_VERSION", "Interface Version", "%d", 0, 0, 0, 0);
    interfaceVersionNP.fill(getDeviceName(), "INTERFACE_VERSION", "Interface Version", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(interfaceVersionNP);

    nameTP[Name::NAME].fill("NAME", "Name", "");
    nameTP.fill(getDeviceName(), "NAME", "Name", INFO_TAB, IP_RO, 60, IPS_IDLE);
    registerProperty(nameTP);

    addAuxControls();

    return true;
}

bool AlpacaBase::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    return true;
}

const char *AlpacaBase::getDefaultName()
{
    return "AlpacaBase";
}

bool AlpacaBase::saveConfigItems(FILE *fp)
{
    DefaultDevice::saveConfigItems(fp);

    return true;
}

bool AlpacaBase::Connect()
{
    bool rc = putConnected(true);

    if (rc)
        SetTimer(POLLMS);

    return rc;
}

bool AlpacaBase::Disconnect()
{
    return putConnected(false);
}

void AlpacaBase::TimerHit()
{
    if (!isConnected())
        return;

    SetTimer(POLLMS);
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
        LOG_ERROR("Non-200 response from Alpaca device.");
        return true;
    }

    if (doc.contains("ErrorNumber") && doc["ErrorNumber"] > 0)
    {
        LOGF_ERROR("Error: %d %s", doc["ErrorNumber"].get<int>(), doc["ErrorMessage"].get<std::string>().c_str());
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
