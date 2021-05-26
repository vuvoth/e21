#ifndef SRC_ZKPROOF_TRANSACTION_HPP_
#define SRC_ZKPROOF_TRANSACTION_HPP_

#include <util.h>

#include <ethsnarks.hpp>
#include <jubjub/eddsa.hpp>

#include "account.hpp"
#include "include/tx_data.h"
#include "jubjub/params.hpp"
#include "signature.hpp"
#include "utils.hpp"

using ethsnarks::GadgetT;
using ethsnarks::VariableT;
using ethsnarks::jubjub::Params;
/*
 * Transaction gadget
 */
namespace e21 {
class TransactionGadget : public GadgetT {
public:
  Account sender, receiver;
  VariableT amount;
  SignatureGadget signatureGadget;

  TransactionGadget(ProtoboardT &pb, const Params &params,
                    const std::string &annotation)
      : GadgetT(pb, annotation), amount(make_variable(pb, ".amount")),
        sender(pb, FMT(annotation, ".sender")),
        receiver(pb, sender.current_root(), FMT(annotation, ".receiver")),
        signatureGadget(pb, params,
                        make_var_array(pb, FieldT::size_in_bits() * 4,
                                       FMT(annotation, ".message")),
                        FMT(annotation, ".signature")) {}

  void generate_r1cs_constraints() {
    signatureGadget.generate_r1cs_constraints();
    sender.generate_r1cs_constraints_send(amount);
    receiver.generate_r1cs_constraints_receive(amount);
  }

  void generate_r1cs_witness(TxData tx) {
    this->signatureGadget.generate_r1cs_witness(tx.signature);
    this->pb.val(this->amount) = tx.amount;
    this->sender.generate_r1cs_witness_send(tx.sender_proof, tx.sender,
                                            tx.amount);
    this->receiver.generate_r1cs_witness_receive(tx.receiver_proof, tx.receiver,
                                                 tx.amount);
  }
};
} // namespace e21

#endif // SRC_ZKPROOF_TRANSACTION_HPP_
