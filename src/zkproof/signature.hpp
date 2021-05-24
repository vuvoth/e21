#ifndef SRC_ZKPROOF_INCLUDE_SIGNATURE_H_
#define SRC_ZKPROOF_INCLUDE_SIGNATURE_H_

#include <jubjub/eddsa.hpp>
#include <jubjub/point.hpp>

#include "include/tx_data.h"

using String = std::string;
using Protoboard = ethsnarks::ProtoboardT;
using CurveParameter = ethsnarks::jubjub::Params;
using Point = ethsnarks::jubjub::VariablePointT;
using ethsnarks::FieldT;
using ethsnarks::GadgetT;
using ethsnarks::make_var_array;
using ethsnarks::VariableArrayT;
using ethsnarks::jubjub::EdwardsPoint;

namespace e21 {
class SignatureProver : GadgetT {
public:
  Point pubKey;
  Point R;
  VariableArrayT s;
  VariableArrayT message;

  ethsnarks::jubjub::EdDSA zkSignature;

  SignatureProver(Protoboard &pb, const CurveParameter &params,
                  const String &annotation_prefix)

      : GadgetT(pb, annotation_prefix),
        pubKey(pb, FMT(annotation_prefix, ".publicKey")),
        R(pb, FMT(annotation_prefix, ".R")),
        s(make_var_array(pb, ethsnarks::FieldT::size_in_bits(),
                         FMT(annotation_prefix, ".s"))),
        message(make_var_array(pb, ethsnarks::FieldT::size_in_bits(),
                               FMT(annotation_prefix, ".message"))),
        zkSignature(pb, params, EdwardsPoint(params.Gx, params.Gy), pubKey, R,
                    s, message, FMT(annotation_prefix, ".verify")) {}

  void generate_r1cs_witness(const SignatureSchema &signature) {
    this->pb.val(pubKey.x) = signature.pubKey.x;
    this->pb.val(pubKey.y) = signature.pubKey.y;

    this->pb.val(R.x) = signature.R.x;
    this->pb.val(R.y) = signature.R.y;

    s.fill_with_bits_of_field_element(this->pb, signature.s);
    // message in hash
    message.fill_with_bits(this->pb, signature.message);

    zkSignature.generate_r1cs_witness();
  }

  void generate_r1cs_constraints() { zkSignature.generate_r1cs_constraints(); }
};

} // namespace e21

#endif // SRC_ZKPROOF_INCLUDE_SIGNATURE_H_
