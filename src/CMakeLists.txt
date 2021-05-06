cmake_minimum_required(VERSION 3.10)
project(${PROJECT_NAME})

add_definitions(-DSTB_IMAGE_WRITE_IMPLEMENTATION)

set(STB_IMAGE  "../stb_image_write.h")
set(IMAGE_SRC Image.h Image.cpp)
set(RTX_SRC vec3.h main.cpp "Ray.h" "Hitable.h" "Shape.h" "Shape.cpp" "HittableList.h" "HittableList.cpp" "rtweekend.h" "Camera.h" "Material.h")


add_executable(${PROJECT_NAME} ${RTX_SRC} ${IMAGE_SRC} ${STB_IMAGE})
target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX SDL2)
#target_link_libraries(${PROJECT_NAME} sfml-window sfml-graphics)