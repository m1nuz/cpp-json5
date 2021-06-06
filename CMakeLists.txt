cmake_minimum_required(VERSION 3.15.0)
project(cpp-json5 VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(CPP_JSON5_BUILD_TEST "Build unit tests" OFF)
option(CPP_JSON5_BUILD_SAMPLE "Build sample program" OFF)
option(CPP_JSON5_INSTALL "Install library" OFF)

# 
# Library
#
set(LIB_NAME ${PROJECT_NAME})

add_library(${LIB_NAME} INTERFACE)

add_library(cpp-json5::cpp-json5 ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(${LIB_NAME}
    INTERFACE
        cxx_std_17
)

#
# Tests
#
if (CPP_JSON5_BUILD_TEST)
    include(CTest)
    include(Catch2)

    enable_testing()

    add_executable(json5tests test/test.cpp)

    target_compile_features(json5tests
        PUBLIC
            cxx_std_17
    )

    target_include_directories(json5tests
        PUBLIC
            ${Catch2_INCLUDE_DIR}
    )

    target_link_libraries(json5tests
        PUBLIC
            Catch2
            cpp-json5::cpp-json5
    ) 
endif()

#
# Usage sample
#
if (CPP_JSON5_BUILD_SAMPLE)
    set(SAMPLE_NAME ${LIB_NAME}-sample)

    add_executable(${SAMPLE_NAME} sample/main.cpp)

    if (UNIX AND NOT APPLE)
        target_compile_options(${SAMPLE_NAME}
            PUBLIC
                -pedantic
                -Wall
                -Wextra
                -Werror
                -Wshadow       
        )
        set(PLATFORM_LIBRARIES
            stdc++
            stdc++fs
        )
    elseif (APPLE)
        target_compile_options(${SAMPLE_NAME}
            PUBLIC
                -pedantic
                -Wall
                -Wextra
                -Werror
                -Wshadow
        )
        
        set(PLATFORM_LIBRARIES
            -stdlib=libc++
            -lc++abi
        )
    elseif (MSVC)
        target_compile_options(${SAMPLE_NAME}
            PUBLIC
                /W3
                /WX
        )
    endif()

    target_compile_features(${SAMPLE_NAME}
        PUBLIC
            cxx_std_17
    )

    target_link_libraries(${SAMPLE_NAME}
        PUBLIC
            cpp-json5::cpp-json5
            ${PLATFORM_LIBRARIES}            
    )
endif()

if(CPP_JSON5_INSTALL)
    include(GNUInstallDirs)

    install(TARGETS ${LIB_NAME}
        EXPORT ${LIB_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file("${LIB_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${LIB_NAME}Config.cmake
        ${PROJECT_BINARY_DIR}/${LIB_NAME}Config.cmake
        INSTALL_DESTINATION
            ${CMAKE_INSTALL_DATAROOTDIR}/${LIB_NAME}/cmake)

    install(EXPORT ${LIB_NAME}_Targets
        FILE ${LIB_NAME}Targets.cmake
        NAMESPACE ${LIB_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${LIB_NAME}/cmake)

    install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/json5
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    set(CPACK_PROJECT_NAME ${PROJECT_NAME})
    set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    include(CPack)
endif()