# We collect a list of test files
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Add tests using our utilitarian function
add_project_test(test_zaplet
        SOURCES ${TEST_SOURCES}
        DEPENDENCIES ${PROJECT_NAME}::zaplet GTest::gtest_main
)