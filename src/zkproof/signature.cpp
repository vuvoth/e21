#ifndef E21_ZKPROOF_SIGNATURE_CPP_
#define E21_ZKPROOF_SIGNATURE_CPP_

#include "include/signature.h"

#include <ethsnarks.hpp>

#include "utils.hpp"

namespace e21 {
SignatureProver::SignatureProver(Protoboard &in_pb,
                                 const CurveParameter &params,
                                 const String &annotation_prefix)
    : pubKey(in_pb, FMT(annotation_prefix, ".publicKey")),
      R(in_pb, FMT(annotation_prefix, ".R")),
      s(make_var_array(in_pb, ethsnarks::FieldT::size_in_bits(),
                       FMT(annotation_prefix, ".s"))),
      message(make_var_array(in_pb, ethsnarks::FieldT::size_in_bits(),
                             FMT(annotation_prefix, ".message"))),
      zkSignature(in_pb, params, EdwardsPoint(params.Gx, params.Gy), pubKey, R,
                  s, message, FMT(annotation_prefix, ".verify")) {}

void SignatureProver::generate_r1cs_constraints() {
  zkSignature.generate_r1cs_constraints();
}
void SignatureProver::generate_r1cs_witness() {
  zkSignature.generate_r1cs_witness();
}
}  // namespace e21

#endif
