#include "zkproof/include/tx_data.h"
#include <gtest/gtest.h>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <vector>
#include <zkproof/account.hpp>

using json = nlohmann::json;

using namespace ethsnarks;

const std::string test_data_file_path = "./src/test/json/account_data.json";
