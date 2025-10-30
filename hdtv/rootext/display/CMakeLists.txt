cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(hdtv_rootext LANGUAGES C CXX)

find_package(ROOT 6.20 CONFIG REQUIRED)

if(UNIX AND NOT APPLE)
find_package(X11 REQUIRED)
endif()

include(${ROOT_DIR}/RootMacros.cmake)

set(CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})

# ^ move this to a toplevel CMakeLists.txt

set(MODULE display)

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
    YMarker.cc
    ../calibration/Calibration.cc # This is a hack for windows
)

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
    YMarker.hh
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../calibration)

  
root_generate_dictionary(G__${MODULE} ${HEADERS} MODULE ${MODULE} LINKDEF LinkDef.h)
add_library(${MODULE} SHARED ${SOURCES} G__${MODULE}.cxx)
add_library(hdtv::${MODULE} ALIAS ${MODULE})

if (UNIX AND NOT APPLE)
    target_compile_definitions(${MODULE} PRIVATE -DUSE_X11)
    target_link_libraries(${MODULE} PUBLIC ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gui X11)
else()
    target_link_libraries(${MODULE} PUBLIC ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gui)
endif()

set_target_properties(${MODULE} PROPERTIES RESOURCE
    "${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE}.rootmap;${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE}_rdict.pcm"
)

install(TARGETS ${MODULE} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
