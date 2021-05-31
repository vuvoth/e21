#include "jubjub/params.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include "utils.hpp"
#include "zkproof/config.h"
#include "zkproof/include/tx_data.h"
#include "zkproof/input_hash.hpp"
#include "zkproof/signature.hpp"
#include "zkproof/transaction.hpp"

#include <algorithm>
#include <ethsnarks.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <string>
#include <stubs.hpp>
#include <utils.hpp>
#include <vector>

using json = nlohmann::json;
using String = std::string;
using std::cout;

void set_up(ethsnarks::ProtoboardT &pb, const int number_tx,
            const VariableT &final_merkle_root,
            const ethsnarks::jubjub::Params &params,
            std::vector<e21::TransactionGadget> &transaction_gadgets) {

  cout << "Setup.... \n";

  for (int i = 0; i < number_tx; ++i) {
    if ((i + 1) % 10 == 0) {
      cout << "==> Setup from tx " << i - 9 << " to " << i << ".....\n";
    }
    if (i == 0) {
      transaction_gadgets.push_back(e21::TransactionGadget(pb, params, "tx"));
    } else {
      transaction_gadgets.push_back(e21::TransactionGadget(
          pb, params,
          transaction_gadgets.back().receiver.zk_merkle_path.result(), "tx"));
    }
    transaction_gadgets.back().generate_r1cs_constraints();
  }

  pb.add_r1cs_constraint(
      ethsnarks::ConstraintT(
          final_merkle_root, FieldT::one(),
          transaction_gadgets.back().receiver.zk_merkle_path.result()),
      "final merkle");
  std::cout << "Total constraints " << pb.num_constraints() << "!...."
            << std::endl;
}

void proof_transaction(
    ethsnarks::ProtoboardT &pb, const ethsnarks::VariableT &final_merkle_root,
    const json data, int number_tx,
    std::vector<e21::TransactionGadget> &transaction_gadgets) {

  pb.val(final_merkle_root) =
      FieldT(data["final_merkle_root"].get<std::string>().c_str());

  // for (int i = 0; i < number_tx; ++i) {
  // e21::TxData tx_data(data["tx" + std::to_string(i)]);
  // std::cout << "........\n";
  // tx_data.sender_proof.merkle_address.print();
  // tx_data.receiver_proof.merkle_address.print();
  // tx_data.amount.print();
  // tx_data.sender.nonce.print();
  //}
  int value = 1;
  std::vector<ethsnarks ::FieldT> values(e21::config::MERKLE_DEEP);
  for (int i = 0; i < e21::config::MERKLE_DEEP; ++i) {
    values[i] = FieldT(std::to_string(value).c_str());
  }

  for (int i = 0; i < number_tx; ++i) {
    e21::TxData tx_data(data["tx" + std::to_string(i)]);
    transaction_gadgets.at(i).generate_r1cs_witness(tx_data);
  }
}

int main(int argc, char **argv) {

  ethsnarks::ppT::init_public_params();
  String sign = argv[1];
  // read transaction proof
  String pk_path = argv[2], vk_path = argv[3];
  String file_path = argv[4];

  std::ifstream is(file_path);
  json txs = json::parse(is);
  is.close();

  int number_tx = txs["number_tx"].get<int>();
  std::vector<e21::TransactionGadget> transaction_gadgets;

  ethsnarks::ProtoboardT pb;
  ethsnarks::jubjub::Params curveParams;
  ethsnarks::VariableT final_merkle_root =
      ethsnarks::make_variable(pb, ".final_merkle_root");

  ethsnarks::VariableArrayT public_hash =
      ethsnarks::make_var_array(pb, 2, "public_hash");

  set_up(pb, number_tx, final_merkle_root, curveParams, transaction_gadgets);

  e21::input_hash input_hash_gadget(pb, transaction_gadgets, public_hash,
                                    "check_inputhash");

  input_hash_gadget.generate_r1cs_constraints();

  pb.set_input_sizes(4);
  
  if (sign.compare("genkey") == 0 || sign.compare("all") == 0) {
    ethsnarks::stub_genkeys_from_pb(pb, pk_path.c_str(), vk_path.c_str());
  }
  

  if (sign.compare("witness") == 0 || sign.compare("all") == 0) {
    proof_transaction(pb, final_merkle_root, txs, number_tx,
                      transaction_gadgets);
    input_hash_gadget.generate_r1cs_witness();
    if (!pb.is_satisfied()) {
      std::cout << "winess not true\n";
      return 1;
    } else {
      String witness_path = argv[5];
      auto json = ethsnarks::stub_prove_from_pb(pb, pk_path.c_str());
      ethsnarks::writeToFile(witness_path.c_str(), json);
    }
  }
  return 0;
}
