#pragma once
#ifndef COVERCALIBRATOR_H
#define COVERCALIBRATOR_H

#include "base.h"
#include <libindi/indilightboxinterface.h>
#include <libindi/indidustcapinterface.h>


namespace INDI
{
class CoverCalibrator : public AlpacaBase
{
public:
    CoverCalibrator(
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
    virtual ~CoverCalibrator() = default;

}; // class CoverCalibrator

}; // namespace INDI

#endif // COVERCALIBRATOR_H
