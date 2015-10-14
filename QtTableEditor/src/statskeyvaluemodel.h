#pragma once

#include "basekeyvaluemodel.h"

class StatsKeyValueModel : public BaseKeyValueModel<QString, int>
{
public:
    StatsKeyValueModel();

    void setSampleValues();
};
