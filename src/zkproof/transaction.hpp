#ifndef SRC_ZKPROOF_TRANSACTION_HPP_
#define SRC_ZKPROOF_TRANSACTION_HPP_

#include <util.h>

#include <ethsnarks.hpp>
#include <jubjub/eddsa.hpp>

#include "account.hpp"
#include "include/tx_data.h"
#include "signature.hpp"

using ethsnarks::GadgetT;
using ethsnarks::VariableT;

/*
 * Transaction gadget
 */
namespace e21 {
class TransactionGadget : public GadgetT {
public:
  Account sender, receiver;

  TransactionGadget(ProtoboardT &pb, const std::string &annotation)
      : GadgetT(pb, annotation), sender(pb, FMT(annotation, ".sender")),
        receiver(pb, sender.current_root(), FMT(annotation, ".receiver")) {}

  void generate_r1cs_constraints() {
    sender.generate_r1cs_constraints();
    receiver.generate_r1cs_constraints();
  }

  void generate_r1cs_witness(const MerkleProof &sender_proof,
                             const MerkleProof &receiver_proof) {
    sender.generate_r1cs_witness(
        sender_proof.merkle_root, sender_proof.merkle_address,
        sender_proof.hash_proof, sender_proof.old_leaf, sender_proof.new_leaf);

    sender.current_root_value().print();
    receiver.generate_r1cs_witness(
        sender.current_root_value(), receiver_proof.merkle_address,
        receiver_proof.hash_proof, receiver_proof.old_leaf,
        receiver_proof.new_leaf);
  }
};
} // namespace e21

#endif // SRC_ZKPROOF_TRANSACTION_HPP_
