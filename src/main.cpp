#include <cinttypes>

#include <QCoreApplication>
#include <QFileSystemWatcher>
#include <QCommandLineParser>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(mainProg)

Q_LOGGING_CATEGORY(mainProg, "Main")

#include "Firewall.hpp"
#include "LogWatcher.hpp"

int __cdecl main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion("0.0.1");
    QCoreApplication::setApplicationName("RS2Autoban");

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
        qCDebug(mainProg) << "logOption is set";
        logs = parser.values(logOption);
        qCDebug(mainProg) << logs.size() << "log value(s)";
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
        qCInfo(mainProg) << "using default TTL"
                         << Firewall::Manager::DEFAULT_TTL;
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
        qCInfo(mainProg) << "using default grace period"
                         << Firewall::Manager::DEFAULT_GRACE_PERIOD;
    }

    LogWatcher watcher;
    ok = false;
    for (const QString& log: logs)
    {
        ok = watcher.addLogPath(log);
        if (ok)
        {
            qCInfo(mainProg) << "watching" << log;
        }
        else
        {
            qCWarning(mainProg) << "error adding"
                                << log << "to watch list";
        }
    }

    qCDebug(mainProg) << "initializing firewall manager";
    Firewall::Manager manager{ttl, gracePeriod};
    qCDebug(mainProg) << "firewall manager initialized";

    try
    {
        manager.addBlockInboundAddressRule(L"192.168.1.233");
        manager.addBlockInboundAddressRule(L"192.168.1.234");
        manager.addBlockInboundAddressRule(L"192.168.1.235");
    }
    catch (const Firewall::GenericError& e)
    {
        qCWarning(mainProg) << QString::fromStdWString(e.w_what());
    }

    try
    {
        manager.pruneRules(ttl);
    }
    catch (const Firewall::GenericError& e)
    {
        qCWarning(mainProg) << QString::fromStdWString(e.w_what());
    }

    return QCoreApplication::exec();
}
