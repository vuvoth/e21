#ifndef SRC_ZKPROOF_TRANSACTION_H_
#define SRC_ZKPROOF_TRANSACTION_H_

#include <util.h>
#include <ethsnarks.hpp>
#include <jubjub/eddsa.hpp>

#include "include/signature.h"
#include "include/tx_data.h"

using ethsnarks::GadgetT;
using ethsnarks::VariableT;

namespace e21 {
class TransactionProver : public GadgetT {

public:
  VariableT merkle_root;

  SignatureProver signatureProver;

  TransactionProver(Protoboard &pb, const CurveParameter &params,
                    const VariableT &merkle_root,
                    const String &annotation_prefix);

  void generate_r1cs_constraints();
  void generate_r1cs_witness(const TxData &tx);
};
} // namespace e21

#endif // SRC_ZKPROOF_TRANSACTION_H_
