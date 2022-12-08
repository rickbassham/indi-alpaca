#pragma once
#ifndef COVERCALIBRATOR_H
#define COVERCALIBRATOR_H

#include "base.h"
#include <libindi/indilightboxinterface.h>
#include <libindi/indidustcapinterface.h>


namespace INDI
{
class CoverCalibrator : public AlpacaBase, public DustCapInterface, public LightBoxInterface
{
public:
    enum AlpacaCoverStatus
    {
        Cover_NotPresent = 0,
        Cover_Closed = 1,
        Cover_Moving = 2,
        Cover_Open = 3,
        Cover_Unknown = 4,
        Cover_Error = 5,
    };

    enum AlpacaCalibratorStatus
    {
        Calibrator_NotPresent = 0,
        Calibrator_Off = 1,
        Calibrator_NotReady = 2,
        Calibrator_Ready = 3,
        Calibrator_Unknown = 4,
        Calibrator_Error = 5,
    };


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

protected:
    virtual bool Connect() override;
    virtual bool Disconnect() override;
    void TimerHit() override;

    virtual bool initProperties() override;
    virtual bool updateProperties() override;

protected:
    virtual bool SetLightBoxBrightness(uint16_t value) override;
    virtual bool EnableLightBox(bool enable) override;

    virtual IPState ParkCap() override;
    virtual IPState UnParkCap() override;


private:
    int getBrightness();
    AlpacaCalibratorStatus getCalibratorState();
    AlpacaCoverStatus getCoverState();
    int getMaxBrightness();

    bool putCalibratorOff();
    bool putCalibratorOn();
    bool putCloseCover();
    bool putHaltCover();
    bool putOpenCover();

    bool supportsLightBox();
    bool supportsDustCap();

    bool _supportsLightBox;
    bool _supportsDustCap;

}; // class CoverCalibrator

}; // namespace INDI

#endif // COVERCALIBRATOR_H
