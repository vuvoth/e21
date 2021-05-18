include_directories(.)

#add_executable(
#  proof_hash
#  proof_hash.cpp
#)

#target_link_libraries(
#  proof_hash
#  ethsnarks_jubjub
#)

#target_include_directories(
#  proof_hash 
#  PUBLIC
#  ${DEPENDS_DIR}/ethsnarks
#)


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

#add_executable(
#  sign 
#  sign.cpp
#)

#target_link_libraries(
#  sign 
#  ethsnarks_jubjub
#)

#target_include_directories(
#  sign 
#  PUBLIC
#  ${DEPENDS_DIR}/ethsnarks
#)

