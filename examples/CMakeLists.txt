# Find Boost.Test dependency
find_package(Boost 1.70 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(SEND_ERROR "No se pudo encontrar la librería Boost")
endif()

add_executable(example src/example.cpp)

target_link_libraries(example
    fgs::z_module
    project_options
    project_warnings
    ${CONAN_LIBS_GMP}
)

target_include_directories(example PRIVATE ${CONAN_INCLUDE_DIRS_GMP})
