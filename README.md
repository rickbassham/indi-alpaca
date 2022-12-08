# INDI Alpaca

A way to use ASCOM Alpaca devices within INDI. Supports automatic discovery of Alpaca devices.

This is still very much a work in progress, and still needs a lot of work to be useful. Think of it as a type of test to see if this is even feasible.

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

## Links

* [ASCOM Alpaca API Reference](https://raw.githubusercontent.com/ASCOMInitiative/ASCOMRemote/master/Documentation/ASCOM%20Alpaca%20API%20Reference.pdf)
* [ASCOM Alpaca REST Spec](https://ascom-standards.org/api/)
* [ASCOM Documentation](https://ascom-standards.org/Help/Developer/html/N_ASCOM_DeviceInterface.htm)
