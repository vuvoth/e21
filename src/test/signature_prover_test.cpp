#include <gtest/gtest.h>

#include <ethsnarks.hpp>

#include "zkproof/signature.hpp"

TEST(SignatureProver, verirySignature) {
  ethsnarks::ppT::init_public_params();
  ethsnarks::ProtoboardT pb;
  ethsnarks::jubjub::Params params;

  //  const std::string annotation = "TestRunable";

  // e21::SignatureGadget signatureProver(pb, params, annotation);

  // signatureProver.generate_r1cs_constraints();

  // const ethsnarks::jubjub::EdwardsPoint pubKey(
  // FieldT("33367188117991498929163318894956930911972567618380288662114016698"
  //"7382124337"),
  // FieldT("40504366163250760466008911358283130782485844497679559050067788579"
  //"58871314574"));

  // const ethsnarks::jubjub::EdwardsPoint R(
  // FieldT("17815983127755465894346158776246779862712623073638768513395595796"
  //"132990361464"),
  // FieldT("94717445362410632144273639689032308685114372875426915125777650869"
  //"9019857364"));

  // const auto s =
  // FieldT("133418148654731458000302070904876874175996208474057357"
  //"06082771659861699337012");

  // const auto msg = ethsnarks::bytes_to_bv((const uint8_t *)"abcd", 4);
  // const auto msg_sign_bits = ethsnarks::make_var_array(pb, msg.size(),
  // "msg"); msg_sign_bits.fill_with_bits(pb, msg);

  // e21::SignatureSchema signatureSchema(pubKey, R, s, msg);

  // signatureProver.generate_r1cs_witness(signatureSchema);
  // EXPECT_EQ(true, pb.is_satisfied());
}
