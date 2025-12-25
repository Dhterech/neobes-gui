cmake_minimum_required(VERSION 3.5)

project(neobesms LANGUAGES CXX)

# Enable standard CMake Qt options
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Require Qt6 and OpenAL
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

# Define project sources
set(PROJECT_SOURCES
    about.cpp
    about.h
    about.ui

    lib/pine/src/pine.h
    lib/miniaudio.h

    audio/soundsystem.cpp
    audio/soundsystem.h
    audio/audioplayer.h
    audio/audioplayer.ui

    audio/bd.h
    audio/bd.cpp

    config.h
    main.cpp
    neobes.cpp
    neobes.h
    neobes.ui
    neodata.cpp
    neodata.h

    data/ptr2stages.cpp
    data/ptr2stages.h
    data/ptr2struct.cpp
    data/ptr2struct.h

    io/emuinterfacer.cpp
    io/emuinterfacer.h
    io/ptr2injector.cpp
    io/ptr2reader.cpp
    io/ptr2reader.h

    types.h
    intcommand.h intcommand.cpp
    resources.qrc
)

# Add executable for Qt6
qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

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
