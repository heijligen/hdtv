cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(hdtv_rootext LANGUAGES C CXX)

find_package(ROOT 6.20 CONFIG REQUIRED)

include(${ROOT_DIR}/RootMacros.cmake)

set(CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})

# ^ move this to a toplevel CMakeLists.txt

set(MODULE calibration)

set(HEADERS
    Calibration.hh
)

set(SOURCES
    Calibration.cc
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

root_generate_dictionary(G__${MODULE} ${HEADERS} MODULE ${MODULE} LINKDEF LinkDef.h)
add_library(${MODULE} SHARED ${SOURCES} G__${MODULE}.cxx)
add_library(hdtv::${MODULE} ALIAS ${MODULE})
target_link_libraries(${MODULE} PUBLIC ROOT::Core ROOT::Hist ROOT::Graf ROOT::Gui)

if(UNIX)
    set_target_properties(${MODULE} PROPERTIES RESOURCE
      "${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE}.rootmap;${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE}_rdict.pcm"
    )
elseif(WIN32)
    set_target_properties(${MODULE} PROPERTIES RESOURCE
      "${CMAKE_CURRENT_BINARY_DIR}/${MODULE}.rootmap;${CMAKE_CURRENT_BINARY_DIR}/${MODULE}_rdict.pcm"
    )
else()
    message(FATAL_ERROR "Do not know how to handle non UNIX and non WIN32 systems")
endif()


install(TARGETS ${MODULE} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
