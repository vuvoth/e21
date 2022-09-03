#ifndef SRC_ZKPROOF_TRANSACTION_HPP_
#define SRC_ZKPROOF_TRANSACTION_HPP_


#include <ethsnarks.hpp>
#include <jubjub/eddsa.hpp>

#include "account.hpp"
#include "include/tx_data.h"
#include "jubjub/params.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include "libsnark/gadgetlib1/gadgets/hashes/hash_io.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"
#include "signature.hpp"
#include "utils.hpp"
#include "zkproof/config.h"

using ethsnarks::GadgetT;
using ethsnarks::LinearCombinationT;
using ethsnarks::VariableT;
using ethsnarks::jubjub::Params;
/*
 * Transaction gadget
 */
namespace e21 {
class TransactionGadget : public GadgetT {

public:
  Account sender, receiver;
  libsnark::dual_variable_gadget<FieldT> amount;
  VariableArrayT message;
  SignatureGadget signatureGadget;

  TransactionGadget(ProtoboardT &pb, const Params &params,
                    const std::string &annotation)
      : GadgetT(pb, annotation), amount(pb, config::FIELD_SIZE, ".amount"),
        sender(pb, FMT(annotation, ".sender")),
        receiver(pb, sender.current_root(), FMT(annotation, ".receiver")),
        message(ethsnarks::flatten({sender.account_id.bits,
                                    receiver.account_id.bits, amount.bits,
                                    sender.state.nonce.bits})),
        signatureGadget(pb, params, message, FMT(annotation, ".signature")) {}

  TransactionGadget(ProtoboardT &pb, const Params &params,
                    const VariableT &merkle_root, const std::string &annotation)
      : GadgetT(pb, annotation), amount(pb, config::FIELD_SIZE, ".amount"),
        sender(pb, merkle_root, FMT(annotation, ".sender")),
        receiver(pb, sender.current_root(), FMT(annotation, ".receiver")),
        message(ethsnarks::flatten({sender.account_id.bits,
                                    receiver.account_id.bits, amount.bits,
                                    sender.state.nonce.bits})),
        signatureGadget(pb, params, message, FMT(annotation, ".signature")) {}

  libsnark::dual_variable_gadget<FieldT> getSenderId() const {
    return this->sender.account_id;
  }

  libsnark::dual_variable_gadget<FieldT> getReceiverId() const {
    return this->receiver.account_id;
  }
  libsnark::dual_variable_gadget<FieldT> getTransactionNonce() const {
    return this->sender.state.nonce;
  }
  libsnark::dual_variable_gadget<FieldT> getAmount() const {
    return this->amount;
  }
  void generate_r1cs_constraints() {
    this->amount.generate_r1cs_constraints(true);
    signatureGadget.generate_r1cs_constraints();
    sender.generate_r1cs_constraints_send(this->amount);
    receiver.generate_r1cs_constraints_receive(this->amount);
  }

  void generate_r1cs_witness(TxData tx) {
    this->pb.val(amount.packed) = tx.amount;
    this->amount.generate_r1cs_witness_from_packed();
    this->sender.generate_r1cs_witness_send(tx.sender_proof, tx.sender,
                                            tx.amount);
    tx.receiver_proof.merkle_root = sender.current_root_value();

    this->receiver.generate_r1cs_witness_receive(tx.receiver_proof, tx.receiver,
                                                 tx.amount);

    this->signatureGadget.generate_r1cs_witness(tx.signature);
  }
};
} // namespace e21

#endif // SRC_ZKPROOF_TRANSACTION_HPP_
