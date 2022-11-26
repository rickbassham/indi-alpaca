
#include <libindi/defaultdevice.h>
#include <libindi/indiccd.h>

namespace INDI
{
/**
 * @brief The AlpacaCamera class.
 *
 * @author Rick Bassham
 */
class AlpacaCamera : public INDI::CCD
{
public:
    AlpacaCamera(std::string ipAddress);
    virtual ~AlpacaCamera() = default;

    void ISGetProperties(const char *dev) override;

    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;
    virtual bool ISSnoopDevice(XMLEle *root) override;

protected:
    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    const char *getDefaultName() override;
    virtual bool saveConfigItems(FILE *fp) override;

    virtual bool Connect() override;
    virtual bool Disconnect() override;
    void TimerHit() override;
private:
    std::string _ipAddress;
    uint16_t _port;
}; // class AlpacaCamera

}; // namespace INDI
