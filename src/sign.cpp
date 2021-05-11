#include "jubjub/eddsa.hpp"
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include "utils.hpp"
#include "stubs.hpp"

using ethsnarks::FieldT;
using ethsnarks::jubjub::PureEdDSA;

int main() {
  ethsnarks::ppT::init_public_params();

  ethsnarks::ProtoboardT pb;

  const ethsnarks::jubjub::Params params;

  const ethsnarks::jubjub::EdwardsPoint A(
      FieldT("333671881179914989291633188949569309119725676183802886621140166987382124337"),
      FieldT("4050436616325076046600891135828313078248584449767955905006778857958871314574"));
    
  const ethsnarks::jubjub::EdwardsPoint B(params.Gx, params.Gy);
  
  const ethsnarks::jubjub::EdwardsPoint R(
          FieldT("17815983127755465894346158776246779862712623073638768513395595796132990361464"), 
          FieldT("947174453624106321442736396890323086851143728754269151257776508699019857364")
          );
 
  const auto s = ethsnarks::make_var_array(pb, FieldT::size_in_bits(), "s");
  s.fill_with_bits_of_field_element(pb, FieldT("13341814865473145800030207090487687417599620847405735706082771659861699337012"));

  const auto msg = ethsnarks::bytes_to_bv((const uint8_t*)"abcd", 4);
  
  const auto msg_sign_bits = ethsnarks::make_var_array(pb, msg.size(), "msg");
  msg_sign_bits.fill_with_bits(pb, msg);    
 

  PureEdDSA sign_verify(pb, params, B, A.as_VariablePointT(pb,"A"), R.as_VariablePointT(pb, "R"), s, msg_sign_bits, "verify");

  sign_verify.generate_r1cs_constraints();

  ethsnarks::stub_genkeys_from_pb(pb, "./pk.txt", "./vk.txt");
 
  sign_verify.generate_r1cs_witness();
  
  auto json = ethsnarks::stub_prove_from_pb(pb, "./pk.txt");
  ethsnarks::writeToFile("./proof.txt", json);
  return 0;
}
