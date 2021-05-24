#ifndef SRC_ZKPROOF_INCLUDE_TX_DATA_H_
#define SRC_ZKPROOF_INCLUDE_TX_DATA_H_
#include <ethsnarks.hpp>
#include <jubjub.hpp>
#include <string>

using String = std::string;
namespace e21 {

class SignatureSchema {
public:
  ethsnarks::jubjub::EdwardsPoint pubKey;
  ethsnarks::jubjub::EdwardsPoint R;
  ethsnarks::FieldT s;
  libff::bit_vector message;

  SignatureSchema(const decltype(pubKey) in_pubKey, const decltype(R) in_R,
                  const decltype(s) in_s, const decltype(message) in_message)
      : pubKey(in_pubKey), R(in_R), s(in_s), message(in_message) {}
  void setMessage() {}
};

class MerkleProof {
public:
  ethsnarks::FieldT old_leaf;
  ethsnarks::FieldT new_leaf;
  ethsnarks::FieldT merkle_root;
  ethsnarks::FieldT merkle_address;
  std::vector<ethsnarks::FieldT> hash_proof;

  MerkleProof(const ethsnarks::FieldT _merkle_root,
              ethsnarks::FieldT _merkle_address,
              std::vector<ethsnarks::FieldT> _hash_proof,
              ethsnarks::FieldT _old_leaf, ethsnarks::FieldT _new_leaf)
      : old_leaf(_old_leaf), new_leaf(_new_leaf),
        merkle_address(_merkle_address), hash_proof(_hash_proof),
        merkle_root(_merkle_root) {}
};

class TxData {};
} // namespace e21
#endif
