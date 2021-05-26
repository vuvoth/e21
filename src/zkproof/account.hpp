#ifndef ZK_PROOF_ACCOUNT_HPP_
#define ZK_PROOF_ACCOUNT_HPP_

#include "config.h"

#include "base_state.hpp"
#include "include/tx_data.h"
#include <ethsnarks.hpp>
#include <gadgets/merkle_tree.hpp>
#include <gadgets/mimc.hpp>
#include <jubjub/point.hpp>

using ethsnarks::merkle_path_authenticator;
using ethsnarks::merkle_path_compute;
using ethsnarks::MiMC_e7_hash_gadget;

namespace e21 {

typedef merkle_path_compute<MiMC_e7_hash_gadget> ZK_Merkle_Path;
typedef merkle_path_authenticator<MiMC_e7_hash_gadget> ZK_Merkle_Existence;

using ethsnarks::FieldT;
using ethsnarks::GadgetT;
using ethsnarks::make_var_array;
using ethsnarks::make_variable;
using ethsnarks::merkle_tree_IVs;
using ethsnarks::ProtoboardT;
using ethsnarks::VariableArrayT;
using ethsnarks::VariableT;

/**
 * proof existence of account
 * proof new hash
 *
 */
class Account : public ethsnarks::GadgetT {

public:
  VariableT merkle_root;
  VariableArrayT merkle_position;
  VariableArrayT hash_proof;
  BaseStateGadget state;

  ZK_Merkle_Path zk_merkle_path;
  ZK_Merkle_Existence zk_merkle_existence;

  Account(ProtoboardT &pb, const std::string &annotation)
      : ethsnarks::GadgetT(pb, annotation),
        merkle_root(make_variable(pb, FMT(annotation, ".merkle_root"))),
        state(pb, FMT(annotation, ".state")),
        merkle_position(
            make_var_array(pb, MERKLE_DEEP, FMT(annotation, ".position"))),
        hash_proof(
            make_var_array(pb, MERKLE_DEEP, FMT(annotation, ".hash_proof"))),
        zk_merkle_path(pb, MERKLE_DEEP, merkle_position, merkle_tree_IVs(pb),
                       state.next_hasher.result(), hash_proof,
                       FMT(annotation, ".new_hash")),
        zk_merkle_existence(pb, MERKLE_DEEP, merkle_position,
                            merkle_tree_IVs(pb), state.hasher.result(),
                            merkle_root, hash_proof,
                            FMT(annotation, ".existence")) {}

  Account(ProtoboardT &pb, VariableT merkle_root, const std::string &annotation)
      : ethsnarks::GadgetT(pb, annotation), merkle_root(merkle_root),
        state(pb, annotation),
        merkle_position(
            make_var_array(pb, MERKLE_DEEP, FMT(annotation, ".position"))),
        hash_proof(
            make_var_array(pb, MERKLE_DEEP, FMT(annotation, ".hash_proof"))),
        zk_merkle_path(pb, MERKLE_DEEP, merkle_position, merkle_tree_IVs(pb),
                       state.next_hasher.result(), hash_proof,
                       FMT(annotation, ".new_hash")),
        zk_merkle_existence(pb, MERKLE_DEEP, merkle_position,
                            merkle_tree_IVs(pb), state.hasher.result(),
                            merkle_root, hash_proof,
                            FMT(annotation, ".existence")) {}

  void generate_r1cs_constraints_state_update() {
    this->zk_merkle_path.generate_r1cs_constraints();
    this->zk_merkle_existence.generate_r1cs_constraints();
  }

  void generate_r1cs_constraints_send(VariableT amount) {
    this->state.generate_r1cs_constraints_send(amount);
    this->generate_r1cs_constraints_state_update();
  }

  void generate_r1cs_constraints_receive(VariableT amount) {
    this->state.generate_r1cs_constraints_receive(amount);
    this->generate_r1cs_constraints_state_update();
  }
  /*
   * verify node and update node
   */
  void generate_r1cs_witness_send(MerkleProof merkle_proof,
                                  AccountDetail account, FieldT amount) {

    this->pb.val(this->merkle_root) = merkle_proof.merkle_root;
    this->hash_proof.fill_with_field_elements(this->pb,
                                              merkle_proof.hash_proof);
    this->merkle_position.fill_with_bits_of_field_element(
        this->pb, merkle_proof.merkle_address);
    this->state.generate_r1cs_witness_send(account, amount);
    zk_merkle_existence.generate_r1cs_witness();
    zk_merkle_path.generate_r1cs_witness();

    this->pb.val(state.next_balance).print();
    this->pb.val(state.next_nonce).print();
  }

  void generate_r1cs_witness_receive(MerkleProof merkle_proof,
                                     AccountDetail account, FieldT amount) {

    this->pb.val(this->merkle_root) = merkle_proof.merkle_root;
    this->hash_proof.fill_with_field_elements(this->pb,
                                              merkle_proof.hash_proof);
    this->merkle_position.fill_with_bits_of_field_element(
        this->pb, merkle_proof.merkle_address);
    this->state.generate_r1cs_witness_receive(account, amount);
    zk_merkle_existence.generate_r1cs_witness();
    zk_merkle_path.generate_r1cs_witness();

    this->pb.val(state.next_balance).print();
    this->pb.val(state.next_nonce).print();
  }
  VariableT current_root() { return zk_merkle_path.result(); }
  FieldT current_root_value() { return pb.val(zk_merkle_path.result()); }
};

} // namespace e21

#endif
