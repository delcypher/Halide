# FIXME: Use halide_project() instead
add_executable(interpolate interpolate.cpp)

if (NOT WIN32)
  target_link_libraries(interpolate PRIVATE dl pthread)
endif()
target_include_directories(interpolate PRIVATE ${PNG_INCLUDE_DIRS})
target_compile_definitions(interpolate PRIVATE ${PNG_DEFINITIONS})
target_link_libraries(interpolate PRIVATE Halide ${PNG_LIBRARIES})
set_target_properties(interpolate PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      "${CMAKE_CURRENT_BINARY_DIR}")
