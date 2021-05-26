#include "zkproof/include/tx_data.h"
#include <gtest/gtest.h>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <vector>
#include <zkproof/account.hpp>

using json = nlohmann::json;

using namespace ethsnarks;

const std::string test_data_file_path = "./src/test/json/account_data.json";

TEST(AccountTest, prove_account_sender) {
  ppT::init_public_params();
  ProtoboardT pb;
  jubjub::Params curveParams;

  std::ifstream is(test_data_file_path);

  auto input = json::parse(is);

  is.close();
  std::string annotation = "account";

  e21::Account account(pb, annotation);

  std::cout << input["amount"].get<std::string>() << std::endl;

  FieldT amountValue = FieldT(input["amount"].get<std::string>().c_str());
  VariableT amount(make_variable(pb, ".amount"));

  account.generate_r1cs_constraints_send(amount);

  e21::MerkleProof merkle_proof(input["sender_proof"]);
  e21::AccountDetail account_detail(input["sender"]);

  account.generate_r1cs_witness_send(merkle_proof, account_detail, amountValue);

  bool is_existence = account.zk_merkle_existence.is_valid();

  account.current_root_value().print();
  ASSERT_TRUE(is_existence);
}
