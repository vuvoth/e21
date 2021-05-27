#include "jubjub/params.hpp"
#include "utils.hpp"
#include "zkproof/include/tx_data.h"
#include "zkproof/transaction.hpp"

#include <ethsnarks.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;
using String = std::string;

void set_up(ethsnarks::ProtoboardT &pb, const int number_tx,
            const ethsnarks::jubjub::Params &params,
            std::vector<e21::TransactionGadget> &transaction_gadgets) {
  ethsnarks::VariableT merkle_root =
      ethsnarks::make_variable(pb, "merkle_root");

  for (int i = 0; i < number_tx; ++i) {
    if (i == 0) {
      transaction_gadgets.push_back(e21::TransactionGadget(pb, params, "tx"));
    } else {
      transaction_gadgets.push_back(e21::TransactionGadget(
          pb, params,
          transaction_gadgets.back().receiver.zk_merkle_path.result(), "tx"));
    }
    transaction_gadgets.back().generate_r1cs_constraints();
  }

  std::cout << pb.num_constraints() << std::endl;
}

void proof_transaction(
    const json data, int number_tx,
    std::vector<e21::TransactionGadget> &transaction_gadgets) {
  for (int i = 0; i < number_tx; ++i) {
    e21::TxData tx_data(data["tx" + std::to_string(i)]);
    transaction_gadgets.at(i).generate_r1cs_witness(tx_data);
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

  set_up(pb, number_tx, curveParams, transaction_gadgets);

  proof_transaction(txs, number_tx, transaction_gadgets);
  std::cout << pb.is_satisfied() << "\n";
  return 0;
}
