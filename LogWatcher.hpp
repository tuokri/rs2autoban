#pragma once

#ifndef RS2AUTOBAN_LOGWATCHER_HPP
#define RS2AUTOBAN_LOGWATCHER_HPP

#include <cinttypes>

#include <QObject>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QFileSystemWatcher>

class LogWatcher : public QObject
{
Q_OBJECT

public:

    LogWatcher();

    bool addLogPath(const QString& file);

    bool removeLogPath(const QString& file);

signals:

    void nonPlayerAddressFound(const QString& address);

private slots:

    void onLogChanged(const QString& path);

private:
    QFileSystemWatcher* _watcher;
    QMap<QString, uint64_t> _pathToBookmark;
    QMap<QString, QDateTime> _pathToOpenDate;
};

#endif //RS2AUTOBAN_LOGWATCHER_HPP
