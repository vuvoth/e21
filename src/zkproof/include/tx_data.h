#ifndef SRC_ZKPROOF_INCLUDE_TX_DATA_H_
#define SRC_ZKPROOF_INCLUDE_TX_DATA_H_
#include <ethsnarks.hpp>
#include <jubjub.hpp>
#include <string>

using String = std::string;
namespace e21 {

using ethsnarks::FieldT;
using ethsnarks::jubjub::EdwardsPoint;

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
  ethsnarks::FieldT merkle_root;
  ethsnarks::FieldT merkle_address;
  std::vector<ethsnarks::FieldT> hash_proof;

  MerkleProof(const ethsnarks::FieldT _merkle_root,
              ethsnarks::FieldT _merkle_address,
              std::vector<ethsnarks::FieldT> _hash_proof)
      : merkle_address(_merkle_address), hash_proof(_hash_proof),
        merkle_root(_merkle_root) {}
};

/*
 * account detail
 */
class AccountDetail {
public:
  ethsnarks::jubjub::EdwardsPoint public_key;
  ethsnarks::FieldT balance;
  ethsnarks::FieldT nonce;

  AccountDetail(FieldT public_key_x, FieldT public_key_y, FieldT balance,
                FieldT nonce)
      : public_key(public_key_x, public_key_y), balance(balance), nonce(nonce) {
  }
};

class TxData {
public:
  TxData(AccountDetail sender, AccountDetail receiver, FieldT amount,
         MerkleProof sender_proof, MerkleProof receiver_proof,
         SignatureSchema signature)
      : sender(sender), receiver(receiver), amount(amount),
        signature(signature), sender_proof(sender_proof),
        receiver_proof(receiver_proof) {}
  SignatureSchema signature;
  AccountDetail sender;
  AccountDetail receiver;
  FieldT amount;
  MerkleProof sender_proof;
  MerkleProof receiver_proof;
};
} // namespace e21
#endif
