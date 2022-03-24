#include <filesystem>

#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "LogWatcher.hpp"

Q_LOGGING_CATEGORY(lwDb, "LogWatcher.DB")

Q_LOGGING_CATEGORY(lwGeneric, "LogWatcher.Generic")

namespace fs = std::filesystem;

LogWatcher::LogWatcher(QObject* parent)
    : QObject(parent),
      _watcher(new QFileSystemWatcher())
{
    _loadCache();

    connect(_watcher, &QFileSystemWatcher::fileChanged,
            this, &LogWatcher::onLogChanged);
}

LogWatcher::~LogWatcher()
{
}

bool LogWatcher::addLogPath(const QString& file)
{
    qCDebug(lwGeneric) << "adding log path" << file;

    bool success;
    success = _watcher->addPath(file);

    if (success)
    {
        // Find log open date stamp by using regex.
        // If stamp has changed, this log file is different.
        //  -> reset bookmark.

        QDateTime logOpenDt = QDateTime::fromString("", "");

        _pathToBookmark[file] = 0;
        _pathToOpenDate[file] = logOpenDt;
    }
    else
    {
        qCWarning(lwGeneric) << "error adding log path" << file;
    }

    return success;
}

bool LogWatcher::removeLogPath(const QString& file)
{
    qCDebug(lwGeneric) << "removing log path" << file;
    return _watcher->removePath(file);
}

void LogWatcher::onLogChanged(const QString& path)
{
    qCDebug(lwGeneric) << "log" << path << "changed";

    if (!_watcher->files().contains(path))
    {
        if (fs::exists(path.toStdString()))
        {
            qCDebug(lwGeneric) << "re-adding log path" << path;
            _watcher->addPath(path);
        }
        else
        {
            qCDebug(lwGeneric) << "not re-adding log path"
                               << path << "as it does not exist anymore";
        }
    }

    // Parse log continuing from bookmark, check if there are
    // IP addresses that are not associated with Steam IDs.
    emit nonPlayerAddressFound("");
}

bool LogWatcher::_loadCache()
{
    qCInfo(lwDb) << "loading cache";

    if (!QSqlDatabase::isDriverAvailable(_dbDriver))
    {
        qCWarning(lwDb) << "driver" << _dbDriver << "is not available";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(_dbDriver);
    db.setDatabaseName("rs2autoban.db");

    if (!db.open())
    {
        qCWarning(lwDb) << "cannot open database";
        return false;
    }

    QSqlQuery query(db);
    query.exec("PRAGMA schema.page_count");
    if (query.isActive())
    {
        while (query.next())
        {
            qCInfo(lwDb) << query.value(0).toString();
        }
    }

    query.exec("PRAGMA schema.page_size");
    if (query.isActive())
    {
        while (query.next())
        {
            qCInfo(lwDb) << query.value(0).toString();
        }
    }

    return true;
}
