#ifndef SRC_ZKPROOF_BASE_STATE_HPP_
#define SRC_ZKPROOF_BASE_STATE_HPP_

#include "config.h"
#include "gadgets/merkle_tree.hpp"
#include "gadgets/mimc.hpp"
#include "include/tx_data.h"
#include "jubjub/point.hpp"
#include "libsnark/gadgetlib1/gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include <ethsnarks.hpp>
#include <string>
#include <utils.hpp>

namespace e21 {

using ethsnarks::ConstraintT;
using ethsnarks::FieldT;
using ethsnarks::GadgetT;
using ethsnarks::make_variable;
using ethsnarks::MiMC_e7_hash_gadget;
using ethsnarks::ProtoboardT;
using ethsnarks::VariableArrayT;
using ethsnarks::VariableT;
using ethsnarks::jubjub::VariablePointT;

class BaseStateGadget : public GadgetT {
public:
  VariablePointT publicKey;
  VariableT balanace;
  VariableT next_balance;
  libsnark::dual_variable_gadget<FieldT> nonce;
  VariableT next_nonce;
  MiMC_e7_hash_gadget hasher;
  MiMC_e7_hash_gadget next_hasher;

  BaseStateGadget(ProtoboardT &pb, const std::string annotation)
      : GadgetT(pb, annotation), publicKey(pb, FMT(annotation, ".publicKey")),
        nonce(pb, config::MERKLE_DEEP, ".nonce"),
        balanace(make_variable(pb, FMT(annotation, ".balanace"))),
        next_nonce(make_variable(pb, ".nonce")),
        next_balance(make_variable(pb, FMT(annotation, ".next_balanace"))),
        hasher(pb, make_variable(pb, FieldT::one(), "iv"),
               {publicKey.x, publicKey.y, balanace, nonce.packed},
               FMT(annotation, ".hash")),
        next_hasher(pb, make_variable(pb, FieldT::one(), "iv"),
                    {publicKey.x, publicKey.y, next_balance, next_nonce},
                    FMT(annotation, ".hash")) {}

  void generate_r1cs_constraints_send(
      libsnark::dual_variable_gadget<FieldT> amount) {
    this->pb.add_r1cs_constraint(ConstraintT(this->nonce.packed + FieldT::one(),
                                             FieldT::one(), this->next_nonce),
                                 "nonce + 1 = next_nonce");
    this->pb.add_r1cs_constraint(ConstraintT(this->balanace - amount.packed,
                                             FieldT::one(), this->next_balance),
                                 "balance - amount = next_balance");
    this->generate_r1cs_hasher_constraints();
  }

  void generate_r1cs_constraints_receive(
      libsnark::dual_variable_gadget<FieldT> amount) {
    this->pb.add_r1cs_constraint(
        ConstraintT(this->nonce.packed, FieldT::one(), this->next_nonce),
        "nonce = next_nonce");
    this->pb.add_r1cs_constraint(ConstraintT(this->balanace + amount.packed,
                                             FieldT::one(), this->next_balance),
                                 "balance + amount = next_balance");
    this->generate_r1cs_hasher_constraints();
  }

  void generate_r1cs_hasher_constraints() {
    this->nonce.generate_r1cs_constraints(true);
    this->hasher.generate_r1cs_constraints();
    this->next_hasher.generate_r1cs_constraints();
  }

  void generate_r1cs_witness_send(AccountDetail account, FieldT amount) {
    this->pb.val(this->publicKey.x) = account.public_key.x;
    this->pb.val(this->publicKey.y) = account.public_key.y;
    this->pb.val(this->nonce.packed) = account.nonce;
    this->pb.val(this->balanace) = account.balance;
    this->pb.val(this->next_nonce) = account.nonce + 1;
    this->pb.val(this->next_balance) = account.balance - amount;
    this->nonce.generate_r1cs_witness_from_packed();
    hasher.generate_r1cs_witness();
    next_hasher.generate_r1cs_witness();
  }

  void generate_r1cs_witness_receive(AccountDetail account, FieldT amount) {
    this->pb.val(this->publicKey.x) = account.public_key.x;
    this->pb.val(this->publicKey.y) = account.public_key.y;
    this->pb.val(this->nonce.packed) = account.nonce;
    this->pb.val(this->balanace) = account.balance;
    this->pb.val(this->next_nonce) = account.nonce;
    this->pb.val(this->next_balance) = account.balance + amount;
    this->nonce.generate_r1cs_witness_from_packed();
    hasher.generate_r1cs_witness();
    next_hasher.generate_r1cs_witness();
  }
};
} // namespace e21
#endif
