#ifndef SRC_ZKPROOF_TRANSACTION_HPP_
#define SRC_ZKPROOF_TRANSACTION_HPP_

#include <util.h>

#include <ethsnarks.hpp>
#include <jubjub/eddsa.hpp>

#include "account.hpp"
#include "include/tx_data.h"
#include "signature.hpp"

using ethsnarks::GadgetT;
using ethsnarks::VariableT;

/*
 * Transaction gadget
 */
namespace e21 {
class TransactionGadget : public GadgetT {
public:
  VariableT amount;
  Account sender, receiver;

  TransactionGadget(ProtoboardT &pb, const std::string &annotation)
      : GadgetT(pb, annotation), amount(make_variable(pb, ".amount")),
        sender(pb, FMT(annotation, ".sender")),
        receiver(pb, sender.current_root(), FMT(annotation, ".receiver")) {}

  void generate_r1cs_constraints() {
    sender.generate_r1cs_constraints_send(amount);
    receiver.generate_r1cs_constraints_receive(amount);
  }

  void generate_r1cs_witness(TxData tx) {
    this->pb.val(this->amount) = tx.amount;
    //    this->sender.generate_r1cs_witness();
    //  this->receiver.generate_r1cs_witness();
  }
};
} // namespace e21

#endif // SRC_ZKPROOF_TRANSACTION_HPP_
