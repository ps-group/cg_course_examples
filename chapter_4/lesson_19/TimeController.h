#pragma once

class ITimeController
{
public:
    virtual ~ITimeController() = default;
    virtual double GetSpaceTime()const = 0;
};

class CTimeController : public ITimeController
{
public:
    // ITimeController interface
    double GetSpaceTime() const override;

    void Update(float deltaSeconds);

private:
    static const double DEFAULT_TIME_SPEED;

    double m_spaceTime = 0;
    double m_timeSpeed = DEFAULT_TIME_SPEED;
};
