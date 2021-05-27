#ifndef SRC_ZKPROOF_CONFIG_H
#define SRC_ZKPROOF_CONFIG_H
namespace e21 {
namespace config {
const int MERKLE_DEEP = 2;
const int FIELD_SIZE = 2;
const int MESSAGE_SIZE = MERKLE_DEEP + MERKLE_DEEP + FIELD_SIZE + MERKLE_DEEP;
} // namespace config
} // namespace e21
#endif
