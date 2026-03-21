# Collect all test_*.cpp files (excluding test_Main.cpp which duplicates gtest_main)
file(GLOB_RECURSE TEST_FILES "test_*.cpp")
list(FILTER TEST_FILES EXCLUDE REGEX "test_Main\\.cpp$")

# Find GTest before creating the executable
find_package(GTest CONFIG REQUIRED)

# Create a single test executable
add_executable(SinfulTests ${TEST_FILES})

# Ensure compiler library and internal headers are visible to tests
target_link_libraries(SinfulTests PRIVATE SinfulLang GTest::gtest GTest::gtest_main)
target_include_directories(SinfulTests PRIVATE "${CMAKE_SOURCE_DIR}/src/SinfulLang")

# Copy runtime DLLs next to the test executable
add_custom_command(TARGET SinfulTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:SinfulTests>
        $<TARGET_FILE_DIR:SinfulTests>
    COMMAND_EXPAND_LISTS
)

add_test(NAME SinfulTests COMMAND SinfulTests)
