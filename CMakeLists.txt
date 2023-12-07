# Establece la versión mínima requerida de CMake
cmake_minimum_required(VERSION 3.20)

# Define el nombre del proyecto
project("Socket Lib" LANGUAGES CXX)

# Configuración del estándar de C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} --coverage -fprofile-abs-path")

option(MULTICOMMSLIB_BUILD_TESTS "Build test programs" OFF)

include(FetchContent)

# Configuración de SpdLog
if (NOT TARGET spdlog)
    SET(spdlog_path "components/0009.016 spdlog")

    if(EXISTS "${PROJECT_SOURCE_DIR}/${spdlog_path}")
        add_subdirectory(${spdlog_path})
    else()
        FetchContent_Declare(
        spdlog
        SVN_REPOSITORY https://subversion.gtd.local/E1550/Tecnica/Entradas/0009.016%20spdlog/Source%20code/trunk
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/${spdlog_path}"
        SVN_TRUST_CERT FALSE # TODO: comprobar en linux, puede que sea necesario el flag, en windows hace que no funcione
        )
        FetchContent_MakeAvailable(spdlog)
    endif()
endif()

include_directories(include)

# Configura las rutas de inclusión para las bibliotecas
add_library(SerializableLib  src/serializable/Serializable.cpp)

target_include_directories(
    SerializableLib INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Set de HEADERS:
set(HEADERS
    include/factory/FactorySocket.h
    include/observer/EventListener.h
    include/observer/subscriber.h
    include/socket/Socket.h
    include/serializable/Serializable.h
)

# Set de SOURCES:
set(SOURCES
    src/factory/FactorySocket.cpp
    src/observer/EventListener.cpp
    src/serializable/Serializable.cpp
    src/socket/UDPSocket.cpp
    src/socket/SerialSocket.cpp
)

add_library(SocketLib ${SOURCES} ${HEADERS} )
target_link_libraries(SocketLib SerializableLib spdlog::spdlog)
if (WIN32)
    target_link_libraries(SocketLib wsock32 ws2_32)
endif ()
    # Set the version of SocketLib to 1.0

add_executable(Demo src/main/main.cpp)
target_link_libraries(Demo SocketLib)

if(MULTICOMMSLIB_BUILD_TESTS)
    # Configuración de Google Test
    if (NOT TARGET gtest)
        SET(googletest_path "components/0003.020 googletest")

        if(EXISTS "${PROJECT_SOURCE_DIR}/${googletest_path}")
            add_subdirectory(${googletest_path})
        else()
            FetchContent_Declare(
                googletest
                SVN_REPOSITORY https://subversion.gtd.local/E1550/Tecnica/Entradas/0003.020%20googletest/v1.13.0/Source%20Code
                SOURCE_DIR "${PROJECT_SOURCE_DIR}/${googletest_path}"
                SVN_TRUST_CERT FALSE # TODO: comprobar en linux, puede que sea necesario el flag, en windows hace que no funcione
            )
            # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(googletest)
        endif()
    endif()


    enable_testing()
    include(GoogleTest)

    # Configura los tests unitarios para Windows
    add_executable(TestUDP test/socket/TESTUDPSocket.cpp)
    target_link_libraries(TestUDP SocketLib GTest::gtest_main)
    gtest_discover_tests(
        TestUDP
        TEST_PREFIX "Udp."
        XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/results
    )
endif()

# add_executable(TestTCP test/socket/TESTTCPSocket.cpp)
# target_link_libraries(TestTCP SocketLib GTest::gtest_main)
# gtest_discover_tests(TestTCP)

# add_executable(TestSerial test/socket/TESTSerialSocket.cpp)
# target_link_libraries(TestSerial SocketLib GTest::gtest_main)
# gtest_discover_tests(TestSerial)


# option(CodeCoverage "CodeCoverage" ON)
# set(CMAKE_CXX_FLAGS "-Wno-deprecated-register ${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_FLAGS_DEBUG "-Wno-deprecated-register -O0 -g -fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS_DEBUG}")
# set(CMAKE_CXX_FLAGS_DEBUG "-O0 --coverage -fprofile-arcs -ftest-coverage -fno-elide-constructors -fno-inline")
# set(CMAKE_LFLAGS_DEBUG "-lgcov --coverage")



# if (WIN32)

#     export(TARGETS SocketLib SerializableLib FILE SocketLibConfig.cmake)

# # Create a package configuration file for SocketLib
#     include(CMakePackageConfigHelpers)
#     write_basic_package_version_file(${CMAKE_BINARY_DIR}/SocketLibConfigVersion.cmake VERSION 1.0 COMPATIBILITY SameMajorVersion)

#     # Install the package configuration files
#     install(FILES
#             ${CMAKE_BINARY_DIR}/SocketLibConfig.cmake
#             ${CMAKE_BINARY_DIR}/SocketLibConfigVersion.cmake
#             DESTINATION ${INSTALL_LIB_DIR}/lib/cmake/SocketLib
#     )

#     # Specify the installation directory for the library
#     # Define la ruta de instalación para las bibliotecas y archivos de encabezado
#     set(INSTALL_LIB_DIR "lib")
#     set(INSTALL_INCLUDE_DIR "include")

#     # Instala las bibliotecas
#     install(TARGETS SocketLib
#             EXPORT SocketLibTargets
#             ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
#             LIBRARY DESTINATION ${INSTALL_LIB_DIR}
#             RUNTIME DESTINATION ${INSTALL_LIB_DIR}
#     )

#     # Instala los archivos de encabezado
#     install(FILES ${HEADERS}
#             DESTINATION ${INSTALL_INCLUDE_DIR}/SocketLib
#     )

#     # Exporta los objetivos (targets) del paquete
#     export(TARGETS SocketLib NAMESPACE SocketLib::
#             FILE ${CMAKE_BINARY_DIR}/SocketLibTargets.cmake
#     )

#     # Crea un archivo de configuración de paquete
#     include(CMakePackageConfigHelpers)
#     write_basic_package_version_file(${CMAKE_BINARY_DIR}/SocketLibConfigVersion.cmake VERSION 1.0 COMPATIBILITY SameMajorVersion)

#     # Instala los archivos de configuración del paquete
#     install(FILES
#             ${CMAKE_BINARY_DIR}/SocketLibConfig.cmake
#             ${CMAKE_BINARY_DIR}/SocketLibConfigVersion.cmake
#             DESTINATION ${INSTALL_LIB_DIR}/cmake/SocketLib
#     )

# endif ()

# # Export LinuxSocketLib as a precompiled library
# if (UNIX AND NOT APPLE)

#     # Specify the installation directory for the library
#     set(LINUX_INSTALL_LIB_DIR "bin")

#     install(TARGETS SocketLib
#             EXPORT LinuxSocketLibTargets
#             ARCHIVE DESTINATION ${LINUX_INSTALL_LIB_DIR}/lib
#             LIBRARY DESTINATION ${LINUX_INSTALL_LIB_DIR}/lib
#             RUNTIME DESTINATION ${LINUX_INSTALL_LIB_DIR}/bin
#     )

#     # Install the header files associated with LinuxSocketLib
#     install(DIRECTORY src/socket/Serial/LinuxSerial/
#             DESTINATION ${LINUX_INSTALL_LIB_DIR}/include/LinuxSocketLib
#             FILES_MATCHING PATTERN "*.h"
#     )

#     # Export the targets to a package configuration file
#     export(TARGETS SocketLib SerializableLib NAMESPACE SocketLib::
#             FILE ${CMAKE_BINARY_DIR}/LinuxSocketLibTargets.cmake
#     )

#     # Create a package configuration file for LinuxSocketLib
#     include(CMakePackageConfigHelpers)
#     write_basic_package_version_file(${CMAKE_BINARY_DIR}/LinuxSocketLibConfigVersion.cmake VERSION 1.0 COMPATIBILITY SameMajorVersion)


#     # Install the package configuration files
#     install(FILES
#             ${CMAKE_BINARY_DIR}/LinuxSocketLibConfig.cmake
#             ${CMAKE_BINARY_DIR}/LinuxSocketLibConfigVersion.cmake
#             DESTINATION ${LINUX_INSTALL_LIB_DIR}/lib/cmake/LinuxSocketLib
#     )
# endif ()
