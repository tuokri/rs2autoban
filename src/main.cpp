#include <cinttypes>
#include <csignal>

#include <io.h>
#include <fcntl.h>

#include <QCoreApplication>
#include <QFileSystemWatcher>
#include <QCommandLineParser>
#include <QLoggingCategory>
#include <QtGlobal>

// TODO: Rethink these categories...
Q_LOGGING_CATEGORY(mainInfo, "Main.Info", QtInfoMsg)

Q_LOGGING_CATEGORY(mainCritical, "Main.Critical", QtCriticalMsg)

Q_LOGGING_CATEGORY(mainWarn, "Main.Warning", QtWarningMsg)

Q_LOGGING_CATEGORY(mainDebug, "Main.Debug", QtDebugMsg)

#include "Firewall.hpp"
#include "LogWatcher.hpp"

Firewall::Manager* manager = nullptr;

int __cdecl wmain(int argc, char* argv[])
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    QCoreApplication a(argc, argv);

    // std::signal(SIGINT, sigHandler);
    // std::signal(SIGTERM, sigHandler);

    QCoreApplication::setApplicationVersion("0.0.1");
    QCoreApplication::setApplicationName("RS2Autoban");

    qSetMessagePattern("%{time} %{category}: %{message}");

    QCommandLineParser parser;

    parser.setApplicationDescription(
        "Automatic banning of malicious IP addresses "
        "from Rising Storm 2: Vietnam dedicated server logs.");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption logOption{
        {"l", "log"},
        "server log file path to watch",
        "log"};
    parser.addOption(logOption);

    QCommandLineOption ttlOption{
        {"t", "ttl"},
        "time to live for block rules in seconds",
        "ttl"};
    parser.addOption(ttlOption);

    QCommandLineOption gracePeriodOption{
        {"g", "grace-period"},
        "grace period after finding new address in seconds",
        "gracePeriod"};
    parser.addOption(gracePeriodOption);

    parser.process(a);

    QStringList logs;
    if (parser.isSet(logOption))
    {
        qCDebug(mainDebug) << "logOption is set";
        logs = parser.values(logOption);
        qCDebug(mainDebug) << logs.size() << "log value(s)";
    }

    bool ok = false;
    uint64_t ttl = Firewall::Manager::DEFAULT_TTL;
    if (parser.isSet(ttlOption))
    {
        ttl = parser.value(ttlOption).toULongLong(&ok);
    }
    if (!ok)
    {
        ttl = Firewall::Manager::DEFAULT_TTL;
        qCInfo(mainInfo) << "using default TTL"
                         << ttl;
    }

    ok = false;
    uint64_t gracePeriod = Firewall::Manager::DEFAULT_GRACE_PERIOD;
    if (parser.isSet(gracePeriodOption))
    {
        gracePeriod = parser.value(gracePeriodOption).toULongLong(&ok);
    }
    if (!ok)
    {
        gracePeriod = Firewall::Manager::DEFAULT_GRACE_PERIOD;
        qCInfo(mainInfo) << "using default grace period"
                         << gracePeriod;
    }

    /*
    LogWatcher watcher;
    ok = false;
    for (const QString& log: logs)
    {
        ok = watcher.addLogPath(log);
        if (ok)
        {
            qCInfo(mainInfo) << "watching" << log;
        }
        else
        {
            qCWarning(mainWarn) << "error adding"
                                << log << "to watch list";
        }
    }
    */

    qCDebug(mainDebug) << "initializing firewall manager";
    // manager = new Firewall::Manager{ttl, gracePeriod};
    qCDebug(mainDebug) << "firewall manager initialized";

    try
    {
        // manager->addBlockInboundAddressRule(L"192.168.1.234");
        // manager->addBlockInboundAddressRule(L"192.168.1.235");
        // manager->addBlockInboundAddressRule(L"192.168.1.233");
    }
    catch (const Firewall::GenericError& e)
    {
        qCWarning(mainWarn) << QString::fromStdWString(e.w_what());
    }

    try
    {
        // manager->pruneRules(ttl);
    }
    catch (const Firewall::GenericError& e)
    {
        qCWarning(mainWarn) << QString::fromStdWString(e.w_what());
    }

    // manager->startPruneTimer();

    qCInfo(mainInfo) << "running, press CTRL+C to exit";

    auto ret = QCoreApplication::exec();

    qCInfo(mainInfo) << "exiting";

    if (manager != nullptr)
    {
        manager->deleteLater();
    }

    return ret;
}
