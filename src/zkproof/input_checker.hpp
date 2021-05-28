#ifndef SRC_ZKPROOF_INPUT_CHECKE_HPP_
#define SRC_ZKPROOF_INPUT_CHECKE_HPP_
#include "config.h"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include "transaction.hpp"
#include "utils.hpp"
#include <ethsnarks.hpp>
#include <string>
#include <vector>
using ethsnarks::FieldT;
using ethsnarks::GadgetT;
using ethsnarks::ProtoboardT;

namespace e21 {

class input_checker : public GadgetT {

public:
  ethsnarks::VariableT sender_address, receiver_address, amount, nonce;
  ethsnarks::VariableArrayT bits;
  libsnark::inner_product_gadget<FieldT> send_checker, receive_checker;

  input_checker(ProtoboardT &pb, const ethsnarks::VariableT &sender_address,
                const ethsnarks::VariableT &receiver_address,
                const ethsnarks::VariableT &amount,
                const ethsnarks::VariableT &nonce,
                const ethsnarks::VariableArrayT &bits,
                const TransactionGadget &transaction_gadget)
      : GadgetT(pb, ".checker"), sender_address(sender_address),
        receiver_address(receiver_address), amount(amount), nonce(nonce),
        bits(bits), send_checker(pb, transaction_gadget.sender.merkle_position,
                                 bits, sender_address, "sender_checker"),
        receive_checker(pb, transaction_gadget.receiver.merkle_position, bits,
                        receiver_address, "sender_checker") {}

  void generate_r1cs_constraints(const TransactionGadget &transaction_gadget) {
    send_checker.generate_r1cs_constraints();
    receive_checker.generate_r1cs_constraints();
    this->pb.add_r1cs_constraint(
        ethsnarks::ConstraintT(this->amount, FieldT::one(),
                               transaction_gadget.amount.packed),
        "check input amount");
    this->pb.add_r1cs_constraint(
        ethsnarks::ConstraintT(this->nonce, FieldT::one(),
                               transaction_gadget.sender.state.nonce.packed),
        "check input nonce");
  }
  void set_up_values(FieldT sender_address, FieldT receiver_address,
                     FieldT amount, FieldT nonce) {

    this->pb.val(this->sender_address) = sender_address;
    this->pb.val(this->receiver_address) = receiver_address;
    this->pb.val(this->amount) = amount;
    this->pb.val(this->nonce) = nonce;
  }

  void generate_r1cs_witness() {
    send_checker.generate_r1cs_witness();
    receive_checker.generate_r1cs_witness();
  }
};

} // namespace e21
#endif
