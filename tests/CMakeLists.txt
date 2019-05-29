# Find Boost.Test dependency
find_package(Boost 1.70 REQUIRED COMPONENTS unit_test_framework)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(SEND_ERROR "No se pudo encontrar la librería Boost")
endif()

#add_executable(unit_tests unit_tests.cpp)
#target_link_libraries(unit_tests
#    polynomials
#    project_options
#    project_warnings
#    Boost::unit_test_framework
#)

#add_test(NAME unit_tests COMMAND unit_tests)

add_executable(validation_test src/validation_test.cpp)
target_link_libraries(validation_test
    fgs::z_module
    project_options
    project_warnings
)

add_test(NAME validation_test COMMAND validation_test)
