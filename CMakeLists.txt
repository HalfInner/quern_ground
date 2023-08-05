project("Quern Farm")
cmake_minimum_required(VERSION 3.18)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set (SOURCES 
  user_drivers/spi_driver.cc
)
message(STATUS "System:" ${CMAKE_SYSTEM})

add_executable(quern_farm ${SOURCES})
add_library(quern_farm_api SHARED ${SOURCES})

set_property(TARGET quern_farm PROPERTY CXX_STANDARD 20)
set_property(TARGET quern_farm_api  PROPERTY CXX_STANDARD 20)