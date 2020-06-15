#pragma once

#ifndef RS2AUTOBAN_LOGWATCHER_HPP
#define RS2AUTOBAN_LOGWATCHER_HPP

#include <cinttypes>

#include <QObject>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lwDb)

Q_DECLARE_LOGGING_CATEGORY(lwGeneric)

class LogWatcher : public QObject
{
Q_OBJECT

public:

    explicit LogWatcher(QObject* parent = nullptr);

    ~LogWatcher() override;

    bool addLogPath(const QString& file);

    bool removeLogPath(const QString& file);

signals:

    void nonPlayerAddressFound(const QString& address);

private slots:

    void onLogChanged(const QString& path);

private:
    bool _loadCache();

    QFileSystemWatcher* _watcher;
    QMap<QString, uint64_t> _pathToBookmark{};
    QMap<QString, QDateTime> _pathToOpenDate{};
    const QString _dbDriver{"QSQLITE"};
};

#endif //RS2AUTOBAN_LOGWATCHER_HPP
