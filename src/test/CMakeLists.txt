include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
  signature_prover_test
  signature_prover_test.cpp 
)

target_link_libraries(
  signature_prover_test
  ethsnarks_jubjub
  gtest_main
)

add_executable(
  account_test
  account_test.cpp 
)

target_link_libraries(
  account_test 
  ethsnarks_jubjub
  gtest_main
)


add_executable(
  transaction_gadget_test 
  transaction_gadget_test.cpp 
)

target_link_libraries(
  transaction_gadget_test
  ethsnarks_jubjub
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(signature_prover_test)

