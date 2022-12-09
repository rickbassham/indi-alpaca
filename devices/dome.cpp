#include "config.h"
#include "dome.h"

using namespace INDI;

AlpacaDome::AlpacaDome(
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
    : Dome(), AlpacaBase(
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
      )
{
    setVersion(VERSION_MAJOR, VERSION_MINOR);
}

bool AlpacaDome::initProperties()
{
    INDI::Dome::initProperties();
    initAlpacaBaseProperties();
    addAuxControls();

    return true;
}

const char *AlpacaDome::getDefaultName()
{
    return _deviceName.c_str();
}

bool AlpacaDome::updateProperties()
{
    return INDI::Dome::updateProperties();
}

bool AlpacaDome::Connect()
{
    bool rc = putConnected(true);

    if (rc)
        SetTimer(POLLMS);

    return rc;
}

bool AlpacaDome::Disconnect()
{
    return putConnected(false);
}

void AlpacaDome::TimerHit()
{
    if (!isConnected())
        return;

}

IPState AlpacaDome::Move(DomeDirection dir, DomeMotionCommand operation)
{
    return IPS_ALERT;
}

IPState AlpacaDome::MoveRel(double azDiff)
{
    return IPS_ALERT;
}

IPState AlpacaDome::MoveAbs(double az)
{
    return IPS_ALERT;
}

IPState AlpacaDome::Park()
{
    return IPS_ALERT;
}

IPState AlpacaDome::UnPark()
{
    return IPS_ALERT;
}

IPState AlpacaDome::ControlShutter(ShutterOperation operation)
{
    return IPS_ALERT;
}

bool AlpacaDome::Abort()
{
    return false;
}

bool AlpacaDome::SetCurrentPark()
{
    return false;
}

bool AlpacaDome::SetDefaultPark()
{
    return false;
}
