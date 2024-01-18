cmake_minimum_required(VERSION 3.18)

project(harbour-sailbabel VERSION 0.6.0 LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Qml Quick LinguistTools REQUIRED)

include(FindPkgConfig)
pkg_search_module(SAILFISH sailfishapp REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/translations)

set(CMAKE_SKIP_RPATH OFF)
set(CMAKE_INSTALL_RPATH /usr/share/harbour-sailbabel/lib)

set(QML_FILES
    qml/pages/About.qml
    qml/pages/ChooseDirectory.qml
    qml/pages/Error.qml
    qml/pages/History.qml
    qml/pages/LoadDirectory.qml
    qml/pages/MainPage.qml
    qml/harbour-sailbabel.qml
    qml/cover/CoverPage.qml
)

set(TS_FILES
    translations/harbour-sailbabel.ts
    translations/harbour-sailbabel-en.ts
    translations/harbour-sailbabel-de.ts
)
list(TRANSFORM TS_FILES REPLACE "ts" "qm" OUTPUT_VARIABLE QM_FILES)
list(TRANSFORM QM_FILES PREPEND ${CMAKE_BINARY_DIR}/)

configure_file(src/Version.h.in ${CMAKE_BINARY_DIR}/src/Version.h @ONLY)

add_executable(harbour-sailbabel
    src/harbour-sailbabel.cpp
    src/Dictionary.hpp
    src/Dictionary.cpp
    src/FolderListModel.hpp
    src/FolderListModel.cpp
    src/AppModel.hpp
    src/AppModel.cpp
)
target_compile_definitions(harbour-sailbabel PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
    SAILJAIL
)
target_include_directories(harbour-sailbabel PRIVATE
    $<BUILD_INTERFACE:
    ${SAILFISH_INCLUDE_DIRS}
>
    ${CMAKE_BINARY_DIR}/src)
target_link_libraries(harbour-sailbabel
    Qt5::Core Qt5::Quick
    ${SAILFISH_LDFLAGS}
)
foreach(ts qm IN ZIP_LISTS TS_FILES QM_FILES)
    add_custom_command(OUTPUT ${qm}
        DEPENDS ${CMAKE_SOURCE_DIR}/${ts}
        COMMAND lrelease ${CMAKE_SOURCE_DIR}/${ts} -qm ${qm})
    install(FILES ${qm} DESTINATION share/harbour-sailbabel/translations)
endforeach()
add_custom_target(generate_qm_files ALL DEPENDS ${QM_FILES})

install(TARGETS harbour-sailbabel
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml
    DESTINATION share/harbour-sailbabel
)
install(FILES harbour-sailbabel.desktop
    DESTINATION share/applications
)
install(FILES icons/86x86/harbour-sailbabel.png
    DESTINATION share/icons/hicolor/86x86/apps
)
install(FILES icons/108x108/harbour-sailbabel.png
    DESTINATION share/icons/hicolor/108x108/apps
)
install(FILES icons/128x128/harbour-sailbabel.png
    DESTINATION share/icons/hicolor/128x128/apps
)
install(FILES icons/172x172/harbour-sailbabel.png
    DESTINATION share/icons/hicolor/172x172/apps
)
install(FILES icons/256x256/harbour-sailbabel.png
    DESTINATION share/icons/hicolor/256x256/apps
)
install(FILES images/cover_background.png
    DESTINATION share/harbour-sailbabel/images
)

# Get the other files reachable from the project tree in Qt Creator
FILE(GLOB TsFiles "translations/*.ts")
add_custom_target(distfiles
    SOURCES
        harbour-sailbabel.desktop
        ${QML_FILES}
        ${TsFiles}
#        rpm/harbour-sailbabel.changes.in
#        rpm/harbour-sailbabel.changes.run.in
        rpm/harbour-sailbabel.spec
#        rpm/harbour-sailbabel.yaml
)

# Tell Qt Creator where the application executable(s) would be located on the
# device.
#
# It is not necessary to list other deployables than executables (runtime
# targets) here. The deployment process of Sailfish OS projects is opaque to
# Qt Creator and the information contained in QtCreatorDeployment.txt is only
# used to locate the executable associated with the active run configuration
# on the device in order to run it.
#
# Search the Qt Creator Manual to learn about the QtCreatorDeployment.txt file
# format.
file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/harbour-sailbabel:bin
")
