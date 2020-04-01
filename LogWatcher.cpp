#include <filesystem>

#include "LogWatcher.hpp"

namespace fs = std::filesystem;

LogWatcher::LogWatcher() : _watcher(new QFileSystemWatcher{})
{
    // @formatter:off
    connect(_watcher, SIGNAL(pathChanged(const QString&)),
            this, SLOT(onLogChanged(const QString&)));
    // @formatter:on

    // Load cached modifications from database.
}

bool LogWatcher::addLogPath(const QString& file)
{
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
    return success;
}

bool LogWatcher::removeLogPath(const QString& file)
{
    return _watcher->removePath(file);
}

void LogWatcher::onLogChanged(const QString& path)
{
    if (!_watcher->files().contains(path))
    {
        if (fs::exists(path.toStdString()))
        {
            _watcher->addPath(path);
        }
    }

    // parse log, check if there are
    emit nonPlayerAddressFound("");
}
