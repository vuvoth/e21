#include "jubjub/params.hpp"
#include "zkproof/include/tx_data.h"
#include "zkproof/transaction.hpp"

#include <ethsnarks.hpp>
#include <nlohmann/json.hpp>

#include <gtest/gtest.h>

using json = nlohmann::json;

using namespace ethsnarks;

TEST(TransactionGadget_Test, veriry_transaction) {
  ppT::init_public_params();
  ProtoboardT pb;
  ethsnarks::jubjub::Params curve_params;
  e21::TransactionGadget zkTx(pb, curve_params, "transaction");
  zkTx.generate_r1cs_constraints();
}
