#ifndef SRC_ZKPROOF_INPUT_HASH_
#define SRC_ZKPROOF_INPUT_HASH_

#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include "libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_components.hpp"
#include "zkproof/transaction.hpp"
#include <ethsnarks.hpp>
#include <gadgets/sha256_many.hpp>
#include <utils.hpp>
#include <vector>

using ethsnarks::GadgetT;
using ethsnarks::ProtoboardT;
using ethsnarks::VariableArrayT;
using ethsnarks::VariableT;

namespace e21 {
class input_hash : public GadgetT {
public:
  ethsnarks::sha256_many sha_system;

  libsnark::multipacking_gadget<FieldT> mpack;
  input_hash(ProtoboardT &pb,
             const std::vector<TransactionGadget> &transaction_gadgets,
             VariableArrayT output_hashes, const std::string &annotation)
      : GadgetT(pb, annotation),
        sha_system(
            pb,
            ethsnarks::flatten_reverse(getInputFromGadget(transaction_gadgets)),
            ".sha"),
        mpack(pb, sha_system.result().bits, output_hashes,
              libsnark::SHA256_digest_size / 2, "compare_hash"){};

  std::vector<VariableArrayT>
  getInputFromGadget(const std::vector<TransactionGadget> transaction_gadgets) {
    std::vector<VariableArrayT> result;
    for (auto gadget : transaction_gadgets) {
      result.push_back(gadget.getSenderId().bits);
      result.push_back(gadget.getReceiverId().bits);
      result.push_back(gadget.getAmount().bits);
      result.push_back(gadget.getTransactionNonce().bits);
    }
    return result;
  }

  void generate_r1cs_constraints() {
    sha_system.generate_r1cs_constraints();
    mpack.generate_r1cs_constraints(false);
  }

  void generate_r1cs_witness() {
    sha_system.generate_r1cs_witness();
    mpack.generate_r1cs_witness_from_bits();
  }
};
} // namespace e21
#endif
