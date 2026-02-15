# Find all src files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/SinfulLang/*.cpp
)

# Create executable
add_library(SinfulLang ${SRC_FILES})

target_include_directories(SinfulLang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(SinfulLang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SinfulLang)
target_precompile_headers(SinfulLang PRIVATE include/sinful/pch.hpp)

# Set required C++ version
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SinfulLang PROPERTY CXX_STANDARD 23)
endif()
target_compile_features(SinfulLang PUBLIC cxx_std_23)
