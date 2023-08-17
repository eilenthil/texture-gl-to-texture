set (sources main.cpp)


add_executable(render ${sources})
target_link_libraries(render ${GLFW3_LIBRARY} GL GLEW png)

