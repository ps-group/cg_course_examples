#include "statsserializer.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <cstdio>
#include <cassert>

static const QLatin1String KEY_VALUES("values");

StatsSerializer::StatsSerializer(const QString &filepath)
    : m_filepath(filepath)
{
}

StatsSerializer::~StatsSerializer()
{
}

bool StatsSerializer::save(StatsKeyValueModel const& model)
{
    if (FILE *file = std::fopen(m_filepath.toLocal8Bit(), "w"))
    {
        try
        {
            QJsonObject root;
            serialize(model, root);
            QJsonDocument document(root);
            QByteArray bytes = document.toJson();
            std::fwrite(bytes.data(), sizeof(char), bytes.size(), file);
        }
        catch (...)
        {
            std::fclose(file);
            throw;
        }
        std::fclose(file);
    }
    else
    {
        assert(!"save() failed: cannot open file.");
        return false;
    }
    return true;
}

bool StatsSerializer::load(StatsKeyValueModel &model)
{
    bool hasErrors = false;
    if (FILE *file = std::fopen(m_filepath.toLocal8Bit(), "r"))
    {
        try
        {
            QByteArray bytes;
            const size_t BUFFER_SIZE = 64 * 1024;
            char buffer[BUFFER_SIZE];
            while (size_t readCount = std::fread(buffer, sizeof(char), BUFFER_SIZE, file))
            {
                bytes.append(buffer, readCount);
            }

            QJsonParseError parseError;
            QJsonDocument document = QJsonDocument::fromJson(bytes, &parseError);
            if (parseError.error == QJsonParseError::NoError)
            {
                deserialize(document.object(), model);
            }
            else
            {
                assert(!"load() failed: cannot parse JSON.");
                hasErrors = true;
            }
        }
        catch (...)
        {
            std::fclose(file);
            throw;
        }
        std::fclose(file);
    }
    else
    {
        assert(!"load() failed: cannot open file.");
        hasErrors = true;
    }
    return !hasErrors;
}

void StatsSerializer::serialize(const StatsKeyValueModel &source, QJsonObject &dest)
{
    QJsonArray values;
    for (auto pair : source)
    {
        QJsonObject pairObj;
        pairObj[pair.first] = pair.second;
        values.append(pairObj);
    }
    dest[KEY_VALUES] = values;
}

void StatsSerializer::deserialize(const QJsonObject &source, StatsKeyValueModel &dest)
{
    QJsonArray values = source[KEY_VALUES].toArray();
    for (QJsonValue const& value : values)
    {
        QJsonObject pairObj = value.toObject();
        QStringList keys = pairObj.keys();
        if (keys.size() != 1)
        {
            assert(!"deserialize error: incorrect JSON format");
        }
        else
        {
            QString const& key = keys[0];
            dest.append(key, pairObj[key].toInt());
        }
    }
}
