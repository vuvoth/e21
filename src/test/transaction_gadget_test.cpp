#include "jubjub/params.hpp"
#include "zkproof/include/tx_data.h"
#include "zkproof/transaction.hpp"

#include <ethsnarks.hpp>
#include <nlohmann/json.hpp>

#include <gtest/gtest.h>

using json = nlohmann::json;

using namespace ethsnarks;

const std::string test_data_file_path = "./src/test/json/tx_simple.json";

TEST(TransactionGadget_Test, veriry_transaction) {
  ppT::init_public_params();
  ProtoboardT pb;
  ethsnarks::jubjub::Params curve_params;

  e21::TransactionGadget zkTx(pb, curve_params, "transaction");

  std::ifstream is(test_data_file_path);

  auto input = json::parse(is);
  is.close();

  zkTx.generate_r1cs_constraints();

  e21::TxData txdata(input);

  zkTx.generate_r1cs_witness(txdata);

  ASSERT_TRUE(pb.is_satisfied());
}
