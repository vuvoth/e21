#ifndef SRC_ZKPROOF_UTIL_HPP_
#define SRC_ZKPROOF_UTIL_HPP_
#include <ethsnarks.hpp>
#include <vector>

inline std::vector<ethsnarks ::FieldT>
stringVecToFieldVec(std::vector<char *> strVec) {
  std::vector<ethsnarks::FieldT> result;
  for (auto e : strVec) {
    result.push_back(ethsnarks::FieldT(e));
  }
  return result;
}
#endif
