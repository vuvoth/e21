#include <ethsnarks.hpp>
#include <gadgets/sha256_many.hpp>
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include "utils.hpp"
#include "crypto/sha256.h"
#include "stubs.hpp"

using namespace ethsnarks;

using libsnark::digest_variable;
using libsnark::block_variable;
using libsnark::SHA256_digest_size;
using libsnark::SHA256_block_size;

using ethsnarks::ppT;

static const size_t SHA256_block_size_bytes = SHA256_block_size / 8;
static const size_t SHA256_digest_size_bytes = SHA256_digest_size / 8;

bool test_sha256_many(uint8_t *input_buffer, size_t input_len)
{
    uint8_t output_digest[SHA256_digest_size_bytes];

    // Hash the input, to get the expected result
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, input_buffer, input_len);
    SHA256_Final(output_digest, &ctx);

    ProtoboardT pb;

    // Fill array of input bits
    VariableArrayT block;
    block.allocate(pb, input_len * 8, "block");
    const libff::bit_vector block_bits = bytes_to_bv(input_buffer, input_len);
    block.fill_with_bits(pb, block_bits);

    sha256_many the_gadget(pb, block, "the_gadget");
    the_gadget.generate_r1cs_witness();
    the_gadget.generate_r1cs_constraints();

    // Display the bits for each block
    std::cout << "Input length (bytes): " << input_len << std::endl;
    std::cout << "Input length (bits): " << (input_len * 8) << std::endl;
    std::cout << "Number of blocks: " << the_gadget.m_blocks.size() << std::endl;
    for( size_t b = 0; b < the_gadget.m_blocks.size(); b++ )
    {
        std::cout << "Block " << b << std::endl;
        const auto& block = the_gadget.m_blocks[b];
        for( size_t j = 0; j < SHA256_block_size; j++ )
        {
            std::cout << (pb.val(block[j]) == FieldT::one() ? "1" : "0");
        }
        std::cout << std::endl;
    }

    // ----------------------------------------------------------------    

    // Binds circuit satisfiability to whether or not the full output
    // matches what was computed by OpenSSL's SHA256
    auto output_digest_bits = bytes_to_bv(output_digest, SHA256_digest_size_bytes);

    // Verify the result matches what we computed
    auto full_output_bits = the_gadget.result().get_digest();
    uint8_t full_output_bytes[SHA256_digest_size_bytes];
    bv_to_bytes(full_output_bits, full_output_bytes);
    if( memcmp(full_output_bytes, output_digest, SHA256_digest_size_bytes) != 0 ) {
        std::cerr << "full_output_bytes mismatch" << std::endl;
        print_bytes("Expected: ", SHA256_digest_size_bytes, output_digest);
        print_bytes("Actual: ", SHA256_digest_size_bytes, full_output_bytes);
        return false;
    }

    uint8_t output_buffer[SHA256_digest_size_bytes];
    bv_to_bytes(full_output_bits, output_buffer);
    printf("Output digest bytes: ");
    for( uint8_t x : output_buffer )
    {
        printf("%02X", x);
    }
    printf("\n");
    
    ethsnarks::stub_genkeys_from_pb(pb, "./pk.txt", "./vk.txt");
    auto json =  ethsnarks::stub_prove_from_pb(pb, "./pk.txt");

    writeToFile("./proof.txt", json);
    return pb.is_satisfied();
}

int main() {
    ppT::init_public_params();

    int n = 32;
    uint8_t *buffer = new uint8_t[n];

    // Instead of using random, generate a predictable sequence of bytes
    for( size_t i = 0; i < n; i++ ) {
        buffer[i] = 0 % 0xFF;
    }
    
    buffer[n - 1] = (uint8_t)5;
    
     
    std::cout << test_sha256_many(buffer, 32) << std::endl;

    delete [] buffer;

    return 0;
}
