# Collect all SinfulLang source files
file(GLOB_RECURSE SINFUL_LANG_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/SinfulLang/*.cpp
)

# Create static library
add_library(SinfulLang STATIC ${SINFUL_LANG_SOURCES})

target_include_directories(SinfulLang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(SinfulLang PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/SinfulLang)
target_precompile_headers(SinfulLang PRIVATE include/sinful/pch.hpp)
target_compile_features(SinfulLang PUBLIC cxx_std_23)

# Entry-point executable
add_executable(SinfulApp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(SinfulApp PRIVATE SinfulLang)
target_include_directories(SinfulApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/SinfulLang)
target_compile_features(SinfulApp PRIVATE cxx_std_23)

