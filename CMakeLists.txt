# CMakeList.txt : CMake project for Sinful, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Sinful")

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE) 
	set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/ildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file") 
endif()

add_subdirectory(src)
add_subdirectory(tests)

add_executable(SinfulApp ${CMAKE_SOURCE_DIR}/Sinful.cpp)
target_link_libraries(SinfulApp PRIVATE SinfulLang)
