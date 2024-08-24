cmake_minimum_required(VERSION 3.29)
project(game)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(
  game
  ${SOURCE_FILES}
)

find_package(SDL2 REQUIRED)
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

target_include_directories(game  
  PUBLIC src
  PUBLIC ${SDL2_INCLUDE_DIRS}
)

set(SDL2_IMAGE_LIBRARIES "/usr/lib/x86_64-linux-gnu/libSDL2_image.so")

target_link_libraries(game
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/assets
)