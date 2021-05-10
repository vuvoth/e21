#include "libff/algebra/curves/public_params.hpp"
#include "libsnark/gadgetlib1/gadget.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"
#include "libsnark/gadgetlib1/protoboard.hpp"
#include <cassert>
#include <iostream>
#include <memory>


namespace proofsystem {

using namespace libsnark;

template <typename FieldT> class SimpleSystem : public gadget<FieldT> {
  private:
    pb_variable<FieldT> square_x;
    pb_variable<FieldT> double_x;
    pb_variable<FieldT> sum_x;

  public:
    const pb_variable<FieldT> out;
    const pb_variable<FieldT> x;

    SimpleSystem(protoboard<FieldT> &pb, const pb_variable<FieldT> &out,
                 const pb_variable<FieldT> &x);
    /**
     * build constraint system.
     */
    void generate_r1cs_constraints();

    // fill value for all node and generate withness
    void generate_r1cs_witness();
};
} // namespace proofsystem

#include "simple_system.tcc"
