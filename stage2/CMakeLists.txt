cmake_minimum_required(VERSION 3.16...3.27)
project(WaterBox)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(water_box water_box.cc ${sources} ${headers})
target_include_directories(water_box PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(water_box ${Geant4_LIBRARIES})

foreach(_macro run1.mac vis.mac)
    configure_file(${PROJECT_SOURCE_DIR}/${_macro} ${PROJECT_BINARY_DIR}/${_macro} COPYONLY)
endforeach()
