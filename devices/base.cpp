#include "base.h"
#include "config.h"

#include <memory>
#include <string>
#include <stdexcept>

using namespace INDI;

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ) {
        throw std::runtime_error( "Error during formatting." );
    }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

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

    setDriverInterface(getDriverInterface() | BaseDevice::DUSTCAP_INTERFACE | BaseDevice::LIGHTBOX_INTERFACE);

    return true;
}

bool AlpacaBase::updateProperties()
{
    return false;
}

const char *AlpacaBase::getDefaultName()
{
    return "AlpacaBase";
}

bool AlpacaBase::saveConfigItems(FILE *fp)
{
    return false;
}

bool AlpacaBase::Connect()
{
    return false;
}

bool AlpacaBase::Disconnect()
{
    return false;
}

void AlpacaBase::TimerHit()
{

}

nlohmann::json AlpacaBase::doGetRequest(const std::string url)
{
    std::string fullUrl = string_format("http://%s:%u/api/v1/%s?clientId=%u&clientTransactionId=%u", _ipAddress, _port, url, _clientId, _clientTransactionId);

    return get_json(fullUrl.c_str());
}

nlohmann::json AlpacaBase::doPutRequest(const std::string url, std::map<std::string, std::string> &body)
{
    std::string fullUrl = string_format("http://%s:%u/api/v1/%s", _ipAddress, _port, url);

    body["clientId"] = std::to_string(_clientId);
    body["clientTransactionId"] = std::to_string(_clientTransactionId);

    return get_json(fullUrl.c_str());
}

bool AlpacaBase::putConnected(const bool connected)
{
    std::string url = string_format("/api/v1/%s/%u/connected", _deviceType, _deviceNumber);
    std::map<std::string, std::string> body;

    body["Connected"] = connected;

    return doPutRequest(url, body);
}

bool AlpacaBase::getConnected()
{
    std::string url = string_format("/api/v1/%s/%u/connected", _deviceType, _deviceNumber);

    nlohmann::json response = doGetRequest(url);

    if (response == nullptr)
        return false;

    return response["Value"].get<bool>();
}