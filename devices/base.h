
#include <libindi/defaultdevice.h>

namespace INDI
{
/**
 * @brief The AlpacaBase class.
 *
 * @author Rick Bassham
 */
class AlpacaBase : public DefaultDevice
{
public:
    AlpacaBase();
    virtual ~AlpacaBase() = default;

    void ISGetProperties(const char *dev) override;

    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;
    virtual bool ISSnoopDevice(XMLEle *root) override;

protected:
    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    const char *getDefaultName() override;
    virtual bool saveConfigItems(FILE *fp) override;

    virtual bool Connect() override;
    virtual bool Disconnect() override;
    void TimerHit() override;

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
