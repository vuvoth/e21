#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <zkproof/account.hpp>

using namespace ethsnarks;

TEST(AccountTest, prove_account_sender) {
  ppT::init_public_params();
  ProtoboardT pb;
  jubjub::Params curveParams;

  std::string annotation = "account";

  e21::Account account(pb, annotation);

  FieldT merkle_root = FieldT("");
  FieldT merkle_position = FieldT("");
  FieldT x = FieldT("");
  FieldT y = FieldT("");
  FieldT balance = FieldT("");
  FieldT nonce = FieldT("");
  FieldT amountValue = FieldT("");

  VariableT amount(make_variable(pb, amountValue, ".amount"));

  std::vector<FieldT> hash_proof;
  e21::AccountDetail account_detail(x, y, balance, nonce);

  account.generate_r1cs_constraints_send(amount);

  std::cout << pb.num_constraints() << std::endl;
  bool is_existence = account.zk_merkle_existence.is_valid();
  ASSERT_TRUE(is_existence);
}
