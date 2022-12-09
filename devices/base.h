
#pragma once
#ifndef ALPACABASE_H
#define ALPACABASE_H

#include <libindi/indibase.h>
#include <libindi/indipropertytext.h>
#include <libindi/indipropertynumber.h>
#include "jsonRequest.h"

#define ALPACA_ERROR_NOT_IMPLEMENTED 0x400
#define ALPACA_ERROR_INVALID_VALUE 0x401
#define ALPACA_ERROR_VALUE_NOT_SET 0x402
#define ALPACA_ERROR_NOT_CONNECTED 0x407
#define ALPACA_ERROR_INVALID_WHILE_PARKED 0x408
#define ALPACA_ERROR_INVALID_WHILE_SLAVED 0x409
#define ALPACA_ERROR_INVALID_OPERATION 0x40B
#define ALPACA_ERROR_ACTION_NOT_IMPLEMENTED 0x40C

namespace INDI
{
/**
 * @brief The AlpacaBase class.
 *
 * @author Rick Bassham
 */
class AlpacaBase
{
public:
    AlpacaBase(
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
    );
    virtual ~AlpacaBase() = default;

protected:
    virtual bool initAlpacaBaseProperties();

protected:
    std::string _serverName;
    std::string _manufacturer;
    std::string _manufacturerVersion;
    std::string _location;
    std::string _deviceName;
    std::string _deviceType;
    uint32_t _deviceNumber;
    std::string _uniqueId;
    std::string _ipAddress;
    uint16_t _port;

private:
    uint32_t _clientId;
    uint32_t _clientTransactionId;
    DefaultDevice *_device;


protected:
    nlohmann::json doGetRequest(const std::string url);
    nlohmann::json doPutRequest(const std::string url, std::map<std::string, std::string> &body);

    nlohmann::json doDeviceGetRequest(const std::string url);
    nlohmann::json doDevicePutRequest(const std::string url, std::map<std::string, std::string> &body);

    bool hasError(nlohmann::json &response);

    bool putConnected(const bool connected);
    bool getConnected();

protected:

    // /management/v1/description
    enum ServerDescription
    {
        SERVER_NAME,
        MANUFACTURER,
        MANUFACTURER_VERSION,
        LOCATION,
        SERVER_DESCRIPTION_LEN,
    };
    INDI::PropertyText serverDescriptionTP{ServerDescription::SERVER_DESCRIPTION_LEN};

    // /management/v1/configureddevices
    enum ConfiguredDevice
    {
        DEVICE_NAME,
        DEVICE_TYPE,
        DEVICE_NUMBER,
        UNIQUE_ID,
        CONFIGURED_DEVICE_LEN,
    };
    INDI::PropertyText configuredDeviceTP{ConfiguredDevice::CONFIGURED_DEVICE_LEN};

    // /{device_type}/{device_number}/description
    enum Device
    {
        DEVICE_DESCRIPTION,
        DEVICE_LEN,
    };
    INDI::PropertyText deviceTP{Device::DEVICE_LEN};

    // /{device_type}/{device_number}/driverinfo
    enum DriverInfo
    {
        DRIVER_DESCRIPTION,
        DRIVER_INFO_LEN,
    };
    INDI::PropertyText driverInfoTP{DriverInfo::DRIVER_INFO_LEN};

    // /{device_type}/{device_number}/driverversion
    enum DriverVersion
    {
        DRIVER_VERSION,
        DRIVER_VERSION_LEN,
    };
    INDI::PropertyText driverVersionTP{DriverVersion::DRIVER_VERSION_LEN};

    // /{device_type}/{device_number}/interfaceversion
    enum InterfaceVersion
    {
        INTERFACE_VERSION,
        INTERFACE_VERSION_LEN,
    };
    INDI::PropertyNumber interfaceVersionNP{InterfaceVersion::INTERFACE_VERSION_LEN};

    // /{device_type}/{device_number}/name
    enum Name
    {
        NAME,
        NAME_LEN,
    };
    INDI::PropertyText nameTP{Name::NAME_LEN};

}; // class AlpacaBase

}; // namespace INDI

#endif // ALPACABASE_H
