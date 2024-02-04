cmake_minimum_required(VERSION 3.5)

project(neobesms LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui REQUIRED)

set(PROJECT_SOURCES
    adpcm.cpp
    adpcm.h
    bdutil.cpp
    bdutil.h
    config.h
    main.cpp
    neobes.cpp
    neobes.h
    neobes.ui
    neodata.cpp
    neodata.h
    neosound.cpp
    neosound.h
    pcsx2reader.cpp
    pcsx2reader.h
    pcsx2upload.cpp
    pcsx2util.cpp
    pcsx2util.h
    pine.h
    stageinfo.cpp
    stageinfo.h
    suggest.cpp
    suggest.h
    types.h
    intcommand.h intcommand.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        resources.qrc
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        resources.qrc
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)
