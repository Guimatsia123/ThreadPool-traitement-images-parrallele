add_library(testutils testutils.cpp wrapper.c)
target_include_directories(testutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testutils PUBLIC core)

add_executable(test_tp3 test_tp3.cpp)
target_link_libraries(test_tp3 PRIVATE core testutils Catch2::Catch2WithMain)

