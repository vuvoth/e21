include_directories(.)

add_executable(
  main
  main.cpp
)

target_link_libraries(
  main
  ethsnarks_jubjub
)

target_include_directories(
  main
  PUBLIC
  ${DEPENDS_DIR}/ethsnarks
)


