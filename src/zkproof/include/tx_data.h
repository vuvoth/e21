#ifndef SRC_ZKPROOF_INCLUDE_TX_DATA_H_
#define SRC_ZKPROOF_INCLUDE_TX_DATA_H_
#include <algorithm>
#include <ethsnarks.hpp>
#include <jubjub.hpp>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using String = std::string;
namespace e21 {

using ethsnarks::FieldT;
using ethsnarks::jubjub::EdwardsPoint;

class SignatureSchema {
public:
  ethsnarks::jubjub::EdwardsPoint pubKey;
  ethsnarks::jubjub::EdwardsPoint R;
  ethsnarks::FieldT s;

  SignatureSchema(const decltype(pubKey) in_pubKey, const decltype(R) in_R,
                  const decltype(s) in_s)
      : pubKey(in_pubKey), R(in_R), s(in_s) {}

  SignatureSchema(json witness, const String &role)
      : pubKey(FieldT(witness[role]["public_key_x"].get<String>().c_str()),
               FieldT(witness[role]["public_key_y"].get<String>().c_str())),
        R(FieldT(witness["signature"]["R_x"].get<String>().c_str()),
          FieldT(witness["signature"]["R_y"].get<String>().c_str())),
        s(FieldT(witness["signature"]["s"].get<String>().c_str())) {}
};

class MerkleProof {
public:
  ethsnarks::FieldT merkle_root;
  ethsnarks::FieldT merkle_address;
  std::vector<ethsnarks::FieldT> hash_proof;

  MerkleProof(FieldT merkle_root, json witness)
      : merkle_root(merkle_root),
        merkle_address(witness["address"].get<String>().c_str()) {
    this->hash_proof.clear();
    for (auto e : witness["hash_proof"]) {
      this->hash_proof.push_back(FieldT(e.get<String>().c_str()));
    }
  }
  MerkleProof(json witness)
      : merkle_root(FieldT(witness["merkle_root"].get<String>().c_str())),
        merkle_address(witness["address"].get<String>().c_str()) {
    this->hash_proof.clear();
    for (auto e : witness["hash_proof"]) {
      this->hash_proof.push_back(FieldT(e.get<String>().c_str()));
    }
  }

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

  AccountDetail(json witness)
      : public_key(FieldT(witness["public_key_x"].get<String>().c_str()),
                   FieldT(witness["public_key_y"].get<String>().c_str())),
        balance(FieldT(witness["balance"].get<String>().c_str())),
        nonce(FieldT(witness["nonce"].get<String>().c_str())) {}

  AccountDetail(FieldT public_key_x, FieldT public_key_y, FieldT balance,
                FieldT nonce)
      : public_key(public_key_x, public_key_y), balance(balance), nonce(nonce) {
  }
};

class TxData {
public:
  SignatureSchema signature;
  AccountDetail sender;
  AccountDetail receiver;
  FieldT amount;
  MerkleProof sender_proof;
  MerkleProof receiver_proof;

  TxData(AccountDetail sender, AccountDetail receiver, FieldT amount,
         MerkleProof sender_proof, MerkleProof receiver_proof,
         SignatureSchema signature)
      : sender(sender), receiver(receiver), amount(amount),
        signature(signature), sender_proof(sender_proof),
        receiver_proof(receiver_proof) {}

  TxData(json witness)
      : sender(witness["sender"]), receiver(witness["receiver"]),
        sender_proof(witness["sender_proof"]),
        receiver_proof(FieldT("0"), witness["receiver_proof"]),
        signature(witness, "sender"),
        amount(FieldT(witness["amount"].get<String>().c_str())) {}
};

} // namespace e21
#endif
