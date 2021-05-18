#include "zkproof/transaction.h"

using ethsnarks::bytes_to_bv;
using ethsnarks::make_var_array;

int main(int argc, char **argv) {
  std::cout << argc << argv[0] << std::endl;

  ethsnarks::ppT::init_public_params();
  ethsnarks::ProtoboardT pb;
  const CurveParameter params;

  // const auto message_bv = bytes_to_bv((const uint8_t *)"abcdef", 6);

  // const auto message_bits = make_var_array(pb, message_bv.size(), "msg");
  // message_bits.fill_with_bits(pb, message_bv);

  const auto merkle_root = ethsnarks::make_variable(pb, "merkle_root");

  e21::TransactionProver tx(pb, params, merkle_root, "Tx");
  /*
    tx.generate_r1cs_constraints();

    e21::TxData tx_data("Hello, world!");

    tx.generate_r1cs_witness(tx_data);
    */
  return 0;
}
