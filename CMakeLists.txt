cmake_minimum_required(VERSION 3.11)

project(text_to_hpp)

add_executable(text_to_hpp main.cpp)

set(Boost_USE_STATIC_LIBS ON)
add_subdirectory(convenience)
target_link_libraries(text_to_hpp PRIVATE convenience)

install(
	TARGETS text_to_hpp
	DESTINATION bin
)

if (WIN32)
    set(CPACK_GENERATOR WIX)
    set(CPACK_WIX_UPGRADE_GUID e3053697-8df7-4fe7-b2ad-1b5308f52ba3)
    set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/wix_patch.xml")
endif()

include(CPack)
