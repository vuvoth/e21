#ifndef E21_ZKPROOF_TRANSACTION_
#define E21_ZKPROOF_TRANSACTION_

#include "transaction.h"

using ethsnarks::ProtoboardT;

namespace e21 {
// constructor of Transaction Prover
TransactionProver::TransactionProver(ProtoboardT &pb,
                                     const CurveParameter &params,
                                     const VariableT &merkle_root,
                                     const String &annotation_prefix)
    : GadgetT(pb, annotation_prefix), merkle_root(merkle_root),
      signatureProver(pb, params, FMT(annotation_prefix, ".signature")) {}

void TransactionProver::generate_r1cs_constraints() {
  signatureProver.generate_r1cs_constraints();
}

void TransactionProver::generate_r1cs_witness(const TxData &tx_data) {}

} // namespace e21
#endif
