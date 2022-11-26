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
      )
{
}