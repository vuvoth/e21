include_directories(.)

add_executable(
  proof_hash
  proof_hash.cpp
)

target_link_libraries(
  proof_hash
  ethsnarks_jubjub
)

target_include_directories(
  proof_hash 
  PUBLIC
  ${DEPENDS_DIR}/ethsnarks
)


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

add_executable(
  sign 
  sign.cpp
)

target_link_libraries(
  sign 
  ethsnarks_jubjub
)

target_include_directories(
  sign 
  PUBLIC
  ${DEPENDS_DIR}/ethsnarks
)

