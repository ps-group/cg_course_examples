#pragma once

class StatsKeyValueModel;

class IStatsModelProvider
{
public:
    virtual ~IStatsModelProvider() = default;

    virtual const StatsKeyValueModel &statsModel() const = 0;
    virtual void setStatsModel(StatsKeyValueModel const& model) = 0;
    virtual bool isSaved() const = 0;
    virtual void setIsSaved() = 0;
};
