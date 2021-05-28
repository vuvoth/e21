#include "jubjub/params.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include "utils.hpp"
#include "zkproof/config.h"
#include "zkproof/include/tx_data.h"
#include "zkproof/input_checker.hpp"
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
            const VariableArrayT &sender_addresses,
            const VariableArrayT &receiver_addresses,
            const VariableArrayT &amount, const VariableArrayT &nonce,
            const VariableArrayT &bits, const ethsnarks::jubjub::Params &params,
            std::vector<e21::input_checker> &checkers,
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
    checkers.push_back(e21::input_checker(
        pb, sender_addresses[i], receiver_addresses[i], amount[i], nonce[i],
        bits, transaction_gadgets.back()));

    transaction_gadgets.back().generate_r1cs_constraints();
    checkers.back().generate_r1cs_constraints(transaction_gadgets.back());
  }

  pb.add_r1cs_constraint(
      ethsnarks::ConstraintT(
          final_merkle_root, FieldT::one(),
          transaction_gadgets.back().receiver.zk_merkle_path.result()),
      "final merkle");
  std::cout << "Total constraints " << pb.num_constraints() << "!...."
            << std::endl;
}

void proof_transaction(ethsnarks::ProtoboardT &pb,
                       const ethsnarks::VariableT &final_merkle_root,
                       const json data, int number_tx,
                       std::vector<e21::TransactionGadget> &transaction_gadgets,
                       ethsnarks::VariableArrayT &bits,
                       std::vector<e21::input_checker> &checkers) {
  pb.val(final_merkle_root) =
      FieldT(data["final_merkle_root"].get<std::string>().c_str());
  for (int i = 0; i < number_tx; ++i) {
    e21::TxData tx_data(data["tx" + std::to_string(i)]);
    checkers.at(i).set_up_values(tx_data.sender_proof.merkle_address,
                                 tx_data.receiver_proof.merkle_address,
                                 tx_data.amount, tx_data.sender.nonce);
    std::cout << "........\n";
    tx_data.sender_proof.merkle_address.print();
    tx_data.receiver_proof.merkle_address.print();
    tx_data.amount.print();
    tx_data.sender.nonce.print();
  }
  int value = 1;
  std::vector<ethsnarks ::FieldT> values(e21::config::MERKLE_DEEP);
  for (int i = 0; i < e21::config::MERKLE_DEEP; ++i) {
    values[i] = FieldT(std::to_string(value).c_str());
  }
  bits.fill_with_field_elements(pb, values);

  for (int i = 0; i < number_tx; ++i) {
    e21::TxData tx_data(data["tx" + std::to_string(i)]);
    transaction_gadgets.at(i).generate_r1cs_witness(tx_data);
    checkers.at(i).generate_r1cs_witness();
  }
}

int main(int argc, char **argv) {

  ethsnarks::ppT::init_public_params();
  // read transaction proof
  String file_path = argv[1];
  std::ifstream is(file_path);
  json txs = json::parse(is);
  is.close();

  int number_tx = txs["number_tx"].get<int>();

  std::vector<e21::TransactionGadget> transaction_gadgets;

  ethsnarks::ProtoboardT pb;
  ethsnarks::jubjub::Params curveParams;
  ethsnarks::VariableT final_merkle_root =
      ethsnarks::make_variable(pb, ".final_merkle_root");

  ethsnarks::VariableArrayT sender_addresses(
      make_var_array(pb, number_tx, ".sender_address"));
  ethsnarks::VariableArrayT receiver_addresses(
      make_var_array(pb, number_tx, ".receiver_address"));
  ethsnarks::VariableArrayT amount(make_var_array(pb, number_tx, ".amount"));
  ethsnarks::VariableArrayT nonce(make_var_array(pb, number_tx, ".nonce"));
  ethsnarks::VariableArrayT bits(
      make_var_array(pb, e21::config::MERKLE_DEEP, "bits"));

  std::vector<e21 ::input_checker> checkers;

  set_up(pb, number_tx, final_merkle_root, sender_addresses, receiver_addresses,
         amount, nonce, bits, curveParams, checkers, transaction_gadgets);

  // include (sender_address, receiver_address, amount, nonce) * number_tx +
  // old_root, new_root = number_tx * 4 + 2
  pb.set_input_sizes(number_tx * 4 + 2);

  ethsnarks::stub_genkeys_from_pb(pb, "./prover_key.txt", "./verifier_key.txt");

  proof_transaction(pb, final_merkle_root, txs, number_tx, transaction_gadgets,
                    bits, checkers);

  // for (auto e : pb.primary_input()) {
  // e.print();
  //}

  if (!pb.is_satisfied()) {
    std::cout << "winess not true\n";
    return 1;
  } else {
    auto json = ethsnarks::stub_prove_from_pb(pb, "./prover_key.txt");
    ethsnarks::writeToFile("./winess.txt", json);
  }
  return 0;
}
