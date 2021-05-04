#include "libsnark/gadgetlib1/gadget.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"
#include "libsnark/relations/ram_computations/memory/memory_store_trace.hpp"

using namespace libsnark;

namespace proofsystem {

template <typename FieldT>
SimpleSystem<FieldT>::SimpleSystem(protoboard<FieldT> &pb,
                                   const pb_variable<FieldT> &out,
                                   const pb_variable<FieldT> &x)
    : gadget<FieldT>(pb, "poly_simple"), out(out), x(x) {
    square_x.allocate(this->pb, "square_x");
    double_x.allocate(this->pb, "double_x");
    sum_x.allocate(this->pb, "sum_x");
}

/**
 * x^2 + 2x + 30 = out
 */
template <typename FieldT>
void SimpleSystem<FieldT>::generate_r1cs_constraints() {
    this->pb.add_r1cs_constraint(
        r1cs_constraint<FieldT>(this->x, this->x, this->square_x));
    this->pb.add_r1cs_constraint(
        r1cs_constraint<FieldT>(this->x, 2, this->double_x));
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
        this->square_x + this->double_x, 1, this->sum_x));
    this->pb.add_r1cs_constraint(
        r1cs_constraint<FieldT>(this->sum_x + 30, 1, this->out));
}

template <typename FieldT> void SimpleSystem<FieldT>::generate_r1cs_witness() {
    this->pb.val(square_x) = this->pb.val(x) * this->pb.val(x);
    this->pb.val(double_x) = this->pb.val(x) * 2;
    this->pb.val(sum_x) = this->pb.val(square_x) + this->pb.val(double_x);
}
} // namespace proofsystem
