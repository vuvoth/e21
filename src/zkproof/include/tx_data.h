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
     : pubKey(in_pubKey), R(in_R), s(in_s), message(in_message){}
  void setMessage() {}
};

class TxData {};
}  // namespace e21
#endif
