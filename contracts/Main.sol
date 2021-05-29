pragma solidity ^0.5.0;

import "../libs/ethsnarks/contracts/Verifier.sol";


contract Main {
  uint256 constant public MERKLE_DEEP = 16; 
  uint256 constant public AMOUNT_SIZE = 32;
  uint256 constant public NUMBER_TX_PRE_BATCH = 60;
  uint256 constant public NUMBER_INPUT = NUMBER_TX_PRE_BATCH * 4 + 2;

  struct LeafData{
    uint256 pubkey_x; 
    uint256 pubkey_y; 
    uint256 amount; 
    uint256 nonce;
  }
  
  mapping(address => LeafData) public zkData;
  bytes32 public zkroot;
  
  constructor(bytes32 _zkroot) public {
    zkroot = _zkroot;
  }
  
  //function slice(bytes memory b, uint begin, uint end) public pure returns(uint256) {
    //bytes32 result; 
    //for (uint i = 0; i + begin < end; ++i) {
      //result[32 - i - 1] = b[i]; 
    //}
    //return uint256(result);
  //}

  //function decodeTxPacked(bytes memory txs) public pure returns(uint256[NUMBER_TX_PRE_BATCH *4 + 2] memory){
    //uint256[NUMBER_TX_PRE_BATCH] memory result;
    //uint256 begin;
    //uint256 end; 

    //begin = 0;
    //end = 32;

    ////result[0] = slice(txs, begin, end);
    ////for (int i = 0; i < NUMBER_TX_PRE_BATCH * 4; ++i) {
      ////result[i] = slice(b, )
    ////}
    //return result;
  //}


  function splitToTwo128(bytes32 y) public pure returns(bytes16, bytes16) {
    return (bytes16(y << 128),bytes16((y >> 128) << 128));
  }

  function toLibsnarkBits(bytes16 y) public pure returns(bytes16){
    uint128 k = 0;
    uint128 itow = 1 << 127;
    uint128 tmp = uint128(y);
    for (uint i = 0; i < 128; ++i) {
      k = k + uint128(tmp & 1) * itow;
      itow = itow >> 1;
      tmp = tmp >> 1;
    }
    return bytes16(k);
  }

  function verifyTx(
    uint256[14] memory in_vk, 
    uint256[] memory vk_gammaABC, 
    uint256[8] memory in_proof,
    bytes memory transaction_inputs,
    uint256 old_root, 
    uint256 new_root
  ) public returns(bool) {
    
    uint256[] memory proof_inputs = new uint256[](4);
  
    proof_inputs[3] = old_root;
    proof_inputs[0] = new_root;

    bytes32 input_hash = sha256(transaction_inputs);

    (bytes16 second, bytes16 first) = splitToTwo128(input_hash); 
    
    second = toLibsnarkBits(second);
    first = toLibsnarkBits(first);
  
    proof_inputs[1] = uint128(first); 
    proof_inputs[2] = uint128(second);
    
    require(zkroot == bytes32(proof_inputs[3]), "current_root not match");

    require(Verifier.Verify(in_vk, vk_gammaABC, in_proof, proof_inputs), "verify failed");
    
    zkroot = bytes32(proof_inputs[0]);
    return true;
  }
}


