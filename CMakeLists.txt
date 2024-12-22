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
find_package(OpenAL REQUIRED)

# Define project sources
set(PROJECT_SOURCES
    about.cpp
    about.h
    about.ui

    lib/io/pine.h

    lib/audio/adpcm.cpp
    lib/audio/adpcm.h
    lib/audio/bdutil.cpp
    lib/audio/bdutil.h
    audio/soundsystem.cpp
    audio/soundsystem.h
    audio/audioplayer.h
    audio/audioplayer.ui

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
    OpenAL::OpenAL
)
