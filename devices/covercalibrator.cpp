#include "config.h"
#include "covercalibrator.h"

using namespace INDI;


AlpacaCoverCalibrator::AlpacaCoverCalibrator(
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
    : AlpacaBase(
          this,
          serverName,
          manufacturer,
          manufacturerVersion,
          location,
          deviceName,
          deviceType,
          deviceNumber,
          uniqueId,
          ipAddress,
          port
      ), LightBoxInterface(this, true)
{
    setVersion(VERSION_MAJOR, VERSION_MINOR);
}

void AlpacaCoverCalibrator::ISGetProperties(const char *dev)
{
    DefaultDevice::ISGetProperties(dev);

    // Get Light box properties
    isGetLightBoxProperties(dev);
}

bool AlpacaCoverCalibrator::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (processLightBoxNumber(dev, name, values, names, n))
    {
        return true;
    }

    return DefaultDevice::ISNewNumber(dev, name, values, names, n);
}

bool AlpacaCoverCalibrator::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (processLightBoxSwitch(dev, name, states, names, n))
    {
        return true;
    }

    if (processDustCapSwitch(dev, name, states, names, n))
    {
        return true;
    }

    return DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

bool AlpacaCoverCalibrator::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (processLightBoxText(dev, name, texts, names, n))
    {
        return true;
    }

    return DefaultDevice::ISNewText(dev, name, texts, names, n);
}

bool AlpacaCoverCalibrator::ISSnoopDevice(XMLEle *root)
{
    snoopLightBox(root);

    return DefaultDevice::ISSnoopDevice(root);
}

bool AlpacaCoverCalibrator::initProperties()
{
    INDI::DefaultDevice::initProperties();

    initAlpacaBaseProperties();

    initLightBoxProperties(getDeviceName(), MAIN_CONTROL_TAB);
    initDustCapProperties(getDeviceName(), MAIN_CONTROL_TAB);

    addAuxControls();

    return true;
}

bool AlpacaCoverCalibrator::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    if (isConnected())
    {
        uint16_t interface = BaseDevice::AUX_INTERFACE;

        if (supportsDustCap())
        {
            defineProperty(&ParkCapSP);
            interface |= BaseDevice::DUSTCAP_INTERFACE;
        }

        if (supportsLightBox())
        {
            defineProperty(&LightSP);
            defineProperty(&LightIntensityNP);
            interface |= BaseDevice::LIGHTBOX_INTERFACE;
        }

        setDriverInterface(interface);
        syncDriverInfo();
    }
    else
    {
        deleteProperty(ParkCapSP.name);
        deleteProperty(LightSP.name);
        deleteProperty(LightIntensityNP.name);
    }

    updateLightBoxProperties();

    return true;
}

bool AlpacaCoverCalibrator::saveConfigItems(FILE *fp)
{
    DefaultDevice::saveConfigItems(fp);

    return true;
}

int AlpacaCoverCalibrator::getBrightness()
{
    nlohmann::json response = doDeviceGetRequest("/brightness");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsLightBox = false;
        }

        return -1;
    }

    int brightness = response["Value"].get<int>();

    _supportsLightBox = true;

    return brightness;
}

AlpacaCoverCalibrator::AlpacaCalibratorStatus AlpacaCoverCalibrator::getCalibratorState()
{
    nlohmann::json response = doDeviceGetRequest("/calibratorstate");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsLightBox = false;
        }

        return Calibrator_Error;
    }

    AlpacaCalibratorStatus status = static_cast<AlpacaCalibratorStatus>(response["Value"].get<int>());

    if (status == Calibrator_NotPresent)
    {
        _supportsLightBox = false;
        return Calibrator_Error;
    }

    _supportsLightBox = true;

    return status;
}

AlpacaCoverCalibrator::AlpacaCoverStatus AlpacaCoverCalibrator::getCoverState()
{
    nlohmann::json response = doDeviceGetRequest("/coverstate");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsDustCap = false;
        }

        return Cover_Error;
    }

    AlpacaCoverStatus status = static_cast<AlpacaCoverStatus>(response["Value"].get<int>());

    if (status == Cover_NotPresent)
    {
        _supportsDustCap = false;
        return Cover_Error;
    }

    _supportsDustCap = true;

    return status;
}

int AlpacaCoverCalibrator::getMaxBrightness()
{
    nlohmann::json response = doDeviceGetRequest("/maxbrightness");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsLightBox = false;
        }

        return -1;
    }

    int maxBrightness = response["Value"].get<int>();

    _supportsLightBox = true;

    return maxBrightness;
}

