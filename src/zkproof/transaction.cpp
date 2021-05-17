#include "transaction.h"

using String = std::string;
namespace e21 {
TransactionSnark::TransactionSnark(ProtoboardT &pb, const Params &params, const VariableArrayT& message,
                                   const String &annotation_prefix)
    : GadgetT(pb, annotation_prefix) {
                        
    }

void TransactionSnark::generate_r1cs_constraints() {
  std::cout << "Generate constraints" << std::endl;
}

void TransactionSnark::generate_r1cs_witness(const TxData& tx_data) {
  std::cout << "Generate witness" << std::endl;
}

} // namespace e21

