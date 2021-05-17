#include "zkproof/transaction.h"

using ethsnarks::bytes_to_bv;
using ethsnarks::make_var_array;

int main(int argc, char **argv) {

  ProtoboardT pb;
  const Params params;

  const auto message_bv = bytes_to_bv((const uint8_t *)"abcdef", 6);

  const auto message = make_var_array(pb, message_bv.size(), "msg"); 
  message.fill_with_bits(pb, message_bv);

  e21::TransactionSnark tx(pb, params, message, "Tx");
  tx.generate_r1cs_constraints();

  e21::TxData tx_data("Hello, world!");

  tx.generate_r1cs_witness(tx_data);

  return 0;
}
