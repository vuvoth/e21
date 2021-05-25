#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <zkproof/account.hpp>

using namespace ethsnarks;

TEST(AccountTest, prove_account_sender) {
  ppT::init_public_params();
  ProtoboardT pb;
  jubjub::Params curveParams;

  std::string annotation = "account";

  e21::Account account(pb, annotation);
  /*15567892357950158328174076702272306999716048942239645312988282818616466203732
MerkleProof(leaf=10369667818324323896887277109387174878817546312012550009593689075124224812694,
address=[0, 0],
path=[19321998414906712342737093331922571923461328494325615870852140381009276079041,
17296471688945713021042054900108821045192859417413320566181654591511652308323],
hasher=<ethsnarks.merkletree.MerkleHasher_MiMC object at 0x1059506a0>, width=2)
*/
  FieldT merkle_root = FieldT("144016233623689852628042619555864008260008277463"
                              "19907286134144777205320465013");
  FieldT merkle_position = FieldT("0000");
  FieldT x = FieldT("1097921253835565665058591761593194152060750479952216273004"
                    "9831890951746946604");
  FieldT y = FieldT("5324824554426725410290295966356728817595938893815438489688"
                    "542305809226945840");
  FieldT balance = FieldT("2000");
  FieldT nonce = FieldT("1000");

  FieldT amountValue = FieldT("10");

  VariableT amount(make_variable(pb, amountValue, ".amount"));

  std::vector<FieldT> hash_proof{
      FieldT("19321998414906712342737093331922571923461328494325615870852140381"
             "009276079041"),
      FieldT("17296471688945713021042054900108821045192859417413320566181654591"
             "511652308323")};

  e21::AccountDetail account_detail(x, y, balance, nonce);

  account.generate_r1cs_constraints_send(amount);

  account.generate_r1cs_witness(merkle_root, merkle_position, hash_proof,
                                account_detail, amountValue);

  pb.val(account.state.hasher.result()).print();
  pb.val(account.state.next_hasher.result()).print();
  bool is_existence = account.zk_merkle_existence.is_valid();
  account.current_root_value().print();
  ASSERT_TRUE(is_existence);
}
