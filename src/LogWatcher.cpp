#include <filesystem>

#include <QTimer>
#include <QSqlDatabase>

#include "LogWatcher.hpp"

Q_LOGGING_CATEGORY(lwDb, "LogWatcher.DB")

Q_LOGGING_CATEGORY(lwGeneric, "LogWatcher.Generic")

namespace fs = std::filesystem;

LogWatcher::LogWatcher(QObject* parent)
    : QObject(parent),
      _watcher(new QFileSystemWatcher()),
      _pruneTimer(new QTimer(this))
{
    if (!QSqlDatabase::isDriverAvailable(_dbDriver))
    {
        qCWarning(lwDb) << "driver" << _dbDriver
                        << "is not available";
    }
    // Load cached bookmark & log open date from database.

    connect(_watcher, &QFileSystemWatcher::fileChanged,
            this, &LogWatcher::onLogChanged);

    _pruneTimer->start(60 * 1000);
}

LogWatcher::~LogWatcher()
{
    delete _watcher;
    delete _pruneTimer;
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
