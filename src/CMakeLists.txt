include_directories(.)


add_subdirectory(test)
add_subdirectory(zkproof)

add_executable(
  main
  main.cpp
  zkproof/transaction.cpp
  zkproof/signature.cpp
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

