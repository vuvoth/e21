#include "zkproof/transaction.h"

using ethsnarks::bytes_to_bv;
using ethsnarks::make_var_array;

int main(int argc, char **argv) {

  ethsnarks::ppT::init_public_params();

  ethsnarks::ProtoboardT pb;

  const CurveParameter params;

  const auto merkle_root = ethsnarks::make_variable(pb, "merkle_root");

  e21::TransactionProver tx(pb, params, merkle_root, "Tx");

  tx.generate_r1cs_constraints();

  e21::TxData tx_data;

  tx.generate_r1cs_witness(tx_data);

  return 0;
}
