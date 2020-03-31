#include <iostream>
#include <string>

#include <QCoreApplication>
#include <QFileSystemWatcher>
#include <QCommandLineParser>

#include "Firewall.hpp"

constexpr int DEFAULT_TTL = 3600;
constexpr int DEFAULT_GRACE_PERIOD = 15;

int __cdecl main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion("0.0.1");
    QCoreApplication::setApplicationName("RS2Autoban");

    QCommandLineParser parser;

    parser.setApplicationDescription(
        "Automatic banning of malicious IP addresses "
        "from Rising Storm 2: Vietnam server logs.");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions(
        {
            {"l",                   "server log file"},
            {"t",                   "time to live for bans in seconds"},
            {{"g", "grace-period"}, "seconds to wait after seeing "
                                    "an IP address for the first for valid Steam ID"}
        }
    );

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    const QString log{args[0]};

    bool ok;
    int ttl = args[1].toInt(&ok);
    if (!ok)
    {
        std::cout << "using default TTL (" << ttl << ")\n";
        ttl = DEFAULT_TTL;
    }

    int gracePeriod = args[2].toInt(&ok);
    if (!ok)
    {
        std::cout << "using default grade period ("
                  << DEFAULT_GRACE_PERIOD << ")\n";
        gracePeriod = DEFAULT_GRACE_PERIOD;
    }

    QFileSystemWatcher watcher;
    watcher.addPath(log);

    std::cout << "initializing firewall manager\n";
    Firewall::Manager manager{};
    std::cout << "firewall manager initialized\n";

    try
    {
        manager.AddBlockInboundAddressRule(L"192.168.1.233");
        manager.AddBlockInboundAddressRule(L"192.168.1.234");
        manager.AddBlockInboundAddressRule(L"192.168.1.235");
    }
    catch (const Firewall::GenericError& e)
    {
        std::wcout << e.w_what() << "\n";
        std::cout << e.what() << "\n";
    }

    try
    {
        manager.PruneRules(ttl);
    }
    catch (const Firewall::GenericError& e)
    {
        std::wcout << e.w_what() << "\n";
    }

    return QCoreApplication::exec();
}
