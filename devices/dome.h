#pragma once
#ifndef DOME_H
#define DOME_H

#include "base.h"
#include <libindi/indidome.h>


namespace INDI
{
class AlpacaDome : public INDI::Dome, public AlpacaBase
{
public:
    AlpacaDome(
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
    virtual ~AlpacaDome() = default;

    virtual bool initProperties() override;
    const char *getDefaultName() override;
    bool updateProperties() override;

protected:
    bool Connect() override;
    bool Disconnect() override;

    void TimerHit() override;

    virtual IPState Move(DomeDirection dir, DomeMotionCommand operation) override;
    virtual IPState MoveRel(double azDiff) override;
    virtual IPState MoveAbs(double az) override;
    virtual IPState Park() override;
    virtual IPState UnPark() override;
    virtual IPState ControlShutter(ShutterOperation operation) override;
    virtual bool Abort() override;

    // Parking
    virtual bool SetCurrentPark() override;
    virtual bool SetDefaultPark() override;

};
}

#endif // DOME_H
