#include "statsdocument.h"
#include "statskeyvaluemodel.h"
#include "istatsmodelprovider.h"
#include "statsserializer.h"
#include <QWidget>
#include <QFileDialog>
#include <QStandardPaths>

namespace
{
const QLatin1String FILE_FORMAT_FILTER("*.json");
}

StatsDocument::StatsDocument(QWidget *parent, IStatsModelProvider &provider)
    : QObject(parent)
    , m_provider(provider)
{

}

void StatsDocument::createNew()
{
    StatsKeyValueModel model;
    m_provider.setStatsModel(model);
}

bool StatsDocument::open()
{
    QString openPath = selectOpenPath();
    if (openPath.isEmpty())
    {
        return false;
    }

    StatsKeyValueModel model;
    StatsSerializer serializer(openPath);
    if (!serializer.load(model))
    {
        return false;
    }

    m_provider.setStatsModel(model);
    return true;
}

bool StatsDocument::save()
{
    // TODO: implement me.
    return false;
}

bool StatsDocument::saveAs()
{
    QString savePath = selectSavePath();
    if (savePath.isEmpty())
    {
        return false;
    }
    StatsSerializer serializer(savePath);
    return serializer.save(m_provider.statsModel());
}

QString StatsDocument::selectSavePath() const
{
    QString fromDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    auto parentWindow = qobject_cast<QWidget *>(parent());
    return QFileDialog::getSaveFileName(parentWindow, QString(), fromDir, FILE_FORMAT_FILTER);
}

QString StatsDocument::selectOpenPath() const
{
    QString fromDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    auto parentWindow = qobject_cast<QWidget *>(parent());
    return QFileDialog::getOpenFileName(parentWindow, QString(), fromDir, FILE_FORMAT_FILTER);
}
