#include <gtest/gtest.h>
#include <zkproof/account.hpp>
using namespace ethsnarks;

TEST(AccountTest, prove_account_existain) {
  ppT::init_public_params();
  ProtoboardT pb;
  jubjub::Params curveParams;

  std::string annotation = "account";

  e21::Account account(pb, annotation);

  account.generate_r1cs_constraints();

  account.generate_r1cs_witness(
      FieldT("19790982751286913289823730641586891858807638933628408575447384959"
             "293803436031"),
      FieldT(
          "000000000000000000000000000000000000000000000000000000000000000000"),
      {FieldT("2447343676970420247355835473667983267115132689045447905848734383"
              "579598297565"),
       FieldT("1729647168894571302104205490010882104519285941741332056618165459"
              "1511652308323")},
      FieldT("16640337849281745063262458055167200627541825733870167042696807719"
             "194826501259"),
      FieldT("24473436769704202473558354736679832671151326890454479058487343835"
             "79598297565"));

  std::cout << "new root";
  pb.val(account.zk_merkle_existence.result()).print();
  bool is_existence = account.zk_merkle_existence.is_valid();
  ASSERT_TRUE(is_existence);
}
