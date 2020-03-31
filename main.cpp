#include <iostream>

#include <QCoreApplication>
#include <QFileSystemWatcher>
#include <QCommandLineParser>

#include "Firewall.hpp"

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
            {"t",                   "time to live for bans"},
            {{"g", "grace-period"}, "seconds to wait after seeing "
                                    "an IP address for the first for valid Steam ID"}
        }
    );

    parser.process(a);

    QFileSystemWatcher watcher;

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
        manager.PruneRules(0);
    }
    catch (const Firewall::GenericError& e)
    {
        std::wcout << e.w_what() << "\n";
    }

    return QCoreApplication::exec();
}
