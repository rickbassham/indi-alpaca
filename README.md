# INDI Alpaca

A way to use ASCOM Alpaca devices within INDI. Supports automatic discovery of Alpaca devices.

## How It Works

When the indi_alpaca driver is started, it broadcasts a UDP packet with the message `alpacadiscovery1` and listens for responses from Alpaca devices on the network. For each device it finds, we create a corresponding INDI device.

## Currently Supported ASCOM Device Types

* CoverCalibrator
    * This maps to the `LightBoxInterface` and `DustCapInterface`.

## ASCOM Device Types Not Supported Yet

* Camera
* Dome
* FilterWheel
* Focuser
* ObservingConditions
* Rotator
* SafetyMonitor
* Switch
* Telescope

## Build

```
make clean && make build
```
