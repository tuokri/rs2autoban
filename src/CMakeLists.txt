add_executable(
        rs2autoban
        main.cpp
        Firewall.hpp
        Firewall.cpp
        Utils.hpp
        Utils.cpp
        LogWatcher.cpp
        LogWatcher.hpp
)

target_link_libraries(
        rs2autoban
        ole32.lib
        oleaut32.lib
        Qt6::Core
        Qt6::Sql
)
