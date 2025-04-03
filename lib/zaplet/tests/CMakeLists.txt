# We collect a list of test files
set(TEST_SOURCES

)

# Add tests using our utilitarian function
add_project_test(test_zaplet
        SOURCES ${TEST_SOURCES}
        DEPENDENCIES ${PROJECT_NAME}::zaplet GTest::gtest_main
)