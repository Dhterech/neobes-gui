cmake_minimum_required(VERSION 3.5)

project(neobesms LANGUAGES CXX)

# Enable standard CMake Qt options
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Require Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

# Define project sources
set(PROJECT_SOURCES
    main.cpp
    config.h
    types.h
    neodata.cpp
    neodata.h
    job.cpp
    job.h

    ui/about.cpp
    ui/about.h
    ui/about.ui



    ui/settings.cpp
    ui/settings.h
    ui/settings.ui

    audio/soundmanager.cpp
    audio/soundmanager.h
    audio/audioplayer.h
    audio/audioplayer.ui
    audio/bd.cpp
    audio/bd.h

    data/stageinfo.cpp
    data/stageinfo.h
    data/ptr2struct.cpp
    data/ptr2struct.h

    io/emuinterfacer.cpp
    io/emuinterfacer.h
    io/ptr2injector.cpp
    io/ptr2reader.cpp
    io/ptr2reader.h

    lib/miniaudio.h
    lib/pine/src/pine.h

    resources.qrc
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
    ui/guimanager.h ui/guimanager.cpp
    ui/menugui.h ui/menugui.cpp ui/menugui.ui
    ui/editorgui.h ui/editorgui.cpp ui/editorgui.ui)

# Link necessary libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Threads::Threads # Miniaudio threads
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE dl m)
endif()
