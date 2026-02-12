cmake_minimum_required(VERSION 3.5)

project(ConveyorInterfaceQt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test Mode Option - enables simulator/test mode for hardware-free testing
option(CONVEYOR_TEST_MODE "Enable test mode (no hardware required)" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialBus)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ConveyorInterfaceQt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        motor.h motor.cpp
        motorfactors.h motorfactors.cpp motorfactors.ui
        scaninputs.h scaninputs.cpp
        Counter.cpp Counter.h
        tray.h tray.cpp
        writedigitalout.cpp
        writeanalogoutput.cpp
        #readdigitalin.cpp
        ReadWriteJson.cpp
        Stop.cpp
        Run1.cpp
        TimeDelay.cpp
        Run2.cpp
        EStop.cpp
        timers.cpp
        MotorCalibFact.json
        counter.txt
        CountDownTimer.json
        TrayFactors.json
        COMPorts.json
        TrayCalibFactors.ui
        traycalibfactors.h traycalibfactors.cpp TrayCalibFactors.ui
        updatecomports.h updatecomports.cpp updatecomports.ui
        BuzzerDelay.cpp
        uppersoilbeltfact.h uppersoilbeltfact.cpp uppersoilbeltfact.ui
        UpperSoilBeltFactors.json
        ProductionLog.h ProductionLog.cpp
        logviewer.h logviewer.cpp logviewer.ui
        #ModbusOutput.h ModbusOutput.cpp
        #ModbusInput.h ModbusInput.cpp
        #ReadWriteModbus.h ReadWriteModbus.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ConveyorInterfaceQt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ConveyorInterfaceQt SHARED
            ${PROJECT_SOURCES}
        )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
    add_executable(ConveyorInterfaceQt
        ${PROJECT_SOURCES}
    )
endif()
endif()

target_link_libraries(ConveyorInterfaceQt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialBus)

# Configure test mode preprocessor definition
if(CONVEYOR_TEST_MODE)
    target_compile_definitions(ConveyorInterfaceQt PRIVATE CONVEYOR_TEST_MODE=1)
    message(STATUS "Test mode enabled - application will run in simulator mode")
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ConveyorInterfaceQt)
endif()

set_target_properties(ConveyorInterfaceQt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ConveyorInterfaceQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ConveyorInterfaceQt)
endif()
