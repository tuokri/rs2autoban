#include <iostream>
#include <exception>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include "Firewall.hpp"
#include "Utils.hpp"
#include "LogWatcher.hpp"

#ifdef DEBUGMEMLEAK
#define _CRTDBG_MAP_ALLOC

#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

#endif

TEST_CASE("Utils", "[Utils]")
{
    std::wstring dateNow = Utils::ISO8601DateNow();
    REQUIRE(!dateNow.empty());

    std::tm t1 = Utils::ISO8601WStrToTime(dateNow);
    REQUIRE(t1.tm_year > 0);

    REQUIRE_THROWS_AS(Utils::ISO8601WStrToTime(L"badstring"), std::invalid_argument);

    std::tm invalidTm{0};
    REQUIRE_THROWS_AS(Utils::TmToISO8601WStr(invalidTm), std::invalid_argument);

    std::wstring result = Utils::HRESULTToWString(0);
    REQUIRE(!result.empty());
}

TEST_CASE("Firewall::Manager creation/destruction", "[Firewall]")
{
    auto* o = new QObject{};
    auto* manager = new Firewall::Manager{1, 1, o};

    manager->pruneRules();
    manager->deleteLater();
    o->deleteLater();

    delete o;
}

int main(int argc, char* argv[])
{
#ifdef DEBUGMEMLEAK
    std::cout << "checking for memory leaks\n";
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif

    int result = Catch::Session().run(argc, argv);

    return result;
}
