#pragma once

#include <QObject>
#include <memory>
#include "istatsmodelprovider.h"

class QWidget;
class IStatsModelProvider;

class StatsDocument : public QObject
{
    Q_OBJECT
public:
    explicit StatsDocument(QWidget *parent, IStatsModelProvider &provider);

    void createNew();
    bool open();
    bool save();
    bool saveAs();

private:
    QString selectSavePath() const;
    QString selectOpenPath() const;

    IStatsModelProvider &m_provider;
};
