cmake_minimum_required(VERSION 3.16)

project(ChatBot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Widgets Core Multimedia)


include_directories(src)
include_directories(third-party/HTTPRequest/include)
include_directories(third-party/json/single_include/nlohmann)

file(GLOB CPP_HEADER_FILES "src/*.h")
file(GLOB CPP_SRC_FILES "src/*.cpp")
file(GLOB QML_FILES "src/qml/*.qml")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
    file(GLOB QRC_FILES "src/resource.qrc")
#    qt6_policy(SET QTP0001 NEW)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
    set(QRC_FILES "src/resource.qrc")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    MESSAGE(FATAL_ERROR "DO NOT support Mac OS X currently")
ELSE ()
    MESSAGE(FATAL_ERROR "other platform: ${CMAKE_SYSTEM_NAME}, only support linux and Windows")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")


set(QML_RELATIVE_PATH_FILES)
FOREACH(qml_abs_path ${QML_FILES})
    file(RELATIVE_PATH QML_FILE_R ${CMAKE_SOURCE_DIR} ${qml_abs_path})
    list(APPEND QML_RELATIVE_PATH_FILES ${QML_FILE_R})
    message(${QML_FILE_R})
ENDFOREACH(qml_abs_path)
 
message(STATUS "QML_FILES = ${QML_FILES}")
message(STATUS "QML_RELATIVE_PATH_FILES = ${QML_RELATIVE_PATH_FILES}")
# 启动工程筛选器
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
# 重命名CMake items 筛选器的名字,默认名为:CMakePredefinedTargets
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakeTargets")

source_group("Ui Files" FILES ${QML_FILES})
source_group("Header Files" FILES ${CPP_HEADER_FILES})
source_group("Source Files" FILES ${CPP_SRC_FILES})
source_group("Resource Files" FILES ${QRC_FILES})

qt_standard_project_setup()
qt_add_resources(ALL_SRC ${QRC_FILES})
qt_add_executable(appChatBot ${CPP_HEADER_FILES} ${CPP_SRC_FILES} ${ALL_SRC})

qt_add_qml_module(appChatBot
    URI ChatBot
    VERSION 1.0
    QML_FILES ${QML_RELATIVE_PATH_FILES}
    RESOURCES ${QRC_FILES}
)

set_target_properties(appChatBot PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appChatBot
    PRIVATE Qt6::Quick
)

install(TARGETS appChatBot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
