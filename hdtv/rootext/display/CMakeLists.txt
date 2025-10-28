cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(display LANGUAGES CXX)

set(SOURCES
    DisplayBlock.cc
    DisplayCut.cc
    DisplayFunc.cc
    DisplayObj.cc
    DisplaySpec.cc
    DisplayStack.cc
    Marker.cc
    MTViewer.cc
    Painter.cc
    View1D.cc
    View2D.cc
    View.cc
    Viewer.cc
    XMarker.cc
    YMarker.cc)

set(HEADERS
    DisplayBlock.hh
    DisplayCut.hh
    DisplayFunc.hh
    DisplayObj.hh
    DisplayObjZIndex.hh
    DisplaySpec.hh
    DisplayStack.hh
    Marker.hh
    MTViewer.hh
    Painter.hh
    View1D.hh
    View2D.hh
    Viewer.hh
    View.hh
    XMarker.hh
    YMarker.hh)

if (UNIX AND NOT APPLE)
find_package(X11 REQUIRED)
endif()


find_package(
  ROOT REQUIRED
  COMPONENTS Core
             Hist
             Graf
             Gui)
message(STATUS "ROOT Version ${ROOT_VERSION} found in ${ROOT_root_CMD}")
if(${ROOT_VERSION_MINOR} GREATER_EQUAL 20)
  include(${ROOT_DIR}/RootMacros.cmake)
elseif(${ROOT_VERSION_MINOR} EQUAL 19 AND EXISTS ${ROOT_DIR}/RootMacros.cmake)
  include(${ROOT_DIR}/RootMacros.cmake)
else()
  include(${ROOT_DIR}/modules/RootNewMacros.cmake)
endif()

root_generate_dictionary(
  G__${PROJECT_NAME}
  ${HEADERS}
  LINKDEF
  LinkDef.h
  OPTIONS
  -I${CMAKE_CURRENT_SOURCE_DIR}/../calibration)

add_library(${PROJECT_NAME} SHARED ${SOURCES} G__${PROJECT_NAME}.cxx)
add_library(hdtv::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

if (UNIX AND NOT APPLE)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DUSE_X11)
endif()

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    RESOURCE
    "${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.rootmap;${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}_rdict.pcm"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/../calibration)

if (UNIX AND NOT APPLE)
target_link_libraries(
  ${PROJECT_NAME}
  ROOT::Core
  ROOT::Hist
  ROOT::Graf
  ROOT::Gui
  X11)
else()
target_link_libraries(
  ${PROJECT_NAME}
  ROOT::Core
  ROOT::Hist
  ROOT::Graf
  ROOT::Gui
  )
endif()

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RESOURCE DESTINATION lib)
