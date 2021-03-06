project(choqok)
cmake_minimum_required(VERSION 2.6)
find_package(KDE4 REQUIRED 4.4)
include (KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
set(CHOQOK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CHOQOK_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CHOQOK_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

option(QTINDICATE_DISABLE "Disable support for notifications via indicator")

find_package(QJson REQUIRED)
find_package(QCA2 REQUIRED)
find_package(QtOAuth REQUIRED)

if(NOT QTINDICATE_DISABLE)
    find_package(QtIndicate)
    if(QtIndicate_FOUND)
        set(QTINDICATE_BUILD TRUE)
	endif(QtIndicate_FOUND)
endif(NOT QTINDICATE_DISABLE)

if(QTINDICATE_BUILD)
    message(STATUS "Found libindicate-qt: Will support notifications via indicator")
else(QTINDICATE_BUILD)
    message(STATUS "Optional dependency libindicate-qt not found: Won't support notifications via indicator")
endif(QTINDICATE_BUILD)

set(CHOQOK_INCLUDES
${KDE4_INCLUDES}
${QT_INCLUDES}
${QJSON_INCLUDE_DIR}
${QCA2_INCLUDE_DIR}
${QTOAUTH_INCLUDE_DIR}
${CHOQOK_SOURCE_DIR}/helperlibs
${CHOQOK_BINARY_DIR}/helperlibs
${CHOQOK_SOURCE_DIR}/libchoqok/
${CHOQOK_BINARY_DIR}/libchoqok/
${CHOQOK_SOURCE_DIR}/libchoqok/ui/
)

add_subdirectory( libchoqok )
add_subdirectory( helperlibs )
add_subdirectory( choqok )
add_subdirectory( icons )
add_subdirectory( plugins )
add_subdirectory( microblogs )
add_subdirectory( images )
add_subdirectory( cmake )

include(MacroOptionalAddSubdirectory)
macro_optional_add_subdirectory( doc )
