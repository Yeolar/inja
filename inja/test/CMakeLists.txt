# Copyright 2020-present Yeolar

set(INJA_TEST_SRCS
  test-files.cpp
  test-functions.cpp
  test-renderer.cpp
  test-units.cpp
)

foreach(test_src ${INJA_TEST_SRCS})
  get_filename_component(test_name ${test_src} NAME_WE)
  set(test inja_${test_name})
  add_executable(${test} ${test_src})
  target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES})
  add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()
