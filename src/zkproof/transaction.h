#ifndef E21_CORE_TRANSACTION_H_
#define E21_CORE_TRANSACTION_H_

#include "ethsnarks.hpp"
#include "jubjub/eddsa.hpp"
#include "tx_data.h"

using ethsnarks::GadgetT;
using ethsnarks::ProtoboardT;
using ethsnarks::jubjub::Params;
using ethsnarks::jubjub::VariablePointT;
using ethsnarks::VariableT;
using ethsnarks::VariableArrayT;

namespace e21 {
class TransactionSnark : public GadgetT {
public:
  TransactionSnark(ProtoboardT &pb, const Params& params, const VariableArrayT& message, const std::string& annotation_prefix);

  void generate_r1cs_constraints();
  void generate_r1cs_witness(const TxData& tx);
};
} // namespace e21
#endif
