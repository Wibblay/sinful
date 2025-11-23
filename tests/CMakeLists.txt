# Collect all test_*.cpp files
file(GLOB_RECURSE TEST_FILES "test_*.cpp")

# Create a single test executable
add_executable(SinfulTests ${TEST_FILES})

# Ensure compiler library is visible to tests
target_link_libraries(SinfulTests PRIVATE SinfulLang)

# Link gtest
set(GTest_DIR "C:/vcpkg/installed/x64-windows/share/gtest")
find_package(GTest CONFIG REQUIRED)
target_link_libraries(SinfulTests PRIVATE GTest::gtest GTest::gtest_main)

add_custom_command(TARGET SinfulTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<CONFIG:Debug>:C:/vcpkg/installed/x64-windows/debug/bin/gtest_main.dll>
    $<$<CONFIG:Release>:C:/vcpkg/installed/x64-windows/bin/gtest_main.dll>
    $<TARGET_FILE_DIR:SinfulTests>)

add_custom_command(TARGET SinfulTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<CONFIG:Debug>:C:/vcpkg/installed/x64-windows/debug/bin/gtest.dll>
    $<$<CONFIG:Release>:C:/vcpkg/installed/x64-windows/bin/gtest.dll>
    $<TARGET_FILE_DIR:SinfulTests>)

# Add a CTest target so you can run `ctest`
enable_testing()
add_test(NAME SinfulTests COMMAND SinfulTests)