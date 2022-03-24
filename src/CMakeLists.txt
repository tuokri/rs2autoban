add_executable(
    rs2autoban
    main.cpp
    Firewall.cpp
    Utils.cpp
    LogWatcher.cpp
)

target_link_libraries(
    rs2autoban
    PUBLIC
    ole32.lib
    oleaut32.lib
    Qt6::Core
    Qt6::Sql
)
