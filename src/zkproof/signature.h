#ifndef E21_CORE_SIGNATURE_H_
#define E21_CORE_SIGNATURE_H_

#include "jubjub/point.hpp"
#include "util.h"

namespace e21 {
class Signature {
public:
  ethsnarks::jubjub::VariablePointT R;
  ethsnarks::VariableArrayT s;

  Signature(ethsnarks::jubjub::EdwardsPoint R, ethsnarks::FieldT s);

  void generate_verify_signature_witness();
};
} // namespace e21
#endif
