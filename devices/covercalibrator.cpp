#include "covercalibrator.h"

using namespace INDI;


CoverCalibrator::CoverCalibrator(
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
}

void CoverCalibrator::ISGetProperties(const char *dev)
{
    AlpacaBase::ISGetProperties(dev);

    // Get Light box properties
    isGetLightBoxProperties(dev);
}

bool CoverCalibrator::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (processLightBoxNumber(dev, name, values, names, n))
    {
        return true;
    }

    return AlpacaBase::ISNewNumber(dev, name, values, names, n);
}

bool CoverCalibrator::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (processLightBoxSwitch(dev, name, states, names, n))
    {
        return true;
    }

    return AlpacaBase::ISNewSwitch(dev, name, states, names, n);
}

bool CoverCalibrator::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (processLightBoxText(dev, name, texts, names, n))
    {
        return true;
    }

    return AlpacaBase::ISNewText(dev, name, texts, names, n);
}

bool CoverCalibrator::ISSnoopDevice(XMLEle *root)
{
    snoopLightBox(root);

    return AlpacaBase::ISSnoopDevice(root);
}

bool CoverCalibrator::initProperties()
{
    AlpacaBase::initProperties();

    initLightBoxProperties(getDeviceName(), MAIN_CONTROL_TAB);
    initDustCapProperties(getDeviceName(), MAIN_CONTROL_TAB);

    return true;
}

bool CoverCalibrator::updateProperties()
{
    AlpacaBase::updateProperties();

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

        IDLog("Interface: %d\n", interface);
        IDLog("Expected Interface: %d\n", BaseDevice::AUX_INTERFACE | BaseDevice::DUSTCAP_INTERFACE | BaseDevice::LIGHTBOX_INTERFACE);

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

int CoverCalibrator::getBrightness()
{
    nlohmann::json response = doDeviceGetRequest("/brightness");

    IDLog("getBrightness\n");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsLightBox = false;
        }

        return -1;
    }

    int brightness = response["Value"].get<int>();

    IDLog("Brightness: %d\n", brightness);

    _supportsLightBox = true;

    return brightness;
}

CoverCalibrator::AlpacaCalibratorStatus CoverCalibrator::getCalibratorState()
{
    nlohmann::json response = doDeviceGetRequest("/calibratorstate");

    IDLog("getCalibratorState\n");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsLightBox = false;
        }

        return Calibrator_Error;
    }

    AlpacaCalibratorStatus status = static_cast<AlpacaCalibratorStatus>(response["Value"].get<int>());

    IDLog("Calibrator state: %d\n", status);

    if (status == Calibrator_NotPresent)
    {
        _supportsLightBox = false;
        return Calibrator_Error;
    }

    _supportsLightBox = true;

    return status;
}

CoverCalibrator::AlpacaCoverStatus CoverCalibrator::getCoverState()
{
    nlohmann::json response = doDeviceGetRequest("/coverstate");

    IDLog("getCoverState\n");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsDustCap = false;
        }

        return Cover_Error;
    }

    AlpacaCoverStatus status = static_cast<AlpacaCoverStatus>(response["Value"].get<int>());

    IDLog("Cover state: %d\n", status);

    if (status == Cover_NotPresent)
    {
        _supportsDustCap = false;
        return Cover_Error;
    }

    _supportsDustCap = true;

    return status;
}

int CoverCalibrator::getMaxBrightness()
{
    nlohmann::json response = doDeviceGetRequest("/maxbrightness");

    IDLog("getMaxBrightness\n");

    if (hasError(response))
    {
        if (response["ErrorNumber"] == ALPACA_ERROR_NOT_IMPLEMENTED)
        {
            _supportsLightBox = false;
        }

        return -1;
    }

    int maxBrightness = response["Value"].get<int>();

    IDLog("Max Brightness: %d\n", maxBrightness);

    _supportsLightBox = true;

    return maxBrightness;
}

bool CoverCalibrator::putCalibratorOff()
{
    if (!_supportsLightBox)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/calibratoroff", body);

    if (hasError(response))
        return false;

    return true;
}

bool CoverCalibrator::putCalibratorOn()
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

bool CoverCalibrator::putCloseCover()
{
    if (!_supportsDustCap)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/closecover", body);

    if (hasError(response))
        return false;

    return true;
}

bool CoverCalibrator::putHaltCover()
{
    if (!_supportsDustCap)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/haltcover", body);

    if (hasError(response))
        return false;

    return true;
}

bool CoverCalibrator::putOpenCover()
{
    if (!_supportsDustCap)
        return false;

    std::map<std::string, std::string> body;
    auto response = doDevicePutRequest("/opencover", body);

    if (hasError(response))
        return false;

    return true;
}

bool CoverCalibrator::Connect()
{
    if (!AlpacaBase::Connect())
        return false;

    return true;
}

bool CoverCalibrator::Disconnect()
{
    if (!AlpacaBase::Disconnect())
        return false;

    return true;
}

void CoverCalibrator::TimerHit()
{
    AlpacaBase::TimerHit();

    IDLog("CoverCalibrator::TimerHit\n");

    if (_supportsDustCap)
    {
        AlpacaCoverStatus status = getCoverState();

        IDLog("Cover state: %d\n", status);

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
            ParkCapS[CAP_PARK].s = ISS_ON;
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

        IDLog("Calibrator state: %d\n", status);

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

        IDLog("Brightness: %d\n", brightness);

        if (LightS[FLAT_LIGHT_ON].s == ISS_ON)
        {
            LightIntensityN[0].value = brightness;
            IDSetNumber(&LightIntensityNP, nullptr);
        }
    }
}

bool CoverCalibrator::supportsLightBox()
{
    getCalibratorState();
    return _supportsLightBox;
}

bool CoverCalibrator::supportsDustCap()
{
    getCoverState();
    return _supportsDustCap;
}

bool CoverCalibrator::EnableLightBox(bool enable)
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

bool CoverCalibrator::SetLightBoxBrightness(uint16_t value)
{
    uint16_t original = (uint16_t)LightIntensityN[0].value;

    if (value == original)
        return true;

    LightIntensityN[0].value = value;
    IDSetNumber(&LightIntensityNP, nullptr);

    if (LightS[FLAT_LIGHT_ON].s == ISS_ON)
    {
        return putCalibratorOn();
    }

    return true;
}

IPState CoverCalibrator::ParkCap()
{
    if (putCloseCover())
    {
        return IPS_BUSY;
    }

    return IPS_ALERT;
}

IPState CoverCalibrator::UnParkCap()
{
    if (putOpenCover())
    {
        return IPS_BUSY;
    }

    return IPS_ALERT;
}
