#ifndef E21_ZKPROOF_SIGNATURE_CPP_
#define E21_ZKPROOF_SIGNATURE_CPP_

#include "include/signature.h"

#include <ethsnarks.hpp>
#include <utils.hpp>

namespace e21 {

const int MAX_MSG_LEN = 32;
using namespace ethsnarks;
//
SignatureProver::SignatureProver(Protoboard &pb, const CurveParameter &params,
                                 const String &annotation_prefix)

    : GadgetT(pb, annotation_prefix),
      pubKey(pb, FMT(annotation_prefix, ".publicKey")),
      R(pb, FMT(annotation_prefix, ".R")),
      s(make_var_array(pb, ethsnarks::FieldT::size_in_bits(),
                       FMT(annotation_prefix, ".s"))),
      message(
          make_var_array(pb, MAX_MSG_LEN, FMT(annotation_prefix, ".message"))),
      zkSignature(pb, params, EdwardsPoint(params.Gx, params.Gy), pubKey, R, s,
                  message, FMT(annotation_prefix, ".verify")) {}

void SignatureProver::generate_r1cs_constraints() {
  zkSignature.generate_r1cs_constraints();
}

void SignatureProver::generate_r1cs_witness(const SignatureSchema &signature) {
  this->pb.val(pubKey.x) = signature.pubKey.x;
  this->pb.val(pubKey.y) = signature.pubKey.y;

  this->pb.val(R.x) = signature.R.x;
  this->pb.val(R.y) = signature.R.y;

  s.fill_with_bits_of_field_element(this->pb, signature.s);
  message.fill_with_bits(this->pb, signature.message);

  zkSignature.generate_r1cs_witness();
}

}  // namespace e21

#endif
