include_directories(.)

add_subdirectory(test)
add_subdirectory(zkproof)

add_executable(
  e21 
  main.cpp
)

target_link_libraries(
  e21 
  ethsnarks_jubjub
)

target_include_directories(
  e21 
  PUBLIC
  ${DEPENDS_DIR}/ethsnarks
)

