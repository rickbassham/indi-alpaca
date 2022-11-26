#include "base.h"
#include "config.h"

using namespace INDI;

AlpacaBase::AlpacaBase()
    : DefaultDevice()
{
    setVersion(VERSION_MAJOR, VERSION_MINOR);
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

    serverDescriptionTP[ServerDescription::SERVER_NAME].fill("SERVER_NAME", "Server Name", "");
    serverDescriptionTP[ServerDescription::MANUFACTURER].fill("MANUFACTURER", "Manufacturer", "");
    serverDescriptionTP[ServerDescription::MANUFACTURER_VERSION].fill("MANUFACTURER_VERSION", "Manufacturer Version", "");
    serverDescriptionTP[ServerDescription::LOCATION].fill("LOCATION", "Location", "");
    serverDescriptionTP.fill(getDeviceName(), "SERVER_DESCRIPTION", "Server Description", "Server Description", IP_RO, 60, IPS_IDLE);
    registerProperty(serverDescriptionTP);

    configuredDeviceTP[ConfiguredDevice::DEVICE_NAME].fill("DEVICE_NAME", "Device Name", "");
    configuredDeviceTP[ConfiguredDevice::DEVICE_TYPE].fill("DEVICE_TYPE", "Device Type", "");
    configuredDeviceTP[ConfiguredDevice::DEVICE_NUMBER].fill("DEVICE_NUMBER", "Device Number", "");
    configuredDeviceTP[ConfiguredDevice::UNIQUE_ID].fill("UNIQUE_ID", "Unique ID", "");
    configuredDeviceTP.fill(getDeviceName(), "CONFIGURED_DEVICE", "Configured Device", "Configured Device", IP_RO, 60, IPS_IDLE);
    registerProperty(configuredDeviceTP);

    deviceTP[Device::DEVICE_DESCRIPTION].fill("DEVICE_DESCRIPTION", "Device Description", "");
    deviceTP.fill(getDeviceName(), "DEVICE", "Device", "Device", IP_RO, 60, IPS_IDLE);
    registerProperty(deviceTP);

    driverInfoTP[DriverInfo::DRIVER_DESCRIPTION].fill("DRIVER_DESCRIPTION", "Driver Description", "");
    driverInfoTP.fill(getDeviceName(), "DRIVER_INFO", "Driver Info", "Driver Description", IP_RO, 60, IPS_IDLE);
    registerProperty(driverInfoTP);

    driverVersionTP[DriverVersion::DRIVER_VERSION].fill("DRIVER_VERSION", "Driver Version", "");
    driverVersionTP.fill(getDeviceName(), "DRIVER_VERSION", "Driver Version", "Driver Version", IP_RO, 60, IPS_IDLE);
    registerProperty(driverVersionTP);

    interfaceVersionNP[InterfaceVersion::INTERFACE_VERSION].fill("INTERFACE_VERSION", "Interface Version", "%d", 0, 0, 0, 0);
    interfaceVersionNP.fill(getDeviceName(), "INTERFACE_VERSION", "Interface Version", "Interface Version", IP_RO, 60, IPS_IDLE);
    registerProperty(interfaceVersionNP);

    nameTP[Name::NAME].fill("NAME", "Name", "");
    nameTP.fill(getDeviceName(), "NAME", "Name", "Name", IP_RO, 60, IPS_IDLE);
    registerProperty(nameTP);

    addAuxControls();

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
