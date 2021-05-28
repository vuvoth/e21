pragma solidity ^0.5.0;

import "../libs/ethsnarks/contracts/Verifier.sol";


contract Main {
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

  function verifyTx(
    uint256[14] memory in_vk, 
    uint256[] memory vk_gammaABC, 
    uint256[8] memory in_proof, 
    uint256[] memory proof_inputs
  ) public returns(bool) {
    proof_inputs[9] = uint256(zkroot);
    require(Verifier.Verify(in_vk, vk_gammaABC, in_proof, proof_inputs), "verify failed");
    
    zkroot = bytes32(proof_inputs[0]);
    return true;
  }
}


