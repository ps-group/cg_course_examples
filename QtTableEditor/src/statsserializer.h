#pragma once

#include "statskeyvaluemodel.h"

class QJsonObject;

class StatsSerializer
{
public:
    StatsSerializer(QString const& filepath);
    ~StatsSerializer();

    bool save(StatsKeyValueModel const& model);
    bool load(StatsKeyValueModel & model);

private:
    void serialize(const StatsKeyValueModel &source, QJsonObject &dest);
    void deserialize(const QJsonObject &source, StatsKeyValueModel &dest);

    const QString m_filepath;
};