bool AlpacaCoverCalibrator::putCalibratorOff()
{
    if (!_supportsLightBox)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/calibratoroff", body);

    if (hasError(response))
        return false;

    return true;
}

bool AlpacaCoverCalibrator::putCalibratorOn()
{
    if (!_supportsLightBox)
        return false;

    std::map<std::string, std::string> body;

    body["Brightness"] = std::to_string(int32_t(LightIntensityN[0].value));

    auto response = doDevicePutRequest("/calibratoron", body);

    if (hasError(response))
        return false;

    return true;
}

bool AlpacaCoverCalibrator::putCloseCover()
{
    if (!_supportsDustCap)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/closecover", body);

    if (hasError(response))
        return false;

    return true;
}

bool AlpacaCoverCalibrator::putHaltCover()
{
    if (!_supportsDustCap)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/haltcover", body);

    if (hasError(response))
        return false;

    return true;
}

bool AlpacaCoverCalibrator::putOpenCover()
{
    if (!_supportsDustCap)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/opencover", body);

    if (hasError(response))
        return false;

    return true;
}

bool AlpacaCoverCalibrator::Connect()
{
    bool rc = putConnected(true);

    if (rc)
        SetTimer(POLLMS);

    return rc;
}

bool AlpacaCoverCalibrator::Disconnect()
{
    return putConnected(false);
}

void AlpacaCoverCalibrator::TimerHit()
{
    if (!isConnected())
        return;

    if (_supportsDustCap)
    {
        AlpacaCoverStatus status = getCoverState();

        switch (status)
        {
        case Cover_Closed:
            ParkCapSP.s = IPS_IDLE;
            IUResetSwitch(&ParkCapSP);
            ParkCapS[CAP_PARK].s = ISS_ON;
            break;

        case Cover_Open:
            ParkCapSP.s = IPS_IDLE;
            IUResetSwitch(&ParkCapSP);
            ParkCapS[CAP_UNPARK].s = ISS_ON;
            break;

        case Cover_Moving:
            ParkCapSP.s = IPS_BUSY;
            break;

        default:
            ParkCapSP.s = IPS_ALERT;
            break;
        }

        IDSetSwitch(&ParkCapSP, nullptr);
    }

    if (_supportsLightBox)
    {
        AlpacaCalibratorStatus status = getCalibratorState();

        switch (status)
        {
        case Calibrator_Off:
            LightSP.s = IPS_IDLE;
            IUResetSwitch(&LightSP);
            LightS[FLAT_LIGHT_OFF].s = ISS_ON;
            break;

        case Calibrator_NotReady:
            LightSP.s = IPS_BUSY;
            IUResetSwitch(&LightSP);
            LightS[FLAT_LIGHT_ON].s = ISS_ON;
            break;

        case Calibrator_Ready:
            LightSP.s = IPS_OK;
            IUResetSwitch(&LightSP);
            LightS[FLAT_LIGHT_ON].s = ISS_ON;
            break;

        default:
            LightSP.s = IPS_ALERT;
            break;
        }

        IDSetSwitch(&LightSP, nullptr);
    }

    if (_supportsLightBox)
    {
        int brightness = getBrightness();

        if (LightS[FLAT_LIGHT_ON].s == ISS_ON)
        {
            LightIntensityN[0].value = brightness;
            IDSetNumber(&LightIntensityNP, nullptr);
        }
    }

    SetTimer(POLLMS);
}

const char *AlpacaCoverCalibrator::getDefaultName()
{
    return _deviceName.c_str();
}

bool AlpacaCoverCalibrator::supportsLightBox()
{
    getCalibratorState();
    return _supportsLightBox;
}

bool AlpacaCoverCalibrator::supportsDustCap()
{
    getCoverState();
    return _supportsDustCap;
}

bool AlpacaCoverCalibrator::EnableLightBox(bool enable)
{
    if (enable)
    {
        return putCalibratorOn();
    }
    else
    {
        return putCalibratorOff();
    }
}

bool AlpacaCoverCalibrator::SetLightBoxBrightness(uint16_t value)
{
    if (LightS[FLAT_LIGHT_ON].s == ISS_ON)
    {
        return putCalibratorOn();
    }

    return true;
}

IPState AlpacaCoverCalibrator::ParkCap()
{
    if (putCloseCover())
    {
        return IPS_BUSY;
    }

    return IPS_ALERT;
}

IPState AlpacaCoverCalibrator::UnParkCap()
{
    if (putOpenCover())
    {
        return IPS_BUSY;
    }

    return IPS_ALERT;
}
