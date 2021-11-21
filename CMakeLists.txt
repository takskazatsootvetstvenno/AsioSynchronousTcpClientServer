cmake_minimum_required(VERSION 3.20)

get_filename_component(GENERAL_FOLDER ../ ABSOLUTE)
set(CMAKE_INSTALL_PREFIX "${GENERAL_FOLDER}" CACHE PATH "${GENERAL_FOLDER}" FORCE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

SET(SERVER_PROJECT_NAME Server)

project(${SERVER_PROJECT_NAME})
conan_basic_setup()
SET(SERVER_SOURCES src/asio_server.cpp)

add_executable(${SERVER_PROJECT_NAME}
	${SERVER_SOURCES}
)
target_compile_features(${SERVER_PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${SERVER_PROJECT_NAME} PRIVATE src)

target_link_libraries(${SERVER_PROJECT_NAME} ${CONAN_LIBS})

install(TARGETS ${SERVER_PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/install)

SET(CLIENT_PROJECT_NAME Client)

project(${CLIENT_PROJECT_NAME})

SET(CLIENT_SOURCES src/asio_client.cpp)

add_executable(${CLIENT_PROJECT_NAME}
	${CLIENT_SOURCES}
)
target_compile_features(${CLIENT_PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${CLIENT_PROJECT_NAME} PRIVATE src)

target_link_libraries(${CLIENT_PROJECT_NAME} ${CONAN_LIBS})

install(TARGETS ${CLIENT_PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/install)