include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(run_fire_tests main.cpp wind_tests.cpp model_tests.cpp)

target_link_libraries(run_fire_tests gtest gtest_main)
target_link_libraries(run_fire_tests fire_lib)
