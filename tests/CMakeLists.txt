add_executable(z_module_test
    src/main.cpp
    src/constructors.cpp
    src/increment_decrement.cpp
)

target_link_libraries(z_module_test
    project_options
    project_warnings
    fgs::z_module
    ${CONAN_LIBS_CATCH2}
)

target_include_directories(z_module_test PRIVATE ${CONAN_INCLUDE_DIRS_CATCH2})


add_test(NAME z_module_test COMMAND z_module_test)
