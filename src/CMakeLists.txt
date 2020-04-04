if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_package(VLD)
    if (VLD_FOUND)
        message(STATUS "using VLD")
        message(STATUS "INCLUDE_DIR " ${VLD_INCLUDE_DIR})
        message(STATUS "INCLUDE_DIRS " ${VLD_INCLUDE_DIRS})
        message(STATUS "LIBRARY_DIR " ${VLD_LIBRARY_DIR})
        message(STATUS "LIBRARY_DEBUG " ${VLD_LIBRARY_DEBUG})
        message(STATUS "LIBRARY " ${VLD_LIBRARY})
        add_definitions(-DHAVE_VLD)
        include_directories("${VLD_INCLUDE_DIRS}")
    else ()
        message(WARNING "VLD not found")
    endif ()
else ()
    message(STATUS "not using VLD")
endif ()

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
        Qt5::Core
        Qt5::Sql
)
