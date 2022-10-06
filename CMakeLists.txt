cmake_minimum_required(VERSION 3.24)
project(encryption)

# ===================================================
# CONFIGURATION
# ===================================================
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(ignoreMe "${CMAKE_TOOLCHAIN_FILE}${Boost_NO_WARN_NEW_VERSIONS}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/$<CONFIG>/${CMAKE_SYSTEM_NAME}")
  if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:TRUE>)
  endif()
endif()

set(TARGET_NAME encryption)
set(SOURCES "src/main.cpp")
add_executable(${TARGET_NAME} ${SOURCES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)