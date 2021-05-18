#ifndef SRC_ZKPROOF_INCLUDE_SIGNATURE_H_
#define SRC_ZKPROOF_INCLUDE_SIGNATURE_H_

#include <jubjub/eddsa.hpp>
#include <jubjub/point.hpp>

#include "tx_data.h"

using String = std::string;
using Protoboard = ethsnarks::ProtoboardT;
using CurveParameter = ethsnarks::jubjub::Params;
using Point = ethsnarks::jubjub::VariablePointT;
using ethsnarks::FieldT;
using ethsnarks::make_var_array;
using ethsnarks::VariableArrayT;
using ethsnarks::jubjub::EdwardsPoint;
using ethsnarks::GadgetT;

namespace e21 {
class SignatureProver : GadgetT{
 public:
  Point pubKey;
  Point R;
  VariableArrayT s;
  VariableArrayT message;

  ethsnarks::jubjub::PureEdDSA zkSignature;

  SignatureProver(ethsnarks::ProtoboardT &in_pb, const CurveParameter &params,
                  const String &annotation_prefix);

  void generate_r1cs_witness(const SignatureSchema &signature);

  void generate_r1cs_constraints();
};

}  // namespace e21

#endif  // SRC_ZKPROOF_INCLUDE_SIGNATURE_H_
