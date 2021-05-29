#include <gtest/gtest.h>

#include "zkproof/signature.hpp"
#include <ethsnarks.hpp>
#include <utils.hpp>

TEST(SignatureProver, verirySignature) {

  ethsnarks::ppT::init_public_params();
  std::cout << ethsnarks::FieldT::capacity() << "\n";
  // ethsnarks::ppT::init_public_params();
  // ethsnarks::ProtoboardT pb;
  // ethsnarks::jubjub::Params params;

  // ethsnarks::VariableArrayT message(make_var_array(pb, 12, ".message"));

  // const std::string annotation = "TestRunable";

  // const auto msg = ethsnarks::bytes_to_bv((const uint8_t *)"2a", 2);
  // const auto msg_sign_bits = ethsnarks::make_var_array(pb, msg.size(),
  // "msg");

  // msg_sign_bits.fill_with_bits(pb, msg);

  // const ethsnarks::jubjub::EdwardsPoint pubKey(
  // FieldT("17002285767664649889110177448197328998737537461817974044749636759"
  //"157089730807"),
  // FieldT("13462715376257498982987951911122614658822878439125926054874724264"
  //"566802029873"));
  // const ethsnarks::jubjub::EdwardsPoint R(
  // FieldT("15113412844773325854448466051967465375661695898686485086822041099"
  //"117228475637"),
  // FieldT("18469197527470849250351077798585526419605703495972159547880034601"
  //"175643334070"));

  // const auto s =
  // FieldT("589035397821776268059523352892307953329128385913159450"
  //"5988249149095493083670");

  // e21::SignatureGadget signatureProver(pb, params, msg_sign_bits,
  // annotation);

  // signatureProver.generate_r1cs_constraints();

  // e21::SignatureSchema signatureSchema(pubKey, R, s);

  // signatureProver.generate_r1cs_witness(signatureSchema);
  // EXPECT_EQ(true, pb.is_satisfied());
}
