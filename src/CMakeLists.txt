
add_library(
  templar_lib
  lexer/Lexer.cpp
)

add_library(templar::lib ALIAS templar_lib)
target_include_directories(
  templar_lib
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>"
)

add_executable(
  templar
  templar.cpp
)
target_link_libraries(templar PRIVATE templar::lib)
set_target_properties(templar PROPERTIES OUTPUT_NAME "templar")
